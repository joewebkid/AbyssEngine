#!/usr/bin/env python3
"""layout.py -- authoritative field-offset / drift checker using clang -fdump-record-layouts.

Unlike __builtin_offsetof (UB / returns 0 on non-standard-layout types), -fdump-record-layouts prints
the real offset of every field of every record, polymorphic or not. We parse it, then for each
hex-named member (field_0xNN / byte_0xNN / flag_0xNN / image_0xNN / pad_0xNN ...) compare the actual
offset to the offset encoded in the name. Mismatch = drift.

Usage:
  tools/layout.py                 # scan all headers, list drift
  tools/layout.py Gun KIPlayer    # dump the resolved layouts of named classes (offset | type name)
"""
import re
import subprocess
import sys
import tempfile
import os
from pathlib import Path

ROOT = Path("src")
INC = ["-Isrc", "-Ithird_party/fmod/inc", "-Ithird_party/gl", "-Ithird_party/jni",
       "-Ithird_party/libzip", "-Ithird_party/crypto"]
FLAGS = ["-std=c++14", "-m32", "-include", "cstdint", "-include", "cstddef",
         "-Wno-everything", "-Xclang", "-fdump-record-layouts", "-fsyntax-only"]
HEXNAME = re.compile(r'_0x([0-9a-fA-F]+)')
# a field line in the dump:  "        64 |   char field_0x3f"  (offset | indent type... name)
FIELD = re.compile(r'^\s*(\d+) \|(\s+)([^\n]*?)\b([A-Za-z_]\w*)\s*$')
HEAD = re.compile(r'^\s*0 \| (?:class|struct|union) ([A-Za-z_][\w:]*)')


def dump_layouts(header):
    """Return {classname: {field: offset}} for all records visible from `header`."""
    with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False, dir="/tmp") as f:
        f.write(f'#include "{header}"\n'); tmp = f.name
    out = subprocess.run(["clang++"] + FLAGS + INC + [tmp], capture_output=True, text=True)
    os.unlink(tmp)
    classes = {}
    cur = None
    base_indent = None
    for ln in out.stdout.splitlines():
        if "Dumping AST Record Layout" in ln:
            cur = None
            base_indent = None
            continue
        h = HEAD.match(ln)
        if h:
            cur = h.group(1)
            classes.setdefault(cur, {})
            continue
        if cur is None:
            continue
        fm = FIELD.match(ln)
        if fm:
            off = int(fm.group(1))
            name = fm.group(4)
            # ignore vtable-pointer / base-class pseudo-lines and bit-field noise
            if "vtable pointer" in ln or "(base class)" in ln:
                continue
            # keep the FIRST occurrence (top-level), don't overwrite with nested dup names
            classes[cur].setdefault(name, off)
    return classes


def main():
    args = sys.argv[1:]
    # build a class -> header map by scanning which header defines each class.
    # pre-filter: only headers that textually contain a hex-named member (huge speedup).
    hdrs = [h for h in sorted(ROOT.rglob("*.h"))
            if (args or re.search(r'\b\w+_0x[0-9a-fA-F]+\b', h.read_text(errors="replace")))]
    all_classes = {}
    for h in hdrs:
        rel = h.relative_to(ROOT).as_posix()
        try:
            layouts = dump_layouts(rel)
        except Exception:
            continue
        for cls, fields in layouts.items():
            # only record a class once, from a header that actually defines it (has fields)
            if fields and cls not in all_classes:
                all_classes[cls] = (rel, fields)

    if args:  # dump mode
        for want in args:
            for cls, (rel, fields) in all_classes.items():
                if cls.split("::")[-1] == want:
                    print(f"\n=== {cls}  ({rel}) ===")
                    for name, off in sorted(fields.items(), key=lambda kv: kv[1]):
                        mark = ""
                        hx = HEXNAME.search(name)
                        if hx and int(hx.group(1), 16) != off:
                            mark = f"   <-- DRIFT name=0x{int(hx.group(1),16):x} actual=0x{off:x}"
                        print(f"  0x{off:<5x} {name}{mark}")
        return 0

    drift = []
    for cls, (rel, fields) in all_classes.items():
        for name, off in fields.items():
            hx = HEXNAME.search(name)
            if hx:
                named = int(hx.group(1), 16)
                if named != off:
                    drift.append((cls, name, named, off, rel))
    by_cls = {}
    for cls, name, named, off, rel in drift:
        by_cls.setdefault((cls, rel), []).append((name, named, off))
    print(f"=== DRIFT: {len(drift)} field(s) across {len(by_cls)} class(es) ===")
    for (cls, rel), items in sorted(by_cls.items(), key=lambda kv: -len(kv[1])):
        print(f"\n{cls}  ({rel})  [{len(items)}]")
        for name, named, off in sorted(items, key=lambda t: t[2]):
            print(f"  {name}  name=0x{named:x}  actual=0x{off:x}")
    return 1 if drift else 0


if __name__ == "__main__":
    sys.exit(main())
