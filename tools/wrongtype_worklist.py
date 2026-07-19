#!/usr/bin/env python3
"""Build the worklist for the wrong-signature cleanup.

Reads cmake-build-match/verify/report.json `wrong_type` (functions we implemented with the wrong
C++ signature vs the original binary), and for each entry:
  * disambiguates which of our overloads (`ours`) maps to the original target (by param list),
  * categorizes the per-parameter diff (NAMESPACE_QUALIFY / NESTED_ENUM / PTR_TYPE / CONST_QUAL /
    VALUE_VS_REF / INT_WIDTH / CTOR_DTOR / ARITY / TEMPLATE / OTHER), and routes AUTO vs FLAG,
  * resolves the owning header + cpp via ripgrep (classmap.json is stale; ripgrep is ground truth),
  * looks up the Ghidra address (vaddr from the symbols TSV + 0x10000) for optional body semantics.

Entries are then union-found into HEADER-DISJOINT GROUPS: two entries land in the same group iff their
fixes write a common file. Each group is the unit of work for one isolated agent, so no two parallel
agents ever edit the same header. Groups are ordered leaf-first by fan-in (how many files reference the
owning type).

Output: cmake-build-match/verify/worklist.json   (see build_worklist()).
Read-only except for the output file.
"""
import json
import os
import re
import subprocess
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
SRC = os.path.join(REPO, "src")
REPORT = os.path.join(REPO, "cmake-build-match", "verify", "report.json")
SYMS = "/Users/fionera/Downloads/GalaxyOnFire2/_work/symbols/android_2.0.16.symbols.tsv"
GHIDRA_BASE = 0x10000

NAMESPACES = {"AbyssEngine", "AEMath", "FMOD"}  # AERandom is a class, not a namespace

# Permanently deferred: the binary's signature genuinely can't be matched without a cast or a
# deep cross-type cascade, so these are routed to FLAG (not re-attempted by fix waves).
#   - AEFile/FileInterface SetAppRootDir/SetZipDirectory: original takes void*, but the Android
#     impl stores into a `const char* appRootDir` read back by GetAppRootDir()->const char*, so a
#     void* parameter forces an internal cast. Kept const char* (consistent, cast-free).
#   - FModSound::updateEvent3DAttributes: void*->FMOD::Event* needs the `void* events[]` member
#     array retyped, which cascades through void*-returning FMOD shims.
DEFER_QUALIFIED = set()  # nothing permanently deferred — the hard cases are being fixed directly
INT_TYPES = {"int", "unsigned int", "short", "unsigned short", "char", "signed char",
             "unsigned char", "long", "unsigned long", "long long", "unsigned long long",
             "bool", "uint32_t", "int32_t", "uint16_t", "int16_t", "uint8_t", "int8_t",
             "size_t", "unsigned", "wchar_t"}
OPAQUE_PTRS = {"void*", "void *", "int", "unsigned int", "char const*", "char const *",
               "char*", "char *", "void**", "void **"}


# ---------------------------------------------------------------------------
# signature parsing
# ---------------------------------------------------------------------------
def _split_top(s, opens="(<", closes=")>", sep=","):
    """Split s on top-level `sep`, respecting () and <> nesting."""
    out, depth, cur = [], 0, ""
    for c in s:
        if c in opens:
            depth += 1
        elif c in closes:
            depth -= 1
        if c == sep and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += c
    if cur.strip():
        out.append(cur)
    return [x.strip() for x in out]


def param_list(demangled):
    """Return (params[list[str]], trailing) for a demangled signature. trailing is e.g. 'const'.
    Mirrors verify.qualified_name's paren-finding (angle-depth aware, steps over operator())."""
    depth, i, n = 0, 0, len(demangled)
    start = None
    while i < n:
        c = demangled[i]
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif c == "(" and depth == 0:
            if demangled[max(0, i - 8):i] == "operator":
                i += 1
                continue
            start = i
            break
        i += 1
    if start is None:
        return None, ""  # not a function
    # find matching close paren for the param list
    pdepth, j = 0, start
    while j < n:
        if demangled[j] == "(":
            pdepth += 1
        elif demangled[j] == ")":
            pdepth -= 1
            if pdepth == 0:
                break
        j += 1
    inner = demangled[start + 1:j]
    trailing = demangled[j + 1:].strip()
    params = _split_top(inner)
    if params == ["void"]:
        params = []
    return params, trailing


