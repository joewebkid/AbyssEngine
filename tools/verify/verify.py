#!/usr/bin/env python3
"""Function-level ASM validation: compare our ARM build output against the original
libgof2hdaa.so, function by function, and print a match-% report.

Pipeline (ARM tooling runs in OrbStack or the local NDK fallback; objdiff can't
read ARMv7 so we use GNU objdump directly):
  1. compile each src/*.cpp with the matching NDK r18b toolchain  (build_objs.sh)
  2. for each base .o, delink the matching originals out of the .so   (delink.py)
  3. disassemble both, normalize, fuzzy-match per symbol             (asmdiff.py)
  4. print a table sorted worst-first + write report.json

Usage:
  verify.py [--build-dir DIR] [--only REGEX] [--no-build]
  verify.py --show <mangled-symbol>        # side-by-side diff of one function
"""
import argparse
import concurrent.futures
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import asmdiff       # noqa: E402
import delink as delinker  # noqa: E402  (local delink() below shadows the module name)

DEFAULT_BUILD = os.path.join(REPO, "cmake-build-match", "verify")


def run(cmd, **kw):
    return subprocess.run(cmd, check=True, **kw)


def delink(base_o, target_o):
    # stdout/stderr suppressed so delink diagnostics don't interleave into the report table; a
    # delink failure surfaces as a concise per-unit skip line after the table.
    run([sys.executable, os.path.join(HERE, "delink.py"),
         "--base", base_o, "--out", target_o],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=asmdiff.DISASM_TIMEOUT)


def find_base_objects(build_dir):
    base_root = os.path.join(build_dir, "base")
    objs = []
    for dirpath, _, files in os.walk(base_root):
        for f in files:
            if f.endswith(".o"):
                p = os.path.join(dirpath, f)
                objs.append((os.path.relpath(p, base_root)[:-2], p))  # (unit, path)
    return sorted(objs)


def _diff_unit(unit, base_o, target_root, only_re):
    """Delink + diff one base object. Returns (rows, skip, our_syms) where skip is None or a
    short reason string and our_syms is the set of mangled symbols this object defines (used,
    independently of whether they matched, to spot originals we implement under a different
    signature). Per-unit work is independent (each writes only its own target_o), so it is safe
    to run concurrently. Failures are returned, not printed, so they can be summarised after the
    table instead of interleaving with it."""
    target_o = os.path.join(target_root, unit + ".o")
    os.makedirs(os.path.dirname(target_o), exist_ok=True)
    # Tool calls can wedge transiently; a timed-out unit is retried once (the retry almost always
    # clears) before it is skipped, so a transient wedge doesn't silently drop the unit's functions
    # from the report and make the totals vary run-to-run.
    for attempt in (1, 2):
        try:
            delink(base_o, target_o)
            tf = asmdiff.disassemble(target_o)
            bf = asmdiff.disassemble(base_o)
            break
        except subprocess.CalledProcessError:
            return [], "no delinkable target", set()
        except subprocess.TimeoutExpired:
            if attempt == 1:
                continue
            return [], "objdump hung twice", set()
    our_syms = {s for s in bf if s.startswith("_Z")}
    out = []
    for r in asmdiff.compare(tf, bf):
        if only_re and not only_re.search(r["symbol"]):
            continue
        r["unit"] = unit
        out.append(r)
    return out, None, our_syms


def collect(build_dir, only=None):
    """Returns (rows, skips, our_syms). skips is a list of (unit, reason) for units that
    couldn't be compared; our_syms is the union of mangled symbols our build defines across all
    units. The per-unit delink+disasm work is all subprocesses (GIL released), so we fan
    it out across a thread pool — same GOF2_VERIFY_JOBS knob build_objs.sh uses."""
    target_root = os.path.join(build_dir, "target")
    only_re = re.compile(only) if only else None
    units = find_base_objects(build_dir)
    jobs = max(1, int(os.environ.get("GOF2_VERIFY_JOBS", "8")))
    rows, skips, our_syms = [], [], set()
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
        futures = {ex.submit(_diff_unit, unit, base_o, target_root, only_re): unit
                   for unit, base_o in units}
        for fut, unit in futures.items():
            unit_rows, skip, unit_syms = fut.result()
            rows.extend(unit_rows)
            our_syms |= unit_syms
            if skip:
                skips.append((unit, skip))
    rows.sort(key=lambda r: (r["match"], r["unit"], r["symbol"]))
    skips.sort()
    return rows, skips, our_syms


