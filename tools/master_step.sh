#!/usr/bin/env bash
# master_step.sh RESULT [wave_prepare args...] — one master-loop iteration AFTER a fix wave.
#
# RESULT may be either the wave's result JSON, or the Workflow task .output file (which wraps the
# result under .result) — auto-detected.
#
#   1. apply the wave's caller_rewrites / missing-fields / deferred (serial, race-free)
#   2. re-run verify (regenerate report.json + missing/extra/wrong_type)
#   3. REGRESSION GUARD: linked-exact must not drop (goal counts churn as TUs unlock, but a drop in
#      linked-exact means we broke a previously-matching function) — flagged loudly if it does
#   4. scope_filter -> in-scope counts (the "done" gate)
#   5. dispatch_worklist -> fresh worklist.json
#   6. commit; 7. prepare the next wave
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
cd "$REPO"
RESULT_IN="${1:?usage: master_step.sh RESULT [wave_prepare args...]}"; shift
PREP_ARGS=("$@"); [ ${#PREP_ARGS[@]} -eq 0 ] && PREP_ARGS=(--kinds absent --limit 40 --max-entries 6)
V=cmake-build-match/verify

# normalize RESULT -> a plain result json at $V/_wave_result.json
python3 - "$RESULT_IN" <<'PY'
import json, sys
d = json.load(open(sys.argv[1]))
res = d.get("result", d) if isinstance(d, dict) else d
json.dump(res, open("cmake-build-match/verify/_wave_result.json", "w"), indent=1)
PY
RESULT="$V/_wave_result.json"

# prior linked-exact (for the regression guard)
PREV_LINKED=$(python3 -c "import json;print(json.load(open('$V/report.json')).get('linked_exact',0))" 2>/dev/null || echo 0)

echo "== 1. apply wave results =="
python3 tools/apply_wave_results.py "$RESULT"

echo "== 2. verify =="
python3 tools/verify/verify.py --build-dir "$V" >"$V/_verify_run.log" 2>&1
grep -E "comparisons|coverage" "$V/_verify_run.log"

echo "== 3. regression guard (linked-exact) =="
NOW_LINKED=$(python3 -c "import json;print(json.load(open('$V/report.json')).get('linked_exact',0))")
echo "   linked-exact: $PREV_LINKED -> $NOW_LINKED"
if [ "$NOW_LINKED" -lt "$PREV_LINKED" ]; then
  echo "   !!! REGRESSION: linked-exact dropped by $((PREV_LINKED-NOW_LINKED)). Inspect failing TUs:"
  find "$V/logs" -name '*.log' | sed 's/^/      /'
fi
FAIL_TUS=$(find "$V/logs" -name '*.log' | wc -l | tr -d ' ')
[ "$FAIL_TUS" -gt 0 ] && echo "   failing-compile TUs: $FAIL_TUS (-> $V/logs)"

echo "== 3b. hack lint (no byte-match ctor/dtor asm overrides) =="
python3 tools/lint_hacks.py | sed 's/^/   /' || echo "   !!! a wave introduced a byte-match hack — rewrite it as clean C++ before continuing"

echo "== 4. scope_filter =="
python3 tools/scope_filter.py

echo "== 5. dispatch =="
python3 tools/dispatch_worklist.py | sed 's/^/   /'

echo "== 6. commit =="
git add -A
if git diff --cached --quiet; then echo "   (nothing to commit)"; else
  N=$(python3 -c "import json;c=json.load(open('$V/scope_counts.json'));print(f\"absent {c['absent']} wrong_type {c['wrong_type']} extra {c['extra']}\")")
  git commit -q -m "fix wave: in-scope $N (linked-exact $NOW_LINKED)

Co-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>
Claude-Session: https://claude.ai/code/session_01Vq3g9jxkb38oUx9XXFf4Ht" && echo "   committed ($N)"
fi

echo "== 7. prepare next wave =="
python3 tools/wave_prepare.py "${PREP_ARGS[@]}"

echo "== DONE-CHECK =="
python3 -c "
import json
c=json.load(open('$V/scope_counts.json'))
print('SCOPE_COUNTS', json.dumps(c))
print('ALL_ZERO' if all(v==0 for v in c.values()) else 'CONTINUE')
"
