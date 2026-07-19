#!/usr/bin/env python3
"""dispatch_worklist.py — one combined, race-free worklist for ALL in-scope verify gaps.

Reads the in-scope gap lists produced by tools/scope_filter.py (absent originals, wrong-type
originals, extra symbols) and routes every item to a handler `kind`, resolves its owning files,
pairs absent originals against the extra shims that stand in for them, then union-finds everything
into HEADER-DISJOINT groups so parallel fix agents never touch the same file.

Handler kinds (drive the per-group agent prompt flavor):
  wrong_type  original we implement under a different signature -> retype params to match.
  shim        an extra free-function emulation shim (`Player_ctor_cs`, `Globals_getLine`) — usually
              paired with an absent original (`Player::Player`, `Globals::getLine`); methodize /
              namespace it so it mangles to the original (removes 1 extra AND fills 1 absent).
  template    Array<T> / `void ArrayAdd|Remove|Release|SetLength<T>(...)` instantiation mismatch.
  global      vtable / typeinfo / guard-variable / static-init (`_GLOBAL__sub_I_*`) data symbol.
  absent      original with no paired extra — decompile from Ghidra (android_2.0.16) + DeepOpen.
  extra       symbol we define that the original lacks and that pairs with nothing — rename to a
              real original, retype, or inline+delete where the original inlined it.

Output: cmake-build-match/verify/worklist.json   Read-only except that file.
"""
import json
import os
import re
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import wrongtype_worklist as wt  # noqa: E402  (param_list, resolve_files, load_sym_addrs, UF, REPO…)
import scope_filter as sf        # noqa: E402  (classify, _demangle_many)

REPO = wt.REPO
VDIR = os.path.join(REPO, "cmake-build-match", "verify")
GHIDRA_BASE = wt.GHIDRA_BASE


def qualified_name(dem):
    """Fully-qualified name with the parameter list stripped (mirrors verify.qualified_name)."""
    depth, i, n = 0, 0, len(dem)
    while i < n:
        c = dem[i]
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif c == "(" and depth == 0:
            if dem[max(0, i - 8):i] == "operator":
                i += 1
                continue
            return dem[:i].strip()
        i += 1
    return dem.strip()


def leaf_key(dem):
    """Pairing key: last name component, lowercased, with our emulation suffixes stripped. Lets
    `Globals_getLine` <-> `Globals::getLine` and `Foo_bar_cs` <-> `Foo::bar` find each other."""
    base = re.sub(r"<.*>", "", dem.split("(")[0]).strip()
    base = re.sub(r"\b(ctor|dtor)\b", "", base)
    parts = re.split(r"::|_", base)
    parts = [p for p in parts if p]
    if not parts:
        return None
    tail = parts[-1].lower()
    if tail in ("cs", "cp", "cm", "cft", "ca", "ou", "ote", "up", "ext", "oat", "tail") \
            and len(parts) >= 2:
        tail = parts[-2].lower()
    return tail


DATA_PREFIXES = ("_ZTV", "_ZTI", "_ZTS", "_ZTT", "_ZGV")
DATA_DEMANGLED = re.compile(r"^(vtable|typeinfo|typeinfo name|VTT|guard variable|"
                            r"construction vtable) for ")
SHIM_FREE = re.compile(r"^[A-Z]\w*_\w")  # `Player_ctor_cs`, `Globals_getLine`, `Mod*_OnResume`


def base_kind(sym, dem, is_extra):
    name = dem.split("(")[0]
    if sym.startswith(("_GLOBAL__sub_I", "__cxx_global_var_init")) \
            or sym.startswith(DATA_PREFIXES) or DATA_DEMANGLED.match(name):
        return "global"
    if "<" in name:
        return "template"
    if is_extra:
        if "::" not in name and SHIM_FREE.match(name):
            return "shim"
        return "extra"
    return "absent"


