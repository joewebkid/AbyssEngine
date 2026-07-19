#!/usr/bin/env python3
"""sodiff.py -- whole-.so dynamic-symbol parity diff.

Compares the *defined* dynamic symbols our build exports against the original libgof2hdaa.so and
reports, reccmp-style:

    original / ours / extra / missing  (and missing split into wrong_type vs absent)

A symbol is "defined & exported" when it lives in .dynsym, is not LOCAL-bound, and is not undefined
(st_shndx != SHN_UNDEF). That is exactly the set a consumer of the .so can bind against, so matching
it means our library presents the same shape as the original.

Pure Python ELF32 parsing -- no nm/readelf/NDK tools, so it runs anywhere (incl. stock macOS).
Demangling is best-effort via c++filt/llvm-cxxfilt for display only; matching is on raw names.

Exit 0 iff extra == 0 and missing == 0 (subject to --allow, the documented irreducible delta).
"""
import argparse
import struct
import subprocess
import sys
from pathlib import Path

# ELF constants
SHT_DYNSYM = 11
STB_LOCAL = 0
SHN_UNDEF = 0


def _read_elf32_dynsym(path):
    """Return the list of (name, st_info, st_shndx) for every .dynsym entry in an ELF32 LE file."""
    data = Path(path).read_bytes()
    if data[:4] != b"\x7fELF":
        raise SystemExit(f"{path}: not an ELF file")
    if data[4] != 1:  # EI_CLASS: 1 = ELFCLASS32
        raise SystemExit(f"{path}: not ELF32 (only the armeabi-v7a target is supported)")
    if data[5] != 1:  # EI_DATA: 1 = little-endian
        raise SystemExit(f"{path}: not little-endian")

    # ELF32 header: e_shoff @0x20, e_shentsize @0x2e, e_shnum @0x30, e_shstrndx @0x32
    e_shoff = struct.unpack_from("<I", data, 0x20)[0]
    e_shentsize = struct.unpack_from("<H", data, 0x2E)[0]
    e_shnum = struct.unpack_from("<H", data, 0x30)[0]

    # Section header (ELF32_Shdr, 40 bytes): name,type,flags,addr,offset,size,link,info,addralign,entsize
    sections = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        sh_type, = struct.unpack_from("<I", data, off + 0x04)
        sh_offset, sh_size = struct.unpack_from("<II", data, off + 0x10)
        sh_link, = struct.unpack_from("<I", data, off + 0x18)
        sh_entsize, = struct.unpack_from("<I", data, off + 0x24)
        sections.append((sh_type, sh_offset, sh_size, sh_link, sh_entsize))

    dynsym = next((s for s in sections if s[0] == SHT_DYNSYM), None)
    if dynsym is None:
        raise SystemExit(f"{path}: no .dynsym section")
    _, sym_off, sym_size, str_link, entsize = dynsym
    entsize = entsize or 16
    strtab_off, strtab_size = sections[str_link][1], sections[str_link][2]
    strtab = data[strtab_off:strtab_off + strtab_size]

    def cstr(idx):
        end = strtab.find(b"\x00", idx)
        return strtab[idx:end].decode("utf-8", "replace")

    # ELF32_Sym (16 bytes): st_name,st_value,st_size,st_info,st_other,st_shndx
    out = []
    for off in range(sym_off, sym_off + sym_size, entsize):
        st_name, = struct.unpack_from("<I", data, off)
        st_info = data[off + 12]
        st_shndx, = struct.unpack_from("<H", data, off + 14)
        name = cstr(st_name)
        if name:
            out.append((name, st_info, st_shndx))
    return out


def defined_exports(path):
    """Names that are defined (not UNDEF) and exported (not LOCAL-bound) in the .dynsym."""
    names = set()
    for name, st_info, st_shndx in _read_elf32_dynsym(path):
        if st_shndx == SHN_UNDEF:
            continue
        if (st_info >> 4) == STB_LOCAL:
            continue
        names.add(name)
    return names


def demangle_map(names):
    """name -> demangled (best effort). Falls back to identity when no demangler is available."""
    for tool in ("c++filt", "llvm-cxxfilt"):
        try:
            p = subprocess.run([tool], input="\n".join(names), capture_output=True,
                               text=True, check=True)
            return dict(zip(names, p.stdout.splitlines()))
        except (FileNotFoundError, subprocess.CalledProcessError):
            continue
    return {n: n for n in names}


def qualified_name(demangled):
    """Strip the parameter list / return type from a demangled name, leaving Class::method.

    Used to tell a wrong-signature symbol (same qualified name, different params) from a genuinely
    absent one. 'void Foo::bar(int) const' -> 'Foo::bar'.
    """
    s = demangled
    depth = 0
    for i, c in enumerate(s):
        if c == "(" and depth == 0:
            s = s[:i]
            break
        depth += {"<": 1, ">": -1}.get(c, 0)
    s = s.strip()
    # drop a leading 'return-type ' (the last top-level space before the name, outside <...>)
    depth = 0
    cut = -1
    for i, c in enumerate(s):
        depth += {"<": 1, ">": -1, "(": 1, ")": -1}.get(c, 0)
        if c == " " and depth == 0:
            cut = i
    return s[cut + 1:] if cut >= 0 else s


def main():
    ap = argparse.ArgumentParser(description="dynamic-symbol parity diff vs the original .so")
    ap.add_argument("--original", required=True, help="the original libgof2hdaa.so")
    ap.add_argument("--ours", required=True, help="our built libgof2hdaa.so")
    ap.add_argument("--allow", help="file of mangled names permitted as extra (irreducible delta)")
    ap.add_argument("--list", action="store_true", help="print every extra/missing symbol")
    ap.add_argument("--max", type=int, default=40, help="max symbols to list per bucket (0 = all)")
    args = ap.parse_args()

    orig = defined_exports(args.original)
    ours = defined_exports(args.ours)

    allow = set()
    if args.allow and Path(args.allow).exists():
        allow = {ln.strip() for ln in Path(args.allow).read_text().splitlines()
                 if ln.strip() and not ln.startswith("#")}

    extra = (ours - orig) - allow
    missing = orig - ours

    dm = demangle_map(extra | missing)
    extra_q = {n: qualified_name(dm[n]) for n in extra}
    missing_q = {n: qualified_name(dm[n]) for n in missing}
    extra_qset = set(extra_q.values())
    wrong_type = {n for n in missing if missing_q[n] in extra_qset}
    absent = missing - wrong_type

    def dump(title, names):
        if not names:
            return
        print(f"\n{title} ({len(names)}):")
        shown = sorted(names, key=lambda n: dm.get(n, n))
        if args.max and not args.list:
            shown = shown[:args.max]
        for n in shown:
            print(f"  {dm.get(n, n)}")
            if dm.get(n, n) != n:
                print(f"      {n}")
        if args.max and not args.list and len(names) > args.max:
            print(f"  ... +{len(names) - args.max} more (use --list)")

    print(f"original: {len(orig)}  ours: {len(ours)}  "
          f"extra: {len(extra)}  missing: {len(missing)} "
          f"(wrong_type: {len(wrong_type)}  absent: {len(absent)})"
          + (f"  allowed: {len(allow)}" if allow else ""))
    dump("EXTRA (we export, original does not)", extra)
    dump("MISSING/wrong_type (same name, wrong signature)", wrong_type)
    dump("MISSING/absent (not emitted at all)", absent)

    ok = not extra and not missing
    print("\n" + ("SYMBOL PARITY ✓" if ok else "NOT yet symbol-identical"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
