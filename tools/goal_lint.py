#!/usr/bin/env python3
"""goal_lint.py - quality scoreboard for the symbol-identical decompilation goal.

Scans src/ and reports, per acceptance criterion, a count and a few offenders.
All categories must reach 0 for the goal to be met. Exit code is non-zero while
any category is non-zero, so it can gate CI / wave loops.

Usage:
    python3 tools/goal_lint.py            # summary table
    python3 tools/goal_lint.py --list CAT # list every offender for a category
    python3 tools/goal_lint.py --json     # machine-readable counts
"""
import os, re, sys, json, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src")

# --- token bans (criterion 7) -------------------------------------------------
# word-boundary tokens; comments/strings are not stripped (decomp has ~none in
# these positions and an over-count is safe — the target is literal zero).
TOKEN_PATTERNS = {
    "asm":           re.compile(r"\basm\s*\("),
    "extern":        re.compile(r"\bextern\b"),
    "inline":        re.compile(r"\binline\b"),
    "__attribute__": re.compile(r"__attribute__"),
}

# --- fieldaccess helpers (criterion 5) ---------------------------------------
FIELDACCESS = re.compile(r"\b(?:f32|i32|u32|u16|u8|s16|pp)\s*\(|\bF\s*<")

# --- pointer arithmetic (criterion 4) ----------------------------------------
# raw offset reinterpret access:  (char *) <expr> + 0xNN   or  + <decimal>
PTR_ARITH = re.compile(r"\(\s*char\s*\*\s*\)[^;\n]*?\+\s*(?:0x[0-9a-fA-F]+|\d+)")

# --- class/struct definitions (criterion 1) ----------------------------------
# top-of-line (optionally indented) definition with an opening brace, not a
# forward declaration (which ends in ';') and not a variable like `struct X x;`.
CLASS_DEF = re.compile(r"^[ \t]*(?:class|struct)\s+([A-Za-z_]\w*)\b(?:\s*:[^{;]*)?\s*\{", re.M)
FWD_DECL  = re.compile(r"^[ \t]*(?:class|struct)\s+([A-Za-z_]\w*)\s*;", re.M)

INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.M)


def src_files(exts):
    for dp, _, fns in os.walk(SRC):
        for fn in fns:
            if os.path.splitext(fn)[1] in exts:
                yield os.path.join(dp, fn)


def rel(p):
    return os.path.relpath(p, ROOT)


def scan_tokens():
    """Return {category: [(file, lineno, text)]}."""
    hits = collections.defaultdict(list)
    for fp in src_files({".h", ".cpp", ".hpp", ".c", ".cc"}):
        try:
            lines = open(fp, errors="replace").read().splitlines()
        except OSError:
            continue
        for i, line in enumerate(lines, 1):
            for cat, pat in TOKEN_PATTERNS.items():
                if pat.search(line):
                    hits[cat].append((rel(fp), i, line.strip()))
            if FIELDACCESS.search(line):
                hits["fieldaccess"].append((rel(fp), i, line.strip()))
            if PTR_ARITH.search(line):
                hits["ptr_arith"].append((rel(fp), i, line.strip()))
    return hits


def scan_multiclass():
    """Headers (.h) that define more than one class/struct."""
    offenders = []
    for fp in src_files({".h", ".hpp"}):
        txt = open(fp, errors="replace").read()
        names = CLASS_DEF.findall(txt)
        if len(names) > 1:
            offenders.append((rel(fp), names))
    return offenders


def resolve_include(inc, from_file):
    """Resolve a quoted include to an absolute path (dir-relative, then -Isrc)."""
    cand = os.path.normpath(os.path.join(os.path.dirname(from_file), inc))
    if os.path.isfile(cand):
        return cand
    cand = os.path.normpath(os.path.join(SRC, inc))
    if os.path.isfile(cand):
        return cand
    return None


def build_include_graph():
    g = collections.defaultdict(set)
    files = list(src_files({".h", ".hpp", ".cpp", ".cc", ".c"}))
    for fp in files:
        txt = open(fp, errors="replace").read()
        for inc in INCLUDE.findall(txt):
            tgt = resolve_include(inc, fp)
            if tgt:
                g[os.path.abspath(fp)].add(os.path.abspath(tgt))
    return g


def find_cycles(g):
    """Tarjan SCC; return SCCs of size>1 plus self-loops, as rel-path lists."""
    index = {}
    low = {}
    onstack = {}
    stack = []
    counter = [0]
    sccs = []

    import sys as _sys
    _sys.setrecursionlimit(10000)

    def strongconnect(v):
        index[v] = low[v] = counter[0]
        counter[0] += 1
        stack.append(v)
        onstack[v] = True
        for w in g.get(v, ()):  # only nodes that have outgoing edges keyed
            if w not in index:
                strongconnect(w)
                low[v] = min(low[v], low[w])
            elif onstack.get(w):
                low[v] = min(low[v], index[w])
        if low[v] == index[v]:
            comp = []
            while True:
                w = stack.pop()
                onstack[w] = False
                comp.append(w)
                if w == v:
                    break
            sccs.append(comp)

    nodes = set(g.keys())
    for vs in g.values():
        nodes |= vs
    for v in nodes:
        if v not in index:
            strongconnect(v)

    cycles = []
    for comp in sccs:
        if len(comp) > 1:
            cycles.append(sorted(rel(p) for p in comp))
        elif len(comp) == 1 and comp[0] in g.get(comp[0], ()):
            cycles.append([rel(comp[0])])
    return cycles


def main():
    args = sys.argv[1:]
    hits = scan_tokens()
    multiclass = scan_multiclass()
    graph = build_include_graph()
    cycles = find_cycles(graph)

    counts = {cat: len(v) for cat, v in hits.items()}
    counts.setdefault("asm", 0); counts.setdefault("extern", 0)
    counts.setdefault("inline", 0); counts.setdefault("__attribute__", 0)
    counts.setdefault("fieldaccess", 0); counts.setdefault("ptr_arith", 0)
    counts["multiclass_headers"] = len(multiclass)
    counts["include_cycles"] = len(cycles)

    if "--json" in args:
        print(json.dumps(counts, indent=2, sort_keys=True))
        return 0 if all(v == 0 for v in counts.values()) else 1

    if "--list" in args:
        cat = args[args.index("--list") + 1]
        if cat == "multiclass_headers":
            for fp, names in multiclass:
                print(f"{fp}: {', '.join(names)}")
        elif cat == "include_cycles":
            for c in cycles:
                print(" <-> ".join(c))
        else:
            for fp, ln, txt in hits.get(cat, []):
                print(f"{fp}:{ln}: {txt}")
        return 0

    order = ["asm", "extern", "inline", "__attribute__", "fieldaccess",
             "ptr_arith", "multiclass_headers", "include_cycles"]
    total = 0
    print(f"{'category':<22} {'count':>8}")
    print("-" * 32)
    for cat in order:
        c = counts[cat]
        total += c
        flag = "" if c == 0 else "  <-- TODO"
        print(f"{cat:<22} {c:>8}{flag}")
    print("-" * 32)
    print(f"{'TOTAL':<22} {total:>8}")
    if cycles:
        print("\nfirst include cycles:")
        for c in cycles[:8]:
            print("  " + " <-> ".join(c))
    return 0 if total == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