def original_text_functions():
    """All function names in the original .so .text — the coverage denominator."""
    addr2names, _ = delinker.load_symbols(delinker.DEFAULT_SYMS)
    ta, _, tsz = delinker.find_text_section(delinker.DEFAULT_SO)
    te = ta + tsz
    return {n for a, ns in addr2names.items() if ta <= a < te for n in ns}


def find_symbol_objects(build_dir, sym):
    """Locate the (base_o, target_o) that define a given symbol, for --show."""
    target_root = os.path.join(build_dir, "target")
    for unit, base_o in find_base_objects(build_dir):
        bf = asmdiff.disassemble(base_o)
        if sym in bf:
            target_o = os.path.join(target_root, unit + ".o")
            delink(base_o, target_o)
            return unit, base_o, target_o, bf
    return None


def demangle_many(names):
    """{mangled -> demangled} via `c++filt -n` (Apple/LLVM c++filt demangles Itanium names
    natively, so this needs no OrbStack). Best-effort: returns {} if c++filt is unavailable or
    the line count doesn't round-trip, in which case the wrong-type analysis is simply skipped."""
    names = list(names)
    if not names:
        return {}
    try:
        p = subprocess.run(["c++filt", "-n"], input="\n".join(names),
                           text=True, capture_output=True, timeout=120)
    except (FileNotFoundError, OSError, subprocess.SubprocessError):
        return {}
    out = p.stdout.splitlines()
    if len(out) != len(names):
        return {}
    return dict(zip(names, out))


def qualified_name(demangled):
    """The function's fully-qualified name with the parameter list stripped, e.g.
    'AbyssEngine::AEMath::Matrix::operator*=(Matrix const&)' -> '...::operator*='. Tracks
    angle-bracket depth so commas/parens inside template args or function-pointer parameters
    don't confuse the split, and steps over the parens in an 'operator()' name. Returns None
    when there is no top-level parameter list (i.e. the symbol isn't a function)."""
    depth = 0
    i, n = 0, len(demangled)
    while i < n:
        c = demangled[i]
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif c == "(" and depth == 0:
            if demangled[max(0, i - 8):i] == "operator":
                i += 1  # this '(' is part of the operator() name, not the param list
                continue
            return demangled[:i].strip()
        i += 1
    return None


