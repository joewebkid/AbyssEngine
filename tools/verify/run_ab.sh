#!/usr/bin/env bash
# A/B the optimization level on equal coverage, via the cmake `verify` target
# (parallel build + parallel diff). Saves a report per opt level, then prints the
# intersection-only head-to-head. Run from the repo root.
set -u
cd "$(dirname "$0")/../.." || exit 1
export GOF2_VERIFY_JOBS="${GOF2_VERIFY_JOBS:-8}"
V=cmake-build-match/verify

run_opt() {
  local opt="$1" out="$2"
  echo "=========================================================="
  echo "[A/B] building + diffing at opt=$opt  (jobs=$GOF2_VERIFY_JOBS)"
  echo "=========================================================="
  cmake --preset match -DGOF2_MATCH_OPT="$opt" >/dev/null 2>&1
  cmake --build cmake-build-match --target verify 2>&1 | tail -4
  cp "$V/report.json" "$out"
  echo "[A/B] saved $out"
}

run_opt -Oz "$V/report-Oz-fixed.json"
run_opt -O2 "$V/report-O2-fixed.json"

echo
echo "=========================================================="
echo "[A/B] intersection-only comparison (functions compared in BOTH)"
echo "=========================================================="
python3 - "$V/report-Oz-fixed.json" "$V/report-O2-fixed.json" <<'PY'
import json, sys
oz = json.load(open(sys.argv[1])); o2 = json.load(open(sys.argv[2]))
def idx(d): return {r["symbol"]: r for r in d["functions"]}
a, b = idx(oz), idx(o2)
common = sorted(set(a) & set(b))
def stats(m, keys):
    be = sum(1 for k in keys if m[k]["bytes_equal"])
    avg = sum(m[k]["match"] for k in keys) / len(keys) if keys else 0
    return be, avg
oz_be, oz_avg = stats(a, common)
o2_be, o2_avg = stats(b, common)
print(f"compared in both: {len(common)}   (Oz total {oz['count']}, O2 total {o2['count']})")
print(f"  -Oz : byte-exact {oz_be:4d}   avg-match {oz_avg:5.1f}%   (overall report byte-exact {oz['byte_exact']})")
print(f"  -O2 : byte-exact {o2_be:4d}   avg-match {o2_avg:5.1f}%   (overall report byte-exact {o2['byte_exact']})")
# functions byte-exact under exactly one opt level (decisive evidence)
only_oz = [k for k in common if a[k]["bytes_equal"] and not b[k]["bytes_equal"]]
only_o2 = [k for k in common if b[k]["bytes_equal"] and not a[k]["bytes_equal"]]
print(f"  byte-exact only at -Oz: {len(only_oz)}    byte-exact only at -O2: {len(only_o2)}")
winner = "-O2" if o2_be > oz_be else ("-Oz" if oz_be > o2_be else "tie")
print(f"  => more byte-exact: {winner}")
PY
