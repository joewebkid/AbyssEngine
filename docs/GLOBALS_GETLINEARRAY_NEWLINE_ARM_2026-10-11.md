# Globals getLineArray trailing newline — Android ARM (2026-10-11)

## Evidence and correction

The Android 2.0.16 `Globals::getLineArray` function begins at ELF address
`0xe4384`. At `0xe43b4` it loads a string literal and constructs a `String`
from it before appending that string to the input at `0xe43c0`–`0xe43c6`.
The referenced `.rodata` bytes at `0x1ca4c6` are `0a 00`, i.e. `"\n"`.
The IDA pseudocode independently shows the same literal.

The recovered `gGLA_newline` was empty, so the input lacked the original
sentinel newline. It is now `"\n"`; the two-pass split, line allocation,
and leading/trailing-space trimming remain unchanged. Callers include
`ScrollTouchBox`, `StatusWindow`, and `Radio`, where line count affects
layout or display duration.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Globals` comparison: 49 functions, ten linked-exact and one
  byte-exact; `getLineArray` remains 82.7% source-shape and 42.6% strict
  fuzzy. These instruction-shape scores do not validate the literal value.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type; 92.33% average source-shape,
  80.47% strict fuzzy, 2,353 linked-exact, and 1,008 byte-exact. A
  per-symbol comparison against the preceding full report found no metric
  changes or exact-pair losses. Report:
  `A:/Galaxy on Fire new version/_work/globals_getlinearray_full_20261011.json`.

No live native text-layout run was performed; the native target is a static
library, and the complete executable is not yet linkable. Other machine-code
differences in `getLineArray` remain; they require separate semantic review
before claiming runtime fidelity.