def strip_scopes(t):
    """Remove namespace/class qualifiers from each identifier in a type string, so
    'AbyssEngine::Curve const*' and 'Curve const*' compare equal."""
    return re.sub(r"\b[A-Za-z_]\w*::", "", t)


def norm_ws(t):
    return re.sub(r"\s+", " ", t).strip()


# ---------------------------------------------------------------------------
# per-parameter diff classification
# ---------------------------------------------------------------------------
def is_opaque(t):
    n = norm_ws(t)
    return n in OPAQUE_PTRS or n in INT_TYPES


def base_type(t):
    """Strip one trailing ref and const/* decoration to get the core identifier(s)."""
    return norm_ws(t.replace("&", "").replace("*", "").replace("const", ""))


def classify_param(orig, our):
    o, u = norm_ws(orig), norm_ws(our)
    if o == u:
        return "SAME"
    # ref vs value: equal after dropping a single trailing '&'
    if o.rstrip("&").strip() == u.rstrip("&").strip() and (o.endswith("&") != u.endswith("&")):
        return "VALUE_VS_REF"
    # const-only difference
    if norm_ws(o.replace("const", "")) == norm_ws(u.replace("const", "")):
        return "CONST_QUAL"
    # scope/spelling difference only (namespace-qualify or nested-enum)
    if strip_scopes(o) == strip_scopes(u):
        # which side carries the extra leading scope, and is it a namespace?
        scopes = re.findall(r"\b([A-Za-z_]\w*)::", o + " " + u)
        if scopes and all(s in NAMESPACES for s in scopes):
            return "NAMESPACE_QUALIFY"
        return "NESTED_ENUM"
    # pointer typed <-> opaque
    o_ptr, u_ptr = "*" in o, "*" in u
    if (o_ptr or u_ptr) and (is_opaque(o) != is_opaque(u)):
        return "PTR_TYPE"
    if o_ptr and u_ptr and base_type(o) != base_type(u):
        return "PTR_TYPE"
    # integer width / signedness
    if base_type(o) in INT_TYPES and base_type(u) in INT_TYPES:
        return "INT_WIDTH"
    return "OTHER"


CAT_PRIORITY = ["OTHER", "PTR_TYPE", "NESTED_ENUM", "INT_WIDTH", "CONST_QUAL",
                "VALUE_VS_REF", "NAMESPACE_QUALIFY"]


def aggregate(cats):
    """Pick the dominant category of an entry. OTHER dominates (most-careful); otherwise the
    lowest-priority (most-substantive) non-trivial category present."""
    cats = [c for c in cats if c != "SAME"]
    if not cats:
        return "SAME"
    if "OTHER" in cats:
        return "OTHER"
    return min(cats, key=lambda c: CAT_PRIORITY.index(c) if c in CAT_PRIORITY else 0)


def is_ctor_dtor(qualified):
    parts = qualified.split("::")
    if len(parts) < 2:
        return False
    cls, meth = parts[-2], parts[-1]
    return meth == cls or meth == "~" + cls


def is_template(demangled, ours):
    return "<" in demangled.split("(")[0] or any("<" in o.split("(")[0] for o in ours)


# ---------------------------------------------------------------------------
# overload disambiguation
# ---------------------------------------------------------------------------
def disambiguate(target_params, ours):
    """Choose the single `ours` overload that matches the target by param list. Returns
    (chosen_sig | None, chosen_params | None, reason). reason in {ok, missing_overload, ambiguous}."""
    parsed = [(o, param_list(o)[0]) for o in ours]
    same_arity = [(o, p) for o, p in parsed if p is not None and len(p) == len(target_params)]
    if not same_arity:
        return None, None, "missing_overload"
    if len(same_arity) == 1:
        return same_arity[0][0], same_arity[0][1], "ok"
    # score by number of differing positions; fewer = closer
    def score(p):
        return sum(1 for a, b in zip(target_params, p) if norm_ws(a) != norm_ws(b))
    scored = sorted(same_arity, key=lambda op: score(op[1]))
    best = score(scored[0][1])
    winners = [op for op in scored if score(op[1]) == best]
    if len(winners) == 1:
        return winners[0][0], winners[0][1], "ok"
    return None, None, "ambiguous"


# ---------------------------------------------------------------------------
# file resolution (ripgrep-primary)
# ---------------------------------------------------------------------------
_rg_cache = {}