def main():
    # --- load in-scope gap lists -------------------------------------------------------------
    absent = [l.strip() for l in open(os.path.join(VDIR, "in_scope_missing.txt")) if l.strip()]
    extra = [l.split("\t", 1)[0].strip()
             for l in open(os.path.join(VDIR, "in_scope_extra.txt")) if l.strip()]
    report = json.load(open(os.path.join(VDIR, "report.json")))
    dm_all = sf._demangle_many(set(report.get("wrong_type", []) and []) | set(absent) | set(extra))
    # drop glue + benign ctor/dtor VARIANT mismatches (C1-vs-C2 etc. with identical demangled) — same
    # alias artifacts scope_filter excludes; agents shouldn't be assigned no-op work on them.
    wrong = [w for w in report.get("wrong_type", [])
             if sf.classify(w["symbol"], w["demangled"]) != "glue"
             and not (sf._CTORDTOR.match(w["symbol"]) and w["demangled"] in w["ours"])]
    sym_addr = wt.load_sym_addrs()

    def addr_of(sym):
        a = sym_addr.get(sym)
        return hex(a + GHIDRA_BASE) if a is not None else None

    entries = []  # each: dict with symbol, demangled, qualified, kind, side, ghidra_addr, ours

    # wrong_type (rich entries already carry `ours`)
    for w in wrong:
        entries.append({"symbol": w["symbol"], "demangled": w["demangled"],
                        "qualified": w["qualified"], "kind": "wrong_type", "side": "wrong",
                        "ours": w["ours"], "ghidra_addr": addr_of(w["symbol"]),
                        "leaf": leaf_key(w["demangled"])})
    for sym in absent:
        if sf._CONTAINER_MEMBER.match(sym):
            continue  # generic Array<T> member — provided by the template, not hand-instantiated
        d = dm_all.get(sym, sym)
        entries.append({"symbol": sym, "demangled": d, "qualified": qualified_name(d),
                        "kind": base_kind(sym, d, is_extra=False), "side": "absent",
                        "ghidra_addr": addr_of(sym), "leaf": leaf_key(d)})
    for sym in extra:
        if sf._CONTAINER_MEMBER.match(sym):
            continue
        d = dm_all.get(sym, sym)
        entries.append({"symbol": sym, "demangled": d, "qualified": qualified_name(d),
                        "kind": base_kind(sym, d, is_extra=True), "side": "extra",
                        "ghidra_addr": addr_of(sym), "leaf": leaf_key(d)})

    # --- pair absent originals with extra SHIMS by leaf + class token -------------------------
    # Only free-function emulation shims (kind 'shim') are pairing candidates — never a real class
    # method (kind 'extra'), so `Array<Matrix>::resize` never gets glued to `..._ToJNI_resize`.
    # When the absent original has an owning class, the shim's leading `Class_` token must match it.
    def absent_class(e):
        return e["qualified"].rsplit("::", 1)[0].split("::")[-1] if "::" in e["qualified"] else None

    def shim_class(e):
        m = re.match(r"^([A-Z]\w*)_", e["demangled"])
        return m.group(1) if m else None

    extra_by_leaf = defaultdict(list)
    for i, e in enumerate(entries):
        if e["side"] == "extra" and e["kind"] == "shim" and e["leaf"]:
            extra_by_leaf[e["leaf"]].append(i)
    pair_edges = []  # (absent_idx, extra_idx) — unioned so both land in one group
    for i, e in enumerate(entries):
        if e["side"] != "absent" or e["kind"] in ("global", "template") or not e["leaf"]:
            continue
        ac = absent_class(e)
        j = next((c for c in extra_by_leaf.get(e["leaf"], [])
                  if ac is None or shim_class(entries[c]) == ac), None)
        if j is None:
            continue
        e["kind"] = "shim"             # this absent IS produced by an extra shim -> methodize
        e["paired_extra"] = entries[j]["symbol"]
        entries[j]["paired_absent"] = e["symbol"]
        pair_edges.append((i, j))

    # --- resolve owning files ---------------------------------------------------------------
    # An entry's file-set is TRUSTWORTHY only when small (1..MAX_TRUST). Anonymous-namespace
    # statics and a few short free-function names resolve to ~every file (the regex over-matches
    # call sites); letting those union would chain every unrelated class into one mega-group. Broad
    # and zero-file entries are instead bucketed by their owning class/scope so related work still
    # lands on one agent without the pathological bridges.
    # nearest-.text-neighbor home: a free function with no syntactic owner belongs in the source
    # unit of the COMPARED original physically adjacent to it (same original .cpp -> contiguous
    # addresses). Build a sorted (ghidra_addr -> unit) table from the report's compared functions.
    import bisect
    sym2unit = {f["symbol"]: f["unit"] for f in report.get("functions", []) if "unit" in f}
    au = sorted((sym_addr[s] + GHIDRA_BASE, u) for s, u in sym2unit.items() if s in sym_addr)
    au_addrs = [a for a, _ in au]
    HOME_DIST = 0x400  # only adopt a neighbor's unit when within 1KB (very-high-confidence same TU)

    def home_files(ghidra_addr_hex):
        if not ghidra_addr_hex or not au:
            return []
        a = int(ghidra_addr_hex, 16)
        i = bisect.bisect_left(au_addrs, a)
        best, bd = None, 1 << 60
        for j in (i - 1, i):
            if 0 <= j < len(au) and abs(au_addrs[j] - a) < bd:
                bd, best = abs(au_addrs[j] - a), au[j][1]
        if best is None or bd >= HOME_DIST:
            return []
        cpp, h = f"src/{best}.cpp", f"src/{best}.h"
        return [p for p in (h, cpp) if os.path.exists(os.path.join(REPO, p))]

    MAX_TRUST = 4
    for e in entries:
        leaf_name = re.sub(r"<.*>", "", e["qualified"]).rsplit("::", 1)[-1]
        # very short names (`F`, `at`, `x`) match everywhere — never trust their resolved files.
        if len(leaf_name) <= 2:
            e["headers"], e["cpps"], e["files"], e["trusted"] = [], [], [], False
            continue
        headers, cpps = wt.resolve_files(e["qualified"])
        e["headers"], e["cpps"] = headers, cpps
        e["files"] = sorted(set(headers) | set(cpps))
        # a free function DECLARED in a header but not yet defined (cpps empty) -> include the
        # header's sibling .cpp, which is where its definition must go (the agent can't add a body to
        # a header-only component). Fixes the recovered_*.cpp input-function case.
        if "::" not in e["qualified"] and e["headers"] and not e["cpps"]:
            sibs = [h[:-2] + ".cpp" for h in e["headers"]
                    if os.path.exists(os.path.join(REPO, h[:-2] + ".cpp"))]
            if sibs:
                e["cpps"] = sibs
                e["files"] = sorted(set(e["files"]) | set(sibs))
        # platform/NDK free functions belong in the platform TU — do NOT nearest-neighbor them into an
        # unrelated component (agents correctly refuse to put loadAPK in FModSound). Leave them
        # file-less so orphan_root routes them to the platform/JNI bucket.
        is_platform = e["symbol"].startswith(("Java_", "JNI_", "ndk23_", "ndk_")) or e["symbol"] in {
            "loadAPK", "loadAPKAndZip", "opensubkeyfile", "decrypt", "setBaughtCredits",
            "checkFirstCreditPackBoughtWriteAction", "getStringUTFChars", "releaseStringUTFChars",
            "pConstToNonConst"}
        # free function nobody declares yet -> adopt its nearest-neighbor unit's file as home.
        if not e["files"] and "::" not in e["qualified"] and not is_platform:
            e["files"] = home_files(e.get("ghidra_addr"))
        e["trusted"] = 1 <= len(e["files"]) <= MAX_TRUST

    # --- union-find: shared file (trusted only) OR an absent<->extra pair -> same group ------
    uf = wt.UF()
    for idx, e in enumerate(entries):
        uf.union(("E", idx), ("E", idx))
        if e["trusted"]:
            for f in e["files"]:
                uf.union(("E", idx), ("F", f))
    for i, j in pair_edges:
        if entries[i]["trusted"] and entries[j]["trusted"]:
            uf.union(("E", i), ("E", j))

    def class_key(e):
        """Owning class/scope for bucketing untrusted/orphan entries. None for anon-ns or pure
        free functions (no stable owner -> handled per-symbol)."""
        scope = qualified_name(e["demangled"]).rsplit("::", 1)[0] if "::" in e["qualified"] else ""
        scope = re.sub(r"\b(AbyssEngine|AEMath|FMOD)::", "", scope).strip()
        if not scope or "anonymous namespace" in scope or "<" in scope:
            return None
        return scope

    # map a class_key -> the file-group root that owns that class (so orphan Player::* methods join
    # the existing Player.cpp group instead of forming a parallel island).
    keyroot = {}
    for idx, e in enumerate(entries):
        if e["trusted"]:
            ck = class_key(e)
            if ck:
                keyroot.setdefault(ck, uf.find(("E", idx)))

    # No-file GLOBAL FREE functions (no `::`) have no owning file and no caller hint, but functions
    # from the same original .cpp are contiguous in .text — so cluster them by address RUN (a gap
    # > RUN_GAP starts a new TU). One agent then reconstructs one original source file, instead of
    # N agents each independently inventing a home for related globals and clobbering it.
    RUN_GAP = 0x1000
    free_orphans = sorted(
        (idx for idx, e in enumerate(entries)
         if not e["trusted"] and "::" not in e["qualified"] and e.get("ghidra_addr")
         and e["kind"] not in ("template",)
         and not e["symbol"].startswith(("Java_", "JNI_", "ndk23_", "ndk_"))),
        key=lambda i: int(entries[i]["ghidra_addr"], 16))
    run_of = {}
    run_id, prev = -1, None
    for idx in free_orphans:
        a = int(entries[idx]["ghidra_addr"], 16)
        if prev is None or a - prev > RUN_GAP:
            run_id += 1
        run_of[idx] = run_id
        prev = a

    def orphan_root(idx):
        e = entries[idx]
        if e["kind"] == "template":
            return ("BUCKET", "__templates__")
        if e["symbol"].startswith(("Java_", "JNI_", "ndk23_", "ndk_")):
            return ("BUCKET", "__jni_bridge__")
        if idx in run_of:
            return ("FREE_RUN", run_of[idx])
        ck = class_key(e)
        if ck:
            return keyroot.get(ck) or ("BUCKET", ck)
        return ("ORPHAN", e["symbol"])

    groups = defaultdict(list)
    for idx, e in enumerate(entries):
        root = uf.find(("E", idx)) if e["trusted"] else orphan_root(idx)
        groups[root].append(idx)

    # A COMPONENT is one file-disjoint island (its files are touched by no other component). A wave
    # runs at most ONE subbatch per component in parallel, so concurrent agents are always
    # file-disjoint (across components by construction; within a component serialized). Each
    # component is split into class-coherent subbatches capped at SUBBATCH so no single agent is
    # handed an unbounded pile.
    SUBBATCH = 18

    def scope_of(e):
        ck = class_key(e)
        if ck:
            return ck
        return e["qualified"].rsplit("::", 1)[0] if "::" in e["qualified"] else "(free)"

    # Synthetic no-file components write a single, deterministic target file. Templates land in the
    # Array header; the JNI/ndk bridge in one platform TU; an address-run reconstructs one original
    # .cpp under a stable address-derived name (so its sequential subbatches all append to the SAME
    # file across waves instead of each inventing a new one). FREE_RUNs are NOT split — one agent
    # owns the whole reconstructed TU, so the filename is decided once.
    # returns (target_file, mode, size): mode 'scope' splits by class scope then chunks at `size`;
    # 'flat' ignores scope and chunks all entries at `size` (synthetic one-file buckets); 'whole'
    # keeps everything in one subbatch.
    def target_and_split(root, idxs):
        if root == ("BUCKET", "__templates__"):
            return "src/engine/core/Array.h", "flat", 45   # trivial one-line instantiations
        if root == ("BUCKET", "__jni_bridge__"):
            return "src/platform/jni_bridge.cpp", "flat", 18
        if isinstance(root, tuple) and root[0] == "FREE_RUN":
            lo = min(int(entries[i]["ghidra_addr"], 16) for i in idxs)
            return f"src/platform/recovered_{lo:x}.cpp", "whole", 0  # one agent owns the new TU
        return None, "scope", SUBBATCH

    components = []
    for root, idxs in groups.items():
        target_file, mode, size = target_and_split(root, idxs)
        subbatches = []
        if mode == "whole":
            subbatches = [idxs]
        elif mode == "flat":
            for k in range(0, len(idxs), size):
                subbatches.append(idxs[k:k + size])
        else:  # scope: keep each class's work together, chunk to <= size
            by_scope = defaultdict(list)
            for i in idxs:
                by_scope[scope_of(entries[i])].append(i)
            for scope, sidx in sorted(by_scope.items(), key=lambda kv: (-len(kv[1]), kv[0])):
                for k in range(0, len(sidx), size):
                    subbatches.append(sidx[k:k + size])
        gfiles = sorted({f for i in idxs for f in entries[i]["files"]})
        kinds = [entries[i]["kind"] for i in idxs]
        kc = {k: kinds.count(k) for k in set(kinds)}
        dominant = max(kc, key=lambda k: (kc[k], -["wrong_type", "global", "template", "shim",
                                                    "extra", "absent"].index(k)))
        components.append({"dominant_kind": dominant, "kinds": kc, "n_entries": len(idxs),
                           "n_subbatches": len(subbatches), "files": gfiles,
                           "target_file": target_file, "subbatches": subbatches})
    # smallest-first so quick wins (singletons / new-file orphans) clear early
    components.sort(key=lambda c: (c["n_subbatches"], c["n_entries"]))

    by_kind = {k: sum(1 for e in entries if e["kind"] == k)
               for k in ("wrong_type", "shim", "template", "global", "absent", "extra")}
    out = {"total": len(entries), "by_kind": by_kind, "n_components": len(components),
           "n_subbatches": sum(c["n_subbatches"] for c in components), "n_pairs": len(pair_edges),
           "unresolved_files": sum(1 for e in entries if not e["files"]),
           "max_subbatches_in_a_component": max(c["n_subbatches"] for c in components),
           "entries": entries, "components": components}
    json.dump(out, open(os.path.join(VDIR, "worklist.json"), "w"), indent=1)
    from collections import Counter
    print(f"entries {out['total']}   components {out['n_components']}   "
          f"subbatches {out['n_subbatches']}   pairs {out['n_pairs']}   "
          f"orphans(no-file) {out['unresolved_files']}")
    print("by_kind:", json.dumps(by_kind))
    print("subbatches-per-component:", dict(sorted(Counter(
        c["n_subbatches"] for c in components).items())))
    print(f"critical path (deepest component): {out['max_subbatches_in_a_component']} "
          f"sequential subbatches")
    print(f"first-wave width (components with work): {sum(1 for c in components if c['n_subbatches'])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
