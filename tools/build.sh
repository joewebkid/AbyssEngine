#!/usr/bin/env bash
# build.sh — compile the cleaned native/ tree and inventory the result.
# Pass 1: compile every src/*.cpp -> build/obj/*.o (64-bit macOS, no byte-matching).
# Reports per-file pass/fail + the aggregate error-category histogram for whatever still fails.
set -u
cd "$(dirname "$0")/.."
OBJ=build/obj; mkdir -p "$OBJ"; LOG=build/errs.txt; : >"$LOG"
CXX="clang++ -std=gnu++14 -fpermissive -w -ferror-limit=0 -Iinclude"
ok=0; fail=0; faillist=""
for f in src/*.cpp; do
  [ -f "$f" ] || continue
  c=$(basename "$f" .cpp)
  if $CXX -c "$f" -o "$OBJ/$c.o" 2>>"$LOG"; then ok=$((ok+1)); else fail=$((fail+1)); faillist="$faillist $c"; fi
done
echo "=== native compile: $ok ok, $fail failing (of $((ok+fail))) ==="
if [ "$fail" -gt 0 ]; then
  echo "--- failing classes:$faillist"
  echo "--- top error categories:"
  grep -oE 'error: .*' "$LOG" | sed -E 's/[0-9]+/N/g; s/'\''[^'\'']*'\''/X/g' | sort | uniq -c | sort -rn | head -15
fi
