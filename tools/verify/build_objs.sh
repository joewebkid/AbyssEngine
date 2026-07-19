#!/usr/bin/env bash
# Compile every src/*.cpp with the matching NDK r18b toolchain (OrbStack or local fallback) into
# <build-dir>/base/<rel>.o, resiliently: a TU that fails to compile is skipped and
# reported, never aborting the run (not every recovered TU compiles yet). Objects
# are the "base" side fed to the verify differ.
#
# Usage: build_objs.sh [build-dir]   (default: cmake-build-match/verify)
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/../.." && pwd)"
BUILD="${1:-$REPO/cmake-build-match/verify}"
JOBS="${GOF2_VERIFY_JOBS:-8}"
cd "$REPO"
. "$HERE/match_flags.sh"

BASE="$BUILD/base"
LOGS="$BUILD/logs"
mkdir -p "$BASE" "$LOGS"
# Only touch .flags when the flag set actually changes, so unchanged flags don't
# force a full rebuild (objects are compared against this file's mtime below).
if [ ! -f "$BUILD/.flags" ] || [ "$(cat "$BUILD/.flags")" != "$GOF2_MATCH_CXXFLAGS" ]; then
  echo "$GOF2_MATCH_CXXFLAGS" > "$BUILD/.flags"
fi

# Is $obj still up to date? It is stale (returns 1) if it is missing, older than its source or the
# flag set, or — using the compiler-generated .d dependency file — older than ANY header it
# includes. The .d (from -MMD) is what makes header edits trigger a rebuild; without it a changed
# header would silently leave a stale object and skew the report. (Paths in this tree contain no
# spaces, so plain word-splitting of the .d is safe.)
up_to_date() {
  local obj="$1" src="$2" dep="$3" prereq
  [ -f "$obj" ]              || return 1
  [ "$obj" -nt "$src" ]      || return 1
  [ "$obj" -nt "$BUILD/.flags" ] || return 1
  [ -f "$dep" ]              || return 1   # no dependency info yet -> rebuild to generate it
  for prereq in $(sed -e 's/^[^:]*://' -e 's/\\$//' "$dep"); do
    [ -e "$prereq" ] && [ "$prereq" -nt "$obj" ] && return 1
  done
  return 0
}

# Recompile a TU when its object is missing or older than the source, the flag set, or any header
# it includes (tracked via the .d file emitted by -MMD).
compile_one() {
  local src="$1"
  local rel="${src#src/}"; rel="${rel%.cpp}"
  local obj="$BASE/$rel.o"
  local dep="$BASE/$rel.d"
  local log="$LOGS/$rel.log"
  mkdir -p "$(dirname "$obj")" "$(dirname "$log")"
  if up_to_date "$obj" "$src" "$dep"; then
    return 0
  fi
  # src/runtime/ holds thin wrappers that compile NDK toolchain runtime sources (e.g. libc++abi's
  # own operator new/delete in src/runtime/ndk_libcxxabi_new_delete.cpp). They build with the
  # toolchain library's own flags — the libc++abi src/ dir on the include path and the C++17
  # aligned-allocation feature — so the objects byte-match what the binary linked from libc++abi.
  local extra=""
  case "$src" in
    src/runtime/*) extra="-faligned-allocation -isystem ${NDK:-/opt/android-ndk-r18b}/sources/cxx-stl/llvm-libc++abi/src" ;;
  esac
  # Retry transient failures (wrapper/toolchain hiccups under parallel load drop TUs and cause
  # false "absent" symbols). A real compile error prints `error:` -> fail fast, don't waste retries.
  local attempt
  for attempt in 1 2 3; do
    if "$HERE/orbcc" $GOF2_MATCH_CXXFLAGS $extra -MMD -MF "$dep" -c "$src" -o "$obj" >"$log" 2>&1; then
      rm -f "$log"
      return 0
    fi
    grep -q 'error:' "$log" && break   # genuine compile error, not a transient
  done
  rm -f "$obj"
  return 1
}
export -f up_to_date compile_one
export HERE BASE LOGS BUILD GOF2_MATCH_CXXFLAGS NDK

# (portable: macOS ships bash 3.2 which lacks `mapfile`)
total=$(find src -name '*.cpp' | wc -l | tr -d ' ')
echo "[build] $total TUs -> $BASE  (jobs=$JOBS, opt=${GOF2_MATCH_OPT:-?})"
find src -name '*.cpp' | sort | xargs -P "$JOBS" -I{} bash -c 'compile_one "$@"' _ {}

ok=$(find "$BASE" -name '*.o' | wc -l | tr -d ' ')
fail=$(find "$LOGS" -name '*.log' | wc -l | tr -d ' ')
echo "[build] compiled $ok, failed $fail"
if [ "$fail" -gt 0 ]; then
  echo "[build] top failure reasons:"
  grep -rhoE 'error: .*' "$LOGS" 2>/dev/null | sed -E 's/0x[0-9a-f]+/0x_/g; s/[0-9]+/N/g' \
    | sort | uniq -c | sort -rn | head -8 | sed 's/^/    /'
  echo "[build] (per-file logs under $LOGS)"
fi
exit 0
