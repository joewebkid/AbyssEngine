#!/usr/bin/env python3
"""Disassemble two ARM objects with arm-linux-gnueabihf-objdump and compare them
function-by-function. Used by verify.py.

objdiff cannot read this binary (its ARM disassembler caps at ARMv6K; the game is
ARMv7-A Thumb-2 + VFP/NEON), so we disassemble with GNU objdump — which fully
supports ARMv7 — and do our own normalize + fuzzy match.

For each symbol present in BOTH objects we report:
  * bytes_equal  : raw instruction bytes identical (the gold byte-match signal)
  * match        : fuzzy instruction match 0..100 (difflib ratio over normalized
                   mnemonics+operands; tolerant of post-link absolute branch/pool
                   addresses that have no relocation in the delinked target)
"""
import difflib
import os
import re
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_OBJDUMP = os.path.join(HERE, "orbobjdump")

if os.name == "nt":
    os.environ["PATH"] = r"C:\msys64\usr\bin;" + os.environ.get("PATH", "")

_BRANCH = re.compile(r"^(b|bl|blx|bx|cb|cbz|cbnz|beq|bne|bcs|bhs|bcc|blo|bmi|bpl|"
                     r"bvs|bvc|bhi|bls|bge|blt|bgt|ble|bal)(\.[wn])?$")
_HEXNUM = re.compile(r"0x[0-9a-fA-F]+")
_ANGLE = re.compile(r"\s*<[^>]*>")          # objdump symbol annotation
_PCREL = re.compile(r"\[pc,\s*#-?\d+\]")    # literal-pool load offset
_REG = re.compile(r"^(r\d+|sp|lr|pc|fp|ip|sl|sb)$", re.I)  # ARM register operand
_RELOC = re.compile(r"^\s*([0-9a-f]+):\s+R_ARM_\S+")       # objdump -r inline reloc
_RELOC_WIDTH = 4   # all R_ARM relocs in these objects (THM_CALL/JUMP24/GOT_PREL/REL32) are 4-byte fields


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


def normalize(mnem, ops):
    """Reduce one instruction to a comparable token, neutralizing addresses that
    differ only because the target is post-link (no relocations)."""
    ops = ops.split(";")[0].split("@")[0].strip()      # drop comments
    ops = _ANGLE.sub("", ops)                          # drop <sym+0x..> notes
    ops = re.sub(r"\s+", " ", ops).strip()
    base = mnem.split(".")[0]
    if _BRANCH.match(mnem) or base in ("b", "bl", "blx", "bx"):
        # A direct bl/blx to a *label* is link-time-interchangeable: our unlinked .o
        # emits `bl`+relocation, the delinked target shows the linker-resolved `blx`
        # interworking veneer. That is not a codegen difference, so collapse both to a
        # single `call` token. A register-indirect blx/bx (blx r3, bx lr) IS a real
        # codegen choice -- keep its register. Other branches: target -> placeholder.
        if base in ("bl", "blx") and not _REG.match(ops):
            return "call <t>"
        if base in ("blx", "bx") and _REG.match(ops):
            return f"{base} {ops}"
        return base + " <t>"                           # branch/call target -> placeholder
    # pointer materialization & pool loads carry absolute values post-link
    if base in ("movw", "movt"):
        ops = re.sub(r"#\S+", "#i", ops)
    ops = _PCREL.sub("[pc]", ops)
    ops = _HEXNUM.sub("#x", ops)
    return f"{mnem} {ops}".strip()


def _symbol_sizes(lines):
    """Parse `objdump -t` symbol table -> {name: size_bytes}."""
    sizes, in_tab = {}, False
    for line in lines:
        if line.startswith("SYMBOL TABLE:"):
            in_tab = True
            continue
        if in_tab:
            if not line.strip() or line.startswith("Disassembly"):
                in_tab = False
                continue
            # "0000000a g     F .text\t0000001c _ZN..."
            if "\t" not in line:
                continue
            right = line.split("\t", 1)[1]
            parts = right.split(None, 1)
            if len(parts) == 2:
                try:
                    sizes[parts[1].strip()] = int(parts[0], 16)
                except ValueError:
                    pass
    return sizes


DISASM_TIMEOUT = 90   # seconds; objdump normally finishes in <2s — a longer run is a hang
                      # (a malformed object or a wedged tool call) and must not freeze the run.


