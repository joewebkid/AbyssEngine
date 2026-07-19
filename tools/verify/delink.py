#!/usr/bin/env python3
"""Delink original functions out of the stripped, fully-linked libgof2hdaa.so into
a relocatable ARM ELF .o that objdiff can diff against our compiled base .o.

For one base object (our compiled TU), we:
  1. list the function symbols it defines (arm nm),
  2. intersect them with the .so symbol table (address per mangled name),
  3. copy each function's raw bytes out of the .so (file_offset == ELF vaddr in .text),
  4. emit an assembly stub (.thumb_func/.arm + label(s) + .byte ...) and assemble it
     with arm-linux-gnueabihf-as into the target .o.

The target .o has the *same* mangled symbol names as our base .o, so objdiff matches
functions by name. Post-link the original has no relocations (calls/literal-pool loads
are absolute), so those instructions show as benign diffs — focus tuning on
opcode/register/immediate mismatches, not branch addresses.

Usage:
  delink.py --base <base.o> --out <target.o> [--names a,b,c]
            [--so ...] [--symbols ...] [--thumb-map ...]
            [--nm tools/verify/orbnm] [--as tools/verify/orbas] [--keep-asm]
"""
import argparse
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WORK = os.environ.get("GOF2_VERIFY_WORK", os.path.join(REPO, "_work"))
DEFAULT_SO = f"{WORK}/bins/android_2.0.16_libgof2hdaa.so"
DEFAULT_SYMS = f"{WORK}/symbols/android_2.0.16.symbols.tsv"
DEFAULT_THUMB = f"{WORK}/symbols/android_thumb_map.tsv"

if os.name == "nt":
    os.environ["PATH"] = r"C:\msys64\usr\bin;" + os.environ.get("PATH", "")


def _msys_path(path):
    path = os.path.abspath(path).replace("\\", "/")
    m = re.match(r"^([A-Za-z]):/(.*)$", path)
    if m:
        return f"/{m.group(1).lower()}/{m.group(2)}"
    return path


def tool_argv(tool, args):
    if os.name == "nt" and not tool.lower().endswith(".exe"):
        bash = os.environ.get("GOF2_BASH", r"C:\msys64\usr\bin\bash.exe")
        return [bash, _msys_path(tool), *args]
    return [tool, *args]


def find_text_section(so_path):
    """Return (sh_addr, sh_offset, sh_size) of .text by parsing ELF32 headers."""
    with open(so_path, "rb") as f:
        data = f.read()
    assert data[:4] == b"\x7fELF", "not an ELF"
    assert data[4] == 1, "expected ELF32"
    e_shoff = struct.unpack_from("<I", data, 0x20)[0]
    e_shentsize = struct.unpack_from("<H", data, 0x2E)[0]
    e_shnum = struct.unpack_from("<H", data, 0x30)[0]
    e_shstrndx = struct.unpack_from("<H", data, 0x32)[0]
    # section-header string table
    sh = e_shoff + e_shstrndx * e_shentsize
    strtab_off = struct.unpack_from("<I", data, sh + 16)[0]
    for i in range(e_shnum):
        base = e_shoff + i * e_shentsize
        name_off = struct.unpack_from("<I", data, base + 0)[0]
        name_end = data.index(b"\x00", strtab_off + name_off)
        name = data[strtab_off + name_off:name_end].decode()
        if name == ".text":
            sh_addr = struct.unpack_from("<I", data, base + 12)[0]
            sh_offset = struct.unpack_from("<I", data, base + 16)[0]
            sh_size = struct.unpack_from("<I", data, base + 20)[0]
            return sh_addr, sh_offset, sh_size
    raise RuntimeError(".text section not found")


def load_symbols(path):
    """addr2names: {int addr -> [mangled,...]}, sorted unique addr list."""
    addr2names = {}
    for line in open(path):
        parts = line.rstrip("\n").split("\t")
        if len(parts) < 2:
            continue
        addr = int(parts[0], 16)
        addr2names.setdefault(addr, []).append(parts[1])
    addrs = sorted(addr2names)
    return addr2names, addrs


def load_thumb(path):
    """mangled name -> bool is_thumb (thumb-map addrs are Ghidra = vaddr + 0x10000)."""
    t = {}
    for line in open(path):
        parts = line.rstrip("\n").split("\t")
        if len(parts) < 3:
            continue
        t[parts[2]] = parts[1] == "1"
    return t


