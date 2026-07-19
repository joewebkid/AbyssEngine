#!/usr/bin/env python3
"""offdrift.py -- surface struct layout drift that field_0xNN scanning can't see.

For every size-matched, not-byte-exact function, disassemble ours and the original, align the
instruction streams 1:1 (equal size => equal count in practice), and collect load/store immediate
offsets that differ (ldr/str/ldrb/strb/ldrh ... [rN, #imm]). A function with several consistent
offset deltas is reading a struct whose layout drifts vs the original. Aggregating across all
functions ranks the worst offenders so one struct-layout fix can flip many functions to byte-exact.

Usage: tools/offdrift.py [--limit N] [--fn MANGLED]
"""
import json
import re
import subprocess
import sys
import tempfile
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "verify"))
from elf import Elf32  # noqa

ORIG = "../_work/bins/android_2.0.16_libgof2hdaa.so"
OURS = "cmake-build-match/libgof2hdaa.so"
OBJ = subprocess.run(["bash", "-lc", "grep -m1 GOF2_NDK_OBJDUMP cmake-build-match/CMakeCache.txt|cut -d= -f2"],
                     capture_output=True, text=True).stdout.strip()
LDST = re.compile(r'\b(ldr|str|ldrb|strb|ldrh|strh|ldrd|strd|ldr\.w|str\.w|ldrb\.w|strb\.w)\b'
                  r'[^\[]*\[\w+,\s*#(\d+)\]')


def disasm(elf, fn):
    f = elf.functions().get(fn)
    if not f:
        return None
    b = elf.bytes_at(f["value"], f["size"])
    t = tempfile.NamedTemporaryFile(suffix=".bin", delete=False)
    t.write(b); t.close()
    out = subprocess.run([OBJ, "-D", "-b", "binary", "-m", "arm", "-M", "force-thumb", t.name],
                         capture_output=True, text=True).stdout
    os.unlink(t.name)
    insns = []
    for l in out.splitlines():
        if ":\t" in l:
            insns.append(l.split("\t", 1)[-1].strip())
    return insns


def offsets(insn):
    """Return list of (mnemonic-ish, imm) for load/store immediate offsets."""
    m = LDST.search(insn.split(";")[0])
    return (m.group(1), int(m.group(2))) if m else None


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("--limit", type=int, default=400)
    ap.add_argument("--fn")
    a = ap.parse_args()
    rows = json.load(open("cmake-build-match/verify/report.json"))["rows"]
    eo = Elf32(ORIG); eu = Elf32(OURS)
    cand = ([{"name": a.fn}] if a.fn else
            [r for r in rows if r["osize"] == r["usize"] and not r["byte"]][:a.limit])
    func_drift = []          # (n_mismatch, fn, [(o,u)...])
    for r in cand:
        fn = r["name"]
        o = disasm(eo, fn); u = disasm(eu, fn)
        if not o or not u or len(o) != len(u):
            continue
        pairs = []
        for io, iu in zip(o, u):
            po = offsets(io); pu = offsets(iu)
            if po and pu and po[0] == pu[0] and po[1] != pu[1]:
                pairs.append((po[1], pu[1]))
        if pairs:
            func_drift.append((len(pairs), fn, pairs))
    func_drift.sort(key=lambda x: -x[0])
    # aggregate delta histogram
    from collections import Counter
    deltas = Counter()
    for n, fn, pairs in func_drift:
        for o, u in pairs:
            deltas[o - u] += 1
    print(f"=== {len(func_drift)} functions with load/store offset drift (of {len(cand)} checked) ===")
    print("delta histogram (orig_off - our_off : count):",
          dict(sorted(deltas.items(), key=lambda kv: -kv[1])[:8]))
    print("\nworst functions (mismatched load/stores):")
    for n, fn, pairs in func_drift[:25]:
        ex = ", ".join(f"0x{o:x}->0x{u:x}" for o, u in pairs[:5])
        print(f"  {n:3}  {fn[:64]}\n        {ex}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