def disassemble(obj, objdump=DEFAULT_OBJDUMP):
    """{mangled symbol -> {'insns':[normalized...], 'bytes':'hex'}}.

    Each function is truncated to its real symbol size so trailing alignment
    padding (attributed by objdump to the preceding symbol) never pollutes the
    comparison.

    'reloc' is the set of function-relative byte offsets covered by a relocation
    in this (unlinked) object — the fields whose final value the linker fills in.
    They are wildcarded by linked_equal() so an external call / global load doesn't
    read as a byte mismatch against the post-link target."""
    # stderr is captured (not inherited) so a failing object's objdump message ("No such file",
    # bad format, ...) doesn't interleave into the report table; the failure is reported as a
    # concise per-unit skip instead.
    out = subprocess.check_output(tool_argv(objdump, ["-d", "-t", "-r", obj]), text=True,
                                  timeout=DISASM_TIMEOUT, stderr=subprocess.DEVNULL)
    lines = out.splitlines()
    sizes = _symbol_sizes(lines)
    funcs, cur, start, limit = {}, None, 0, None
    addr2label = {}   # text address -> the one name objdump put on the `<name>:` header
    for line in lines:
        m = re.match(r"^([0-9a-f]+) <(.+)>:$", line)
        if m:
            start = int(m.group(1), 16)
            sz = sizes.get(m.group(2))
            limit = start + sz if sz else None
            cur = {"insns": [], "bytes": "", "reloc": set()}
            funcs[m.group(2)] = cur
            addr2label[start] = m.group(2)
            continue
        if cur is None:
            continue
        # "   a:\t68 0a       \tldr\tr2, [r1, #0]"   (objdump -d, raw bytes shown)
        m = re.match(r"^\s*([0-9a-f]+):\t([0-9a-f ]+)\t(\S+)\s*(.*)$", line)
        if not m:
            # "\t\t\t22: R_ARM_THM_JUMP24\t_ZN..." -> mask this 4-byte field
            rm = _RELOC.match(line)
            if rm:
                off = int(rm.group(1), 16) - start
                if off >= 0 and (limit is None or start + off < limit):
                    cur["reloc"].update(range(off, off + _RELOC_WIDTH))
            continue
        addr = int(m.group(1), 16)
        if limit is not None and addr >= limit:
            continue
        raw, mnem, ops = m.group(2), m.group(3), m.group(4)
        if mnem in (".word", ".short", ".byte"):
            cur["insns"].append(mnem + " " + _HEXNUM.sub("#x", ops).strip())
        else:
            cur["insns"].append(normalize(mnem, ops))
        cur["bytes"] += raw.replace(" ", "")

    # Credit aliased function symbols. objdump's `<name>:` disassembly header carries only ONE name
    # per address, but the compiler aliases C1/C2 (complete/base ctor) and D1/D2 (complete/base dtor)
    # — and, with -Oz, sometimes emits only one of a pair — so a sibling that shares the address is
    # byte-identical but invisible. Parse the `-t` symbol table (in the same objdump output) for all
    # function names at each text address and point every sibling at the labelled entry, so a match
    # under one variant name credits all of them.
    for line in lines:
        m = re.match(r"^([0-9a-f]+)\s.{6,}\bF\s+\.text\s+[0-9a-f]+\s+(\S+)\s*$", line)
        if not m:
            continue
        addr, name = int(m.group(1), 16), m.group(2)
        label = addr2label.get(addr)
        if label and name not in funcs and label in funcs:
            funcs[name] = funcs[label]
    return funcs


def linked_equal(t, b):
    """True if the raw bytes match except inside relocation-covered fields, which
    legitimately differ: our unlinked .o (b) carries placeholder operands where the
    delinked, post-link target (t) has the linker-resolved absolute value. Equivalent
    to byte-identical machine code *after linking* — the real goal — and unlike a raw
    byte compare it doesn't false-negative on every function that calls out or loads a
    global. Relocation sites come from the base side (the post-link target has none)."""
    tb, bb, mask = t["bytes"], b["bytes"], b.get("reloc") or set()
    if len(tb) != len(bb):
        return False
    for i in range(len(bb) // 2):            # byte index; hex chars [2i:2i+2]
        if i not in mask and tb[2 * i:2 * i + 2] != bb[2 * i:2 * i + 2]:
            return False
    return True


def compare(target_funcs, base_funcs):
    """List of per-symbol dicts for symbols defined in both, sorted worst-first."""
    rows = []
    for sym, b in base_funcs.items():
        t = target_funcs.get(sym)
        if t is None:
            continue
        ratio = difflib.SequenceMatcher(None, t["insns"], b["insns"]).ratio()
        rows.append({
            "symbol": sym,
            "match": round(ratio * 100, 1),
            "bytes_equal": t["bytes"] == b["bytes"],
            "linked_equal": linked_equal(t, b),
            "n_target": len(t["insns"]),
            "n_base": len(b["insns"]),
        })
    rows.sort(key=lambda r: (r["match"], r["symbol"]))
    return rows


def unified(target_funcs, base_funcs, sym):
    """Human-readable side-by-side diff of one symbol's normalized disassembly."""
    t = target_funcs.get(sym, {}).get("insns", [])
    b = base_funcs.get(sym, {}).get("insns", [])
    diff = difflib.unified_diff(t, b, fromfile="ORIGINAL (.so)", tofile="OURS (build)",
                                lineterm="")
    return "\n".join(diff)
