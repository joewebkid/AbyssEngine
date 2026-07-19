#!/usr/bin/env python3
"""dump_asm.py — extract the ORIGINAL function's disassembly from the .so, for baking into wave
jobs as Ghidra-free ground truth. The .so IS the match target, so its asm is authoritative.

  get_asm(ghidra_addr_hex) -> str   # disassembly from vaddr to the next symbol (capped), or ""

vaddr = ghidra_addr - 0x10000 (Ghidra image base). Function end = the next distinct symbol address
in the symbols TSV (capped at MAX_BYTES so huge functions don't bloat the prompt).
"""
import bisect
import os
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
SO = "/Users/fionera/Downloads/GalaxyOnFire2/_work/bins/android_2.0.16_libgof2hdaa.so"
SYMS = "/Users/fionera/Downloads/GalaxyOnFire2/_work/symbols/android_2.0.16.symbols.tsv"
OBJDUMP = os.path.join(HERE, "verify", "orbobjdump")
GHIDRA_BASE = 0x10000
MAX_BYTES = 0x600  # cap a single function's asm window

_addrs = None


def _load_addrs():
    global _addrs
    if _addrs is None:
        s = set()
        for line in open(SYMS):
            p = line.split("\t")
            if len(p) >= 2:
                try:
                    s.add(int(p[0], 16))
                except ValueError:
                    pass
        _addrs = sorted(s)
    return _addrs


def get_asm(ghidra_addr_hex):
    if not ghidra_addr_hex:
        return ""
    vaddr = int(ghidra_addr_hex, 16) - GHIDRA_BASE
    addrs = _load_addrs()
    i = bisect.bisect_right(addrs, vaddr)
    end = addrs[i] if i < len(addrs) else vaddr + MAX_BYTES
    end = min(end, vaddr + MAX_BYTES)
    if end <= vaddr:
        end = vaddr + MAX_BYTES
    try:
        out = subprocess.run(
            [OBJDUMP, "-d", f"--start-address={hex(vaddr)}", f"--stop-address={hex(end)}", SO],
            capture_output=True, text=True, timeout=60).stdout
    except (OSError, subprocess.SubprocessError):
        return ""
    # keep only the disassembly lines (drop the file/section header noise)
    lines = [ln for ln in out.splitlines()
             if ln and (ln[0].isspace() or ln[:8].strip().rstrip(":").isalnum() and "<" in ln)]
    return "\n".join(lines).rstrip()


if __name__ == "__main__":
    import sys
    print(get_asm(sys.argv[1] if len(sys.argv) > 1 else "0x132960"))
