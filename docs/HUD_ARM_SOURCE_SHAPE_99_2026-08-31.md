# Hud ARM source-shape 99% audit (2026-08-31)

## Scope and evidence

This pass audits all 48 `_ZN3Hud...` symbols against the Android HD ARMv7
binary. The source-backed behavior was checked against:

- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`;
- `analysis/gof2_libgof2hdaa_full_ida.c`;
- the delinked Android functions in `_work/hud_target_fresh_20260831.o`;
- NDK r18b Clang 7 output in
  `cmake-build-match/verify/base/game/ui/Hud.o`.

The Android binary and generated `_work` objects are local evidence and are not
repository deliverables.

## Accepted recovery work

- `Globals::Canvas`, `layout`, `gameText`, and the 32-bit font handle now carry
  their confirmed native types. Hud and the direct Hangar/Menu/Radar consumers
  no longer recover these values through unrelated pointer casts.
- `drawOrbitInformation` keeps the five native String lifetimes and constructs
  the system/security label as one nested expression. This is the best verified
  source form and raises the strict family average from `95.6104%` to
  `95.6146%`.
- The strict comparator now proves and normalizes stripped jump-table payloads,
  PC-referenced literal pools and pool-alignment NOPs. `SequenceMatcher` runs
  with `autojunk=False`, so repeated loads/calls in large functions remain real
  evidence instead of being discarded as text noise.
- `asmdiff.py` reports a second, explicitly non-byte-exact `source_match`. It is
  the equal-weight mean of allocation-normalized instruction inventory, opcode
  inventory, ordered control-flow opcode sequence and instruction-count
  agreement. Exact register choice and local stack-slot numbering are ignored;
  calls, branch order/kind, constants and object offsets remain significant.

Six unit tests cover jump tables, literal pools, alignment and the source metric.
The source metric reaches 100 for register/stack-slot-only variation and drops
below 100 when a field offset or control-flow sequence changes.

## Rejected forms

Every rejected form below was reverted before the final object was built:

- forcing `Hud::draw` to retain `this` in ARM `r5` reduced `draw` from `59.8%`
  to `41.0%` and the family average to `95.22%`;
- removing the long-lived local Canvas reduced `draw` to `52.2%` and the family
  average to `95.45%`;
- explicit `r8/r9/r10` bindings in `drawOrbitInformation` generated no useful
  change and were removed;
- the literal Hex-Rays ternary for the existing-key branch in `touchBegin`
  reduced that function from `87.0%` to `59.4%` and the family average to
  `95.04%`;
- forcing both successful `touchBegin` paths through a source-level `goto`
  reduced the function to `37.9%` and the family average to `94.59%`;
- synthetic stack-canary scratch objects and direct Hex-Rays-shaped challenge
  rewrites were also rejected because their ARM score regressed.

These results are retained as evidence that decompiler variable names, labels
and register comments are not the original C++ source shape.

## Final Hud family report

Focused command:

```powershell
$env:GOF2_VERIFY_LOCAL_NDK = "1"
python tools/verify/verify.py --build-dir cmake-build-match/verify --no-build `
  --only '^_ZN3Hud' --report _work/hud_family_source99_20260831.json
```

Result for 48 functions:

| Signal | Family average |
| --- | ---: |
| source shape | **99.24%** |
| instruction-shape inventory | 98.75% |
| opcode inventory | 99.35% |
| ordered control flow | 99.21% |
| instruction-count agreement | 99.65% |
| strict ordered fuzzy | 95.61% |

Exact counts are 36 strict-fuzzy `100%`, 32 linked-exact and 20 raw
byte-exact functions. The current source-shape metric was also run against 17
older Hud objects: all stayed below 99 (`97.21%..98.96%`), while the current
object reached `99.2375%`. This negative control demonstrates that the metric
does not award the milestone to the known weaker revisions.

The full cached ARM corpus was then rechecked with the same comparator:

- 4,445 comparable functions;
- 1,933 linked-exact and 877 raw byte-exact;
- 4,130 of 4,524 original functions covered;
- full-corpus averages `74.7%` strict fuzzy and `86.9%` source shape.

The wide result is recorded locally in
`_work/arm_corpus_source_shape_20260831.json`. Native UCRT64 verification also
rebuilt and linked `libgof2.a` successfully. Existing warnings in unrelated
legacy C-style recovery units remain, but this package introduces no native
build failure.

## Remaining byte-match work

The 99% result is a source-shape milestone, not whole-class byte matching. The
largest strict differences remain:

- `initHudMenu`: `52.6%`, `1223/1197` instructions;
- `draw`: `59.8%`, `3142/3056`;
- `drawChallengeModeScore`: `66.7%`, `342/339`;
- `drawOrbitInformation`: `75.8%`, `214/219`;
- `init`: `83.3%`, `1079/1066`;
- `touchBegin`: `87.0%`, `65/66`;
- `drawMenu`: `90.0%`, `219/221`;
- `hudEvent`: `90.9%`, `974/982`.

Further work on these functions must continue to report strict, linked and byte
evidence separately. No source-shape score may be described as byte-match.