def nm_text_symbols(nm_tool, obj):
    """Mangled names this object *defines* as code. Includes weak/vague-linkage
    symbols (W/V) — templates and inline functions like ArrayAdd<T> are weak, and
    skipping them would drop a large fraction of the comparable functions."""
    out = subprocess.check_output(tool_argv(nm_tool, ["--defined-only", obj]), text=True)
    names = []
    for line in out.splitlines():
        cols = line.split()
        if len(cols) >= 3 and cols[1] in ("t", "T", "W", "V"):
            names.append(cols[2])
    return names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--base", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--names", default=None, help="comma-separated; default: from --base via nm")
    ap.add_argument("--so", default=DEFAULT_SO)
    ap.add_argument("--symbols", default=DEFAULT_SYMS)
    ap.add_argument("--thumb-map", default=DEFAULT_THUMB)
    ap.add_argument("--nm", default=os.path.join(HERE, "orbnm"))
    ap.add_argument("--as", dest="as_tool", default=os.path.join(HERE, "orbas"))
    ap.add_argument("--keep-asm", action="store_true")
    args = ap.parse_args()

    addr2names, addrs = load_symbols(args.symbols)
    name2addr = {n: a for a, ns in addr2names.items() for n in ns}
    thumb = load_thumb(args.thumb_map)
    text_addr, text_off, text_size = find_text_section(args.so)
    text_end = text_addr + text_size
    so = open(args.so, "rb").read()

    if args.names:
        want = [n for n in args.names.split(",") if n]
    else:
        want = nm_text_symbols(args.nm, args.base)

    # Resolve to unique target addresses present in the .so .text.
    seen_addr = set()
    funcs = []  # (addr, [names], bytes, is_thumb)
    missing = []
    for name in want:
        a = name2addr.get(name)
        if a is None:
            missing.append(name)
            continue
        if not (text_addr <= a < text_end):
            missing.append(name)
            continue
        if a in seen_addr:
            continue
        seen_addr.add(a)
        # size = gap to next distinct symbol address (clamp to .text end)
        idx = addrs.index(a)
        nxt = text_end
        for j in range(idx + 1, len(addrs)):
            if addrs[j] > a:
                nxt = min(addrs[j], text_end)
                break
        foff = text_off + (a - text_addr)
        raw = bytearray(so[foff:foff + (nxt - a)])
        # Drop trailing inter-function padding, but only whole 16-bit zero
        # halfwords — trimming an odd number of bytes would desync Thumb decode.
        while len(raw) >= 2 and raw[-1] == 0 and raw[-2] == 0:
            del raw[-2:]
        if not raw:
            continue
        is_thumb = thumb.get(name, True)  # default thumb (4501/4524)
        funcs.append((a, addr2names[a], raw, is_thumb))

    funcs.sort()

    lines = [".syntax unified", ".section .text", ""]
    for idx, (a, names, raw, is_thumb) in enumerate(funcs):
        primary = names[0]
        # Force per-function alignment so a preceding function's length can never
        # desync this one's Thumb/ARM decode.
        lines.append(".balign 4")
        lines.append(".thumb" if is_thumb else ".arm")
        for n in names:
            lines.append(f".global {n}")
            lines.append(f".type {n}, %function")
            if is_thumb:
                lines.append(".thumb_func")
            lines.append(f"{n}:")
        # ARM mapping symbol ($t/$a) so objdump/objdiff disassemble raw bytes in the
        # right mode — .thumb_func alone does not emit one for a .byte payload.
        lines.append(f"${'t' if is_thumb else 'a'}.{idx}:")
        body = ",".join(f"0x{b:02x}" for b in raw)
        lines.append(f".byte {body}")
        lines.append(f".size {primary}, .-{primary}")
        lines.append("")

    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    asm_path = args.out + ".s"
    with open(asm_path, "w") as f:
        f.write("\n".join(lines))

    subprocess.check_call(tool_argv(args.as_tool, [
        "-march=armv7-a", "-mfpu=vfpv3", asm_path, "-o", args.out
    ]))
    if not args.keep_asm:
        os.remove(asm_path)

    print(f"[delink] {os.path.relpath(args.base, REPO)}: "
          f"{len(funcs)} funcs -> {os.path.relpath(args.out, REPO)} "
          f"({len(missing)} not in .so/.text)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
