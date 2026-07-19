#!/usr/bin/env python3
"""Interim forward-declaration inserter (Phase 1 build-green scaffolding).

For each header, forward-declare known non-template classes that are used by
POINTER or REFERENCE but not declared/defined in that file. Placed in the
class's real namespace. Pointer/ref-only usage is exactly the case a forward
declaration legitimately covers and is the one that breaks include cycles.

Run with --dry to preview. NOT a permanent tool; superseded by Phase 3e.
"""
import os, re, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src")
DRY = "--dry" in sys.argv

CLASS_DEF = re.compile(r"^([ \t]*)(class|struct)\s+([A-Za-z_]\w*)\b(?:\s*:[^{;]*)?\s*\{", re.M)
NS_OPEN = re.compile(r"\bnamespace\s+([A-Za-z_]\w*)\s*\{")
INCLUDE_LINE = re.compile(r'^\s*#\s*include\b.*$', re.M)

# Ubiquitous value-types: include the canonical header (provides full def AND,
# for Vector/Matrix, the global `using` alias) instead of forward-declaring.
VALUE_TYPE_HEADER = {
    "Vector": "engine/math/Vector.h",
    "Matrix": "engine/math/Matrix.h",
    "Quaternion": "engine/math/Quaternion.h",
    "String": "engine/core/AEString.h",
}


def src_files(exts):
    for dp, _, fns in os.walk(SRC):
        for fn in fns:
            if os.path.splitext(fn)[1] in exts:
                yield os.path.join(dp, fn)


def enclosing_namespace(txt, pos):
    """Best-effort: namespace path open at byte offset pos (brace tracking)."""
    depth_ns = []
    i = 0
    stack = []  # (name_or_None, brace_depth_at_open)
    brace = 0
    # walk char by char up to pos tracking namespace via simple scan
    # cheaper: find all 'namespace X {' and '}' and track
    tokens = []
    for m in re.finditer(r"namespace\s+([A-Za-z_]\w*)\s*\{|\{|\}", txt[:pos]):
        s = m.group(0)
        if s.startswith("namespace"):
            tokens.append(("ns", m.group(1)))
        elif s == "{":
            tokens.append(("open", None))
        else:
            tokens.append(("close", None))
    nsstack = []
    bstack = []  # parallel: True if this open brace was a namespace
    for kind, name in tokens:
        if kind == "ns":
            nsstack.append(name)
            bstack.append(True)
        elif kind == "open":
            bstack.append(False)
        else:  # close
            if bstack:
                was_ns = bstack.pop()
                if was_ns and nsstack:
                    nsstack.pop()
    return tuple(nsstack)


def build_class_map():
    """name -> set of (namespace-tuple, tag) where defined (non-template only)."""
    m = collections.defaultdict(set)
    for fp in src_files({".h", ".hpp"}):  # headers are canonical; .cpp local re-defs are noise
        txt = open(fp, errors="replace").read()
        for cm in CLASS_DEF.finditer(txt):
            tag, name = cm.group(2), cm.group(3)
            # skip templates: 'template' on the line above the def
            line_start = txt.rfind("\n", 0, cm.start()) + 1
            prev_start = txt.rfind("\n", 0, line_start - 1) + 1
            prev_line = txt[prev_start:line_start]
            if "template" in prev_line or "template" in txt[line_start:cm.start()]:
                continue
            ns = enclosing_namespace(txt, cm.start())
            m[name].add((ns, tag))
    return m


def ns_block(ns_tuple, decls):
    """decls: list of (tag, name)."""
    inner = "".join(f"    {tag} {n};\n" for tag, n in decls)
    if not ns_tuple:
        return "".join(f"{tag} {n};\n" for tag, n in decls)
    open_ = "".join(f"namespace {p} {{ " for p in ns_tuple)
    close_ = " " + "}" * len(ns_tuple)
    return open_ + "\n" + inner + close_ + "\n"


def process(fp, cmap):
    txt = open(fp, errors="replace").read()
    # names defined or forward-declared already in this file
    local_defined = set(re.findall(r"\b(?:class|struct)\s+([A-Za-z_]\w*)", txt))
    inc_basenames = {os.path.basename(m) for m in re.findall(r'#\s*include\s*"([^"]+)"', txt)}
    # candidate usages: NAME * or NAME & or <NAME * or <NAME>
    used = set()
    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*[*&]", txt):
        used.add(m.group(1))
    for m in re.finditer(r"<\s*([A-Za-z_]\w*)\s*[*>]", txt):
        used.add(m.group(1))
    need_by_ns = collections.defaultdict(list)  # ns_tuple -> [(tag, name)]
    add_includes = []
    # math-core headers must not include each other (would form a cycle)
    MATH_CORE = {"AEMath.h", "Vector.h", "Matrix.h", "Quaternion.h"}
    is_math_core = os.path.basename(fp) in MATH_CORE
    if not is_math_core:
        # value types: include canonical header on ANY whole-word usage (by-value too)
        for name, hdr in VALUE_TYPE_HEADER.items():
            if name in local_defined:
                continue
            if re.search(rf"\b{name}\b", txt) and os.path.basename(hdr) not in inc_basenames:
                add_includes.append(hdr)
        # AEMath:: qualifier needs the AEMath header
        if re.search(r"\bAEMath::", txt) and "AEMath.h" not in inc_basenames:
            add_includes.append("engine/math/AEMath.h")
    for name in sorted(used):
        if name in local_defined:
            continue
        if name in VALUE_TYPE_HEADER:
            continue
        if name not in cmap:
            continue
        defs = cmap[name]
        if len(defs) != 1:
            continue  # ambiguous namespace/tag; skip (manual)
        ns, tag = next(iter(defs))
        need_by_ns[ns].append((tag, name))
    add_includes = sorted(set(add_includes))
    if not need_by_ns and not add_includes:
        return None
    # insertion point: after last #include line, else after include-guard #define
    incs = list(INCLUDE_LINE.finditer(txt))
    if incs:
        ins = incs[-1].end()
    else:
        g = re.search(r"^#\s*define\s+\w+\s*$", txt, re.M)
        ins = g.end() if g else 0
    block = "\n"
    for hdr in add_includes:
        block += f'#include "{hdr}"\n'
    for ns in sorted(need_by_ns, key=lambda t: (len(t), t)):
        block += ns_block(ns, sorted(need_by_ns[ns]))
    newtxt = txt[:ins] + "\n" + block + txt[ins:]
    return newtxt, (need_by_ns, add_includes)


def main():
    cmap = build_class_map()
    changed = 0
    for fp in src_files({".h", ".hpp"}):
        r = process(fp, cmap)
        if not r:
            continue
        newtxt, (need, add_includes) = r
        changed += 1
        rel = os.path.relpath(fp, ROOT)
        flat = {f"{'::'.join(ns) or '<global>'}:{','.join(n for _, n in decls)}"
                for ns, decls in need.items()}
        inc = (" +inc:" + ",".join(os.path.basename(h) for h in add_includes)) if add_includes else ""
        print(rel, "->", " | ".join(sorted(flat)), inc)
        if not DRY:
            open(fp, "w").write(newtxt)
    print(f"\n{'DRY ' if DRY else ''}files changed: {changed}")


if __name__ == "__main__":
    main()