def rg_files(pattern, exts):
    key = (pattern, tuple(exts))
    if key in _rg_cache:
        return _rg_cache[key]
    cmd = ["rg", "-l", "--fixed-strings"]
    for e in exts:
        cmd += ["-g", f"*.{e}"]
    cmd += [pattern, SRC]
    try:
        out = subprocess.run(cmd, capture_output=True, text=True, timeout=30).stdout
        files = sorted(os.path.relpath(p, REPO) for p in out.split("\n") if p.strip())
    except Exception:
        files = []
    _rg_cache[key] = files
    return files


def rg_files_re(pattern, exts, pcre=False):
    key = ("RE:" + ("P:" if pcre else "") + pattern, tuple(exts))
    if key in _rg_cache:
        return _rg_cache[key]
    cmd = ["rg", "-l"] + (["-P"] if pcre else [])
    for e in exts:
        cmd += ["-g", f"*.{e}"]
    cmd += [pattern, SRC]
    try:
        out = subprocess.run(cmd, capture_output=True, text=True, timeout=30).stdout
        files = sorted(os.path.relpath(p, REPO) for p in out.split("\n") if p.strip())
    except Exception:
        files = []
    _rg_cache[key] = files
    return files


def resolve_files(qualified):
    """Return (header_files, cpp_files) for the OWNING declaration + definition of a qualified name
    — NOT its call sites. Callers are reconciled post-merge per wave; including them here would glue
    unrelated classes into one mega-group (every shared caller becomes a union edge). So:
      header  = the .h that declares the class (members) / the function (free funcs),
      cpp     = the sibling Class.cpp + any file whose line *starts* with the definition
                (`^<rettype> Scope::meth(`), which excludes mid-expression call sites."""
    parts = qualified.split("::")
    meth = parts[-1]
    scope = parts[-2] if len(parts) >= 2 else None
    headers, cpps = [], []
    if scope and scope not in NAMESPACES:
        # member function of class `scope`
        # match the class DEFINITION (`class Foo {` / `class Foo : ...` / brace on next line),
        # not forward declarations (`class Foo;`) which litter dozens of headers and would chain
        # every group together.
        headers = rg_files_re(rf"\b(class|struct)\s+{re.escape(scope)}\b\s*([:{{]|$)", ["h"], pcre=True)
        sib = [h[:-2] + ".cpp" for h in headers
               if os.path.exists(os.path.join(REPO, h[:-2] + ".cpp"))]
        defpat = rf"^[A-Za-z_].*\b{re.escape(scope)}::{re.escape(meth)}\s*\("
        defs = rg_files_re(defpat, ["cpp"])
        cpps = sorted(set(sib) | set(defs))
    else:
        # free function in a namespace (or global): definition is unqualified — exclude `X::meth`
        defpat = rf"^[A-Za-z_].*(?<![:\w]){re.escape(meth)}\s*\("
        cpps = rg_files_re(defpat, ["cpp"], pcre=True)
        headers = rg_files_re(rf"(?<![:\w]){re.escape(meth)}\s*\(", ["h"], pcre=True)
    return headers, cpps


# ---------------------------------------------------------------------------
# symbols / ghidra address
# ---------------------------------------------------------------------------
def load_sym_addrs():
    m = {}
    with open(SYMS) as f:
        for line in f:
            parts = line.rstrip("\n").split("\t")
            if len(parts) >= 2:
                try:
                    m[parts[1]] = int(parts[0], 16)
                except ValueError:
                    pass
    return m


# ---------------------------------------------------------------------------
# union-find grouping
# ---------------------------------------------------------------------------
class UF:
    def __init__(self):
        self.p = {}

    def find(self, x):
        self.p.setdefault(x, x)
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x

    def union(self, a, b):
        self.p[self.find(a)] = self.find(b)


