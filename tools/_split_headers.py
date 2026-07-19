#!/usr/bin/env python3
"""Split multi-class headers so each .h defines exactly one class (criterion 1).

For header H.h defining [primary + secondaries], extract each SECONDARY class into its own
<Name>.h (same dir) and replace its definition in H.h with `#include "<Name>.h"`. Includers of
H.h transitively still see the secondaries, so NO includer edits and NO symbol/layout/linkage
change (parity-safe). The extracted header carries H.h's own #includes (so deps resolve) plus
forward-declarations of the sibling classes (covers pointer cross-refs). Primary = the class whose
name matches the file stem, else the last/largest.

Run with --dry to preview. Interim tool.
"""
import os, re, sys, glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src")
DRY = "--dry" in sys.argv

CLASS_RE = re.compile(r"(^[ \t]*)(class|struct)\s+([A-Za-z_]\w*)\b(?:\s*:[^{;]*)?\s*\{", re.M)
INCLUDE_RE = re.compile(r'^[ \t]*#\s*include\b.*$', re.M)


def find_class_extent(txt, brace_pos):
    """Given index of the opening '{', return index just past the closing '};'."""
    depth = 0
    i = brace_pos
    n = len(txt)
    while i < n:
        c = txt[i]
        if c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                # consume the trailing ';'
                j = i + 1
                while j < n and txt[j] in ' \t':
                    j += 1
                if j < n and txt[j] == ';':
                    return j + 1
                return i + 1
        i += 1
    return -1


def enclosing_namespace(txt, pos):
    ns = []
    depth = []
    for m in re.finditer(r"namespace\s+([A-Za-z_]\w*)\s*\{|\{|\}", txt[:pos]):
        s = m.group(0)
        if s.startswith("namespace"):
            ns.append(m.group(1)); depth.append(True)
        elif s == "{":
            depth.append(False)
        else:
            if depth:
                wasns = depth.pop()
                if wasns and ns:
                    ns.pop()
    return ns


def process(fp):
    txt = open(fp).read()
    stem = os.path.splitext(os.path.basename(fp))[0]
    # top-level class/struct definitions (not nested inside another class/namespace-depth>1 ok)
    defs = []
    for m in CLASS_RE.finditer(txt):
        name = m.group(3)
        end = find_class_extent(txt, txt.index('{', m.start()))
        if end < 0:
            return None
        defs.append((name, m.start(), end, m.group(2)))
    if len(defs) < 2:
        return None
    # choose primary
    primary = next((d for d in defs if d[0] == stem), None)
    if primary is None:
        primary = max(defs, key=lambda d: d[2] - d[1])
    secondaries = [d for d in defs if d is not primary]
    incs = INCLUDE_RE.findall(txt)
    sibling_names = [d[0] for d in defs]
    reldir = os.path.relpath(os.path.dirname(fp), SRC)
    # build replacements (process from last to first to keep offsets valid)
    new_headers = []
    repls = []
    for (name, s, e, tag) in secondaries:
        ns = enclosing_namespace(txt, s)
        body = txt[s:e]
        # strip leading indentation of the def block's first line already in body
        guard = "GOF2_SPLIT_" + name.upper() + "_H"
        fwd = ""
        if ns:
            fwd_inner = "".join(f"    class {n};\n" for n in sibling_names if n != name)
            nsopen = "".join(f"namespace {p} {{\n" for p in ns)
            nsclose = "}\n" * len(ns)
            block = f"#ifndef {guard}\n#define {guard}\n" + "\n".join(incs) + "\n\n"
            block += nsopen
            for n in sibling_names:
                if n != name:
                    block += f"    class {n};\n"
            # re-indent body (body already indented at its original level)
            block += body.strip("\n") + "\n"
            block += nsclose
            block += f"#endif\n"
        else:
            block = f"#ifndef {guard}\n#define {guard}\n" + "\n".join(incs) + "\n\n"
            for n in sibling_names:
                if n != name:
                    block += f"class {n};\n"
            block += body.strip("\n") + "\n"
            block += f"#endif\n"
        new_fp = os.path.join(os.path.dirname(fp), name + ".h")
        incpath = os.path.join(reldir, name + ".h").replace("\\", "/")
        new_headers.append((new_fp, block))
        repls.append((s, e, f'#include "{incpath}"'))
    # apply replacements in reverse
    out = txt
    for (s, e, rep) in sorted(repls, key=lambda x: -x[0]):
        out = out[:s] + rep + out[e:]
    return out, new_headers


def main():
    targets = sys.argv[1:]
    targets = [t for t in targets if not t.startswith("--")]
    if not targets:
        # all multiclass headers (rough: >1 top-level class def)
        targets = []
        EXCLUDE = {"Transform.h", "CheatHandler.h"}  # collide with existing canonical headers; handle manually
        for fp in glob.glob(os.path.join(SRC, "**", "*.h"), recursive=True):
            if os.path.basename(fp) in EXCLUDE:
                continue
            if len(CLASS_RE.findall(open(fp).read())) > 1:
                targets.append(fp)
    changed = 0
    for fp in targets:
        r = process(fp)
        if not r:
            continue
        out, new_headers = r
        rel = os.path.relpath(fp, ROOT)
        print(f"{rel}: extract {[os.path.basename(h) for h,_ in new_headers]}")
        changed += 1
        if not DRY:
            for nh, block in new_headers:
                if os.path.exists(nh):
                    print(f"   SKIP exists: {os.path.relpath(nh, ROOT)}")
                    continue
                open(nh, "w").write(block)
            open(fp, "w").write(out)
    print(f"\n{'DRY ' if DRY else ''}headers split: {changed}")


if __name__ == "__main__":
    main()
