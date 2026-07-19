#!/usr/bin/env python3
"""drift_scan.py -- find struct fields whose name encodes an offset (field_0xNN, byte_0xNN, ...)
that does NOT match the field's actual compiled offset.

Such "drift" means the struct has missing/mis-sized members upstream, so accesses land at the wrong
byte vs the original binary. It is invisible to symbol-parity (function-level), so we check it by
compiling `__builtin_offsetof` for every hex-named member against the offset in its name, under the
real ARM32 target. Reports each (Class, member, named-offset, actual-offset) mismatch.
"""
import re
import subprocess
import sys
import tempfile
import os
from pathlib import Path

ROOT = Path("src")
INCLUDES = ["-Isrc", "-Ithird_party/fmod/inc", "-Ithird_party/gl", "-Ithird_party/jni",
            "-Ithird_party/libzip", "-Ithird_party/crypto"]
CXX = "clang++"
# No -DGOF2_MATCH: the real build doesn't define it (it'd activate a known-failing Transform assert).
# -include cstdint/cstddef so headers that forgot them still compile standalone.
FLAGS = ["-std=c++14", "-m32", "-fsyntax-only", "-ferror-limit=0",
         "-Wno-everything", "-Winvalid-offsetof", "-include", "cstdint", "-include", "cstddef"]

# a data-member line: <type ...> <name>[ [..] ];   (no '(' => not a method; no '=', static, typedef)
MEMBER = re.compile(r'^\s*([A-Za-z_][\w:<>,*& ]*?[ *&])([A-Za-z_]\w*)\s*(\[[^\]]*\])?\s*;\s*$')
HEXNAME = re.compile(r'_0x([0-9a-fA-F]+)')          # the offset encoded in a member name
CLASS_OPEN = re.compile(r'\b(?:class|struct)\s+([A-Za-z_]\w*)\s*(?:final\s*)?(?::[^{]*)?\{')
NS_OPEN = re.compile(r'\bnamespace\s+([A-Za-z_]\w*)\s*\{')
NONSTD = set()


def members_of(path):
    """Yield (qualified_class, member, named_offset) for hex-named data members in a header."""
    text = path.read_text(errors="replace")
    text = re.sub(r'//.*', '', text)
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    stack = []          # list of (kind, name, depth_when_opened)  kind: 'ns' | 'cls'
    depth = 0
    for ln in text.splitlines():
        # member detection: only when the innermost scope is a class/struct
        if stack and stack[-1][0] == 'cls':
            mm = MEMBER.match(ln)
            if mm and not any(t in ln for t in ('static', 'typedef', 'using', 'return')) \
                    and '=' not in ln and 'union' not in mm.group(1) and 'struct' not in mm.group(1):
                hx = HEXNAME.search(mm.group(2))
                if hx:
                    qual = "::".join(n for _k, n, _d in stack)
                    yield qual, mm.group(2), int(hx.group(1), 16)
        # scope changes on this line: push ns/class at the current depth, then apply braces
        mns = NS_OPEN.search(ln)
        mcl = CLASS_OPEN.search(ln)
        if mns:
            stack.append(('ns', mns.group(1), depth))
        if mcl:
            stack.append(('cls', mcl.group(1), depth))
        depth += ln.count('{') - ln.count('}')
        while stack and stack[-1][2] >= depth:
            stack.pop()


def main():
    checks = []   # (header, qualclass, member, named)
    headers = sorted(ROOT.rglob("*.h"))
    for h in headers:
        try:
            for qual, member, named in members_of(h):
                checks.append((h, qual, member, named))
        except Exception as e:
            print(f"parse-skip {h}: {e}", file=sys.stderr)

    # group by header; emit one TU per header so class names resolve in their own context
    drifts = []
    by_header = {}
    for h, q, m, n in checks:
        by_header.setdefault(h, []).append((q, m, n))
    for h, items in by_header.items():
        rel = h.relative_to(ROOT).as_posix()
        body = [f'#include "{rel}"', "template<unsigned long N> struct DRIFT_OFFSET;"]
        decls = []
        for i, (q, m, n) in enumerate(items):
            # static_assert names the expected offset; failures => actual printed via the assert msg's offsetof
            decls.append(f'static_assert(__builtin_offsetof({q}, {m}) == 0x{n:x}, '
                         f'"DRIFT {q}::{m} name=0x{n:x}");')
        src = "\n".join(body + decls) + "\n"
        with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False, dir="/tmp") as f:
            f.write(src); tmp = f.name
        out = subprocess.run([CXX] + FLAGS + INCLUDES + [tmp], capture_output=True, text=True)
        os.unlink(tmp)
        # classes flagged non-standard-layout: offsetof returns 0 there -> results unreliable (need DWARF)
        for w in re.findall(r"non-standard-layout type '([\w:]+)'", out.stderr):
            NONSTD.add(w.split("::")[-1])
        got_drift = False
        for line in out.stderr.splitlines():
            # clang: error: static assertion failed due to requirement
            #        '__builtin_offsetof(Class, member) == 196': "DRIFT Class::member name=0xc1"
            mm = re.search(r'offsetof\(([\w:]+),\s*(\w+)\)\s*==\s*(\d+)\D.*"DRIFT', line)
            if mm:
                drifts.append((rel, mm.group(1), mm.group(2),
                               int(mm.group(3)), None)); got_drift = True
                continue
            mm2 = re.search(r'"DRIFT ([\w:]+)::(\w+) name=0x([0-9a-f]+)"', line)
            if mm2 and not got_drift:
                drifts.append((rel, mm2.group(1) + "::" + mm2.group(2), "", None,
                               int(mm2.group(3), 16)))
        # surface non-drift compile errors (header not self-contained)
        non_drift = [l for l in out.stderr.splitlines()
                     if "error:" in l and "DRIFT" not in l and "offsetof" not in l]
        if non_drift:
            print(f"compile-skip {rel}: {non_drift[0].split('error:')[-1].strip()[:90]}",
                  file=sys.stderr)

    # drifts entries: (rel, classOrQual, member, actual_or_None, named_or_None)
    rows, skipped = [], set()
    for rel, a, b, actual, named in drifts:
        cls = a if actual is not None else a.rsplit("::", 1)[0]
        short = cls.split("::")[-1]
        if short in NONSTD:                         # offsetof unreliable here -> needs DWARF
            skipped.add(short); continue
        if actual is not None:
            mem = b
            nm = HEXNAME.search(mem)
            named = int(nm.group(1), 16) if nm else named
            rows.append((rel, cls, mem, named, actual))
        else:
            rows.append((rel, a, b, named, None))
    rows = sorted(set(rows))
    print(f"\n=== DRIFT (standard-layout, reliable): {len(rows)} field(s) ===")
    for rel, cls, mem, named, actual in rows:
        ns = f"0x{named:x}" if named is not None else "?"
        ac = f"0x{actual:x}" if actual is not None else "?"
        print(f"  {cls}::{mem}  name={ns}  actual={ac}  ({rel})")
    print(f"\n=== {len(NONSTD)} non-standard-layout classes (offsetof unreliable; DWARF needed): ===")
    print("  " + ", ".join(sorted(NONSTD)))
    return 1 if rows else 0


if __name__ == "__main__":
    sys.exit(main())