# ---------------------------------------------------------------------------
def build_worklist():
    report = json.load(open(REPORT))
    wrong = report["wrong_type"]
    sym_addr = load_sym_addrs()

    entries = []
    for e in wrong:
        target_params, target_trailing = param_list(e["demangled"])
        chosen, chosen_params, reason = disambiguate(target_params or [], e["ours"])
        qualified = e["qualified"]
        ctor_dtor = is_ctor_dtor(qualified)
        templ = is_template(e["demangled"], e["ours"])

        if qualified in DEFER_QUALIFIED:
            category, route, flag_reason = "DEFERRED_HARD", "FLAG", "deferred_hard"
        elif reason != "ok":
            category, route, flag_reason = ("AMBIGUOUS" if reason == "ambiguous"
                                            else "MISSING_OVERLOAD"), "FLAG", reason
        elif norm_ws(e["demangled"]) == norm_ws(chosen):
            # demangled strings identical -> the symbols differ only in mangling: a ctor/dtor
            # variant gap (C1/C2 vs D0/D1/D2) or a substitution artifact. Not a param retype.
            category = "CTOR_DTOR_VARIANT" if ctor_dtor else "SUBST"
            route, flag_reason = "FLAG", "variant_or_subst"
        elif len(chosen_params) != len(target_params):
            category, route, flag_reason = "ARITY", "FLAG", "arity"
        else:
            cats = [classify_param(a, b) for a, b in zip(target_params, chosen_params)]
            category = aggregate(cats)
            if category == "SAME":
                # identical params: difference is the trailing qualifier (e.g. method const)
                category = "CONST_QUAL"
            # AUTO (incl. OTHER — mixed-type diffs are still retypeable; "chase the cascades").
            # FLAG only what isn't a signature retype: templates, and ctor/dtor with a structural
            # (arity) change. Variant gaps / missing overloads / ambiguous were caught above.
            route, flag_reason = "AUTO", None
            if templ:
                category, route, flag_reason = "TEMPLATE", "FLAG", "template"
            elif ctor_dtor and category in ("ARITY",):
                route, flag_reason = "FLAG", "ctor_dtor_structural"

        headers, cpps = resolve_files(qualified)
        files = sorted(set(headers) | set(cpps))
        addr = sym_addr.get(e["symbol"])

        entries.append({
            "symbol": e["symbol"],
            "qualified": qualified,
            "target": e["demangled"],
            "target_params": target_params,
            "ours": e["ours"],
            "chosen_ours": chosen,
            "category": category,
            "route": route,
            "flag_reason": flag_reason,
            "ctor_dtor": ctor_dtor,
            "headers": headers,
            "cpps": cpps,
            "files": files,
            "ghidra_addr": (hex(addr + GHIDRA_BASE) if addr is not None else None),
        })

    # union-find by shared file
    uf = UF()
    for idx, e in enumerate(entries):
        uf.union(("E", idx), ("E", idx))
        for f in e["files"]:
            uf.union(("E", idx), ("F", f))
    groups = defaultdict(list)
    for idx, e in enumerate(entries):
        if e["files"]:
            groups[uf.find(("E", idx))].append(idx)
        else:
            groups[("ORPHAN", idx)].append(idx)  # no files resolved -> its own group

    # fan-in per group (sum of files that reference the owning leaf type) for ordering
    grouped = []
    for key, idxs in groups.items():
        gfiles = sorted({f for i in idxs for f in entries[i]["files"]})
        cats = [entries[i]["category"] for i in idxs]
        routes = [entries[i]["route"] for i in idxs]
        grouped.append({
            "files": gfiles,
            "n_entries": len(idxs),
            "entry_idx": idxs,
            "categories": sorted(set(cats)),
            "has_auto": "AUTO" in routes,
            "has_flag": "FLAG" in routes,
            "all_namespace_qualify": all(entries[i]["category"] == "NAMESPACE_QUALIFY"
                                         and entries[i]["route"] == "AUTO" for i in idxs),
        })
    # order: pure namespace-qualify groups first, then by file count (leaf-first), then size
    grouped.sort(key=lambda g: (not g["all_namespace_qualify"], len(g["files"]), g["n_entries"]))

    out = {
        "total": len(entries),
        "by_category": dict(sorted(
            {c: sum(1 for e in entries if e["category"] == c) for c in
             {e["category"] for e in entries}}.items())),
        "by_route": {r: sum(1 for e in entries if e["route"] == r) for r in ("AUTO", "FLAG")},
        "n_groups": len(grouped),
        "unresolved_files": sum(1 for e in entries if not e["files"]),
        "entries": entries,
        "groups": grouped,
    }
    return out


if __name__ == "__main__":
    wl = build_worklist()
    out_path = os.path.join(REPO, "cmake-build-match", "verify", "worklist.json")
    json.dump(wl, open(out_path, "w"), indent=1)
    print(f"entries {wl['total']}   groups {wl['n_groups']}   "
          f"unresolved-files {wl['unresolved_files']}")
    print("by_route:", wl["by_route"])
    print("by_category:")
    for c, n in sorted(wl["by_category"].items(), key=lambda kv: -kv[1]):
        print(f"  {c:18} {n}")
    print(f"-> {os.path.relpath(out_path, REPO)}")