def analyze_wrong_type(missing, our_syms):
    """Split the exact-name 'missing' originals into those we genuinely never wrote vs those we
    implement under a *different signature* (wrong parameter or qualifier types, so the mangled
    name differs and exact-name matching skips them). Match by demangled qualified name. Returns
    a list of {symbol, demangled, qualified, ours[]} for the present-but-wrong-type cases."""
    ours = sorted(s for s in our_syms if s.startswith("_Z"))
    dm_missing = demangle_many(missing)
    dm_ours = demangle_many(ours)
    if not dm_missing or not dm_ours:
        return []
    our_by_qual = {}
    for sym, dem in dm_ours.items():
        q = qualified_name(dem)
        if q:
            our_by_qual.setdefault(q, []).append(dem)
    hits = []
    for sym in missing:
        dem = dm_missing.get(sym)
        if not dem:
            continue
        q = qualified_name(dem)
        if q and q in our_by_qual:
            hits.append({"symbol": sym, "demangled": dem, "qualified": q,
                         "ours": sorted(set(our_by_qual[q]))})
    hits.sort(key=lambda h: h["qualified"])
    return hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build-dir", default=DEFAULT_BUILD)
    ap.add_argument("--only", default=None, help="regex filter on mangled symbol")
    ap.add_argument("--no-build", action="store_true",
                    help="skip the ARM compile step (reuse existing base/ objects)")
    ap.add_argument("--show", nargs="?", const="", default=None, metavar="SYMBOL",
                    help="print side-by-side disassembly diff for one symbol "
                         "(falls back to the FN environment variable)")
    ap.add_argument("--report", default=None, help="write JSON report to this path")
    ap.add_argument("--fail-on-wrong-type", action="store_true",
                    help="exit non-zero if any function is implemented under a different "
                         "(wrong) signature than the original binary (missing_wrong_type > 0)")
    args = ap.parse_args()

    if not args.no_build and not args.show:
        run(["bash", os.path.join(HERE, "build_objs.sh"), args.build_dir])

    if args.show is not None:
        sym = args.show or os.environ.get("FN", "")
        if not sym:
            print("set the symbol: --show <mangled> or FN=<mangled>")
            return 1
        found = find_symbol_objects(args.build_dir, sym)
        if not found:
            print(f"symbol not found in any base object: {sym}")
            return 1
        unit, base_o, target_o, bf = found
        tf = asmdiff.disassemble(target_o)
        print(f"# unit {unit}   symbol {sym}")
        r = asmdiff.compare(tf, bf)
        r = next((x for x in r if x["symbol"] == sym), None)
        if r:
            print(f"# match {r['match']}%   linked_equal={r['linked_equal']}   "
                  f"bytes_equal={r['bytes_equal']}   "
                  f"insns target={r['n_target']} base={r['n_base']}\n")
        print(asmdiff.unified(tf, bf, sym) or "(identical after normalization)")
        return 0

    rows, skips, our_syms = collect(args.build_dir, only=args.only)
    if not rows:
        print("No comparable functions found. Did the ARM build produce any .o files?")
        return 1

    perfect = sum(1 for r in rows if r["match"] >= 100)
    bytes_eq = sum(1 for r in rows if r["bytes_equal"])
    linked_eq = sum(1 for r in rows if r["linked_equal"])
    avg = sum(r["match"] for r in rows) / len(rows)

    # 'L' = byte-identical after linking (matches modulo relocation fields); '==' = raw
    # bytes identical (the rarer subset with no external refs at all). The unit column is sized to
    # the widest unit so the symbol column stays aligned, and symbols are printed in full.
    uw = max(len("unit"), max(len(r["unit"]) for r in rows))
    header = f"{'match':>7}  {'link':>4}  {'unit':<{uw}}  symbol"
    print()
    print(header)
    print("-" * len(header))
    for r in rows:
        flag = "==" if r["bytes_equal"] else ("L" if r["linked_equal"] else "")
        print(f"{r['match']:6.1f}%  {flag:>4}  {r['unit']:<{uw}}  {r['symbol']}")
    print("-" * len(header))

    report = {"avg_match": round(avg, 2), "count": len(rows),
              "fuzzy_perfect": perfect, "byte_exact": bytes_eq,
              "linked_exact": linked_eq, "skipped": len(skips)}

    # Coverage vs the original: functions in the .so .text that we never compared
    # (not decompiled/compiled yet, or whose signature mangles differently — e.g.
    # Array<T> vs std::vector). Skipped when --only narrows the set.
    if not args.only:
        universe = original_text_functions()
        compared = {r["symbol"] for r in rows}
        # Collapse aliased symbols before computing coverage. Many functions share ONE address in
        # the binary — most commonly the C1/C2 (complete/base) constructor pair and the D0/D1/D2
        # destructor variants, which the compiler emits as aliases when they'd be identical. objdump
        # and nm attribute only ONE name per address, so the aliased sibling looks "missing" even
        # though the function IS compared (and often byte-matches) under the other name. Treat a
        # binary symbol as compared if ANY symbol at its address is compared, so these aliases don't
        # masquerade as wrong-type/absent.
        addr2names, _ = delinker.load_symbols(delinker.DEFAULT_SYMS)
        name2addr = {n: a for a, ns in addr2names.items() for n in ns}
        compared_addrs = {name2addr[s] for s in compared if s in name2addr}
        # A symbol whose EXACT mangled name our build defines (in `our_syms`) is implemented under the
        # correct signature, period — the mangled name encodes the signature. It only fails to appear
        # in `compared` when its unit's delink/disasm hit a transient OrbStack wedge this run (the
        # retry-once doesn't always clear it on the big TUs). Treating such a defined symbol as
        # "missing" would mis-report it as absent/wrong-type and make the gate flaky run-to-run. This
        # does NOT hide real gaps: a symbol we genuinely don't define is not in our_syms and is still
        # reported. (Byte-match quality is tracked separately by linked_exact, per comparison.)
        missing = sorted(s for s in universe
                         if s not in compared and s not in our_syms
                         and name2addr.get(s) not in compared_addrs)
        # Of the missing originals, which do we actually implement — just under a different
        # signature (wrong param/qualifier types -> different mangled name -> skipped by the
        # exact-name match)? These are the high-value fixes: the body exists, only the type is
        # off. The rest are genuinely not built yet.
        wrong_type = analyze_wrong_type(missing, our_syms)
        wrong_type_syms = {h["symbol"] for h in wrong_type}
        absent = [m for m in missing if m not in wrong_type_syms]
        # The inverse of coverage: function symbols our build defines that the original binary has
        # nowhere (compared by name against the FULL binary symbol table — any address/section — so
        # ctor/dtor aliases and non-.text symbols aren't falsely flagged). Mostly intentional
        # re-expression scaffolding (helper/shim free functions, renamed emulation methods like
        # String::ctor/dtor and the *_ext_* shims), surfaced so a genuinely accidental extra — a
        # stray or wrongly-named function — is easy to spot. Informational, not gated.
        binary_names = {n for ns in addr2names.values() for n in ns}
        extra = sorted(s for s in our_syms if s not in binary_names)
        report.update({"original_functions": len(universe),
                       "compared_unique": len(compared), "missing": len(missing),
                       "missing_wrong_type": len(wrong_type),
                       "missing_absent": len(absent),
                       "wrong_type": wrong_type,
                       "extra_symbols": len(extra), "extra": extra})
        miss_path = os.path.join(args.build_dir, "missing.txt")
        os.makedirs(os.path.dirname(miss_path), exist_ok=True)
        with open(miss_path, "w") as f:
            f.write("\n".join(missing) + ("\n" if missing else ""))
        # Present-but-wrong-type, grouped by qualified name with both sides' signatures, so the
        # fix (retype our params to match the original) is reads straight off the file.
        wt_path = os.path.join(args.build_dir, "missing_wrong_type.txt")
        with open(wt_path, "w") as f:
            for h in wrong_type:
                f.write(f"{h['qualified']}\n")
                f.write(f"    original: {h['demangled']}\n")
                f.write(f"      mangled {h['symbol']}\n")
                for dem in h["ours"]:
                    f.write(f"    ours:     {dem}\n")
                f.write("\n")
        # Symbols we emit that the original binary doesn't define (mangled + demangled, one per
        # line) — the inverse of missing.txt, for auditing our build's surface vs the original.
        extra_path = os.path.join(args.build_dir, "extra.txt")
        dm_extra = demangle_many(extra)
        with open(extra_path, "w") as f:
            for s in extra:
                f.write(f"{s}    {dm_extra.get(s, s)}\n")

    print(f"{len(rows)} comparisons   avg {avg:.1f}%   "
          f"100%-fuzzy {perfect}   linked-exact {linked_eq}   byte-exact {bytes_eq}")
    if not args.only:
        print(f"coverage: compared {report['compared_unique']}/{report['original_functions']} "
              f"original functions   missing {report['missing']} "
              f"(-> {os.path.relpath(miss_path, REPO)})")
        if report["missing_wrong_type"]:
            print(f"  of which {report['missing_wrong_type']} are implemented under a different "
                  f"signature (wrong type) "
                  f"(-> {os.path.relpath(wt_path, REPO)}); "
                  f"{report['missing_absent']} genuinely absent")
        print(f"extra: {report['extra_symbols']} symbols our build defines that the original "
              f"doesn't (-> {os.path.relpath(extra_path, REPO)})")
    if skips:
        units = ", ".join(u for u, _ in skips)
        print(f"skipped {len(skips)} units (couldn't delink/diff): {units}")

    report["functions"] = rows
    out = args.report or os.path.join(args.build_dir, "report.json")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    json.dump(report, open(out, "w"), indent=2)
    print(f"report -> {os.path.relpath(out, REPO)}")

    # Regression gate: every function we implement must mangle to the original's signature.
    # missing_wrong_type is only populated on a full run (no --only), so this is inert for
    # narrowed/--show invocations. Runs last so the table, missing_wrong_type.txt and
    # report.json are all still written even when we fail the build.
    if args.fail_on_wrong_type and report.get("missing_wrong_type", 0) > 0:
        print(f"\nERROR: {report['missing_wrong_type']} function(s) are implemented under a "
              f"different (wrong) signature than the original binary "
              f"(-> {os.path.relpath(wt_path, REPO)}). "
              f"Fix the signature(s) so the count returns to 0.", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
