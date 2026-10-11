# Globals getLine canvas and break handling — Android ARM (2026-10-11)

## Evidence and correction

Android 2.0.16 `Globals::getLine(unsigned, String, int, String *)` at ELF
address `0xe4230` measures each character through `Globals::Canvas`.
At `0xe42a4`–`0xe42c4`, the ARM branches continue only while the measured
width is below the limit and the current character is neither LF nor CR.
The newline path copies through the current character (`i + 1`). The
overflow path at `0xe42f8`–`0xe433a` uses the last space if its index is
at least one; otherwise it copies through `i + 1`.

The recovered C++ instead dereferenced a translation-unit-local null
canvas pointer and checked LF/CR only inside the overflow branch. It now
uses live `Globals::Canvas` and the ARM-confirmed termination condition.
The existing last-space choice on overflow remains. `getLineArray` and
`getBoundedString` call this method; their own logic was not changed.

## Verification

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused function: source-shape 84.1% to 87.7%; strict fuzzy 34.8%
  to 36.3%. The 49-function `Globals` unit retains ten linked-exact and
  one byte-exact matches.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions
  compared, 201 missing, zero skipped or wrong-type symbols; 92.33%
  source-shape, 80.47% strict fuzzy, 2,353 linked-exact, 1,008 byte-exact.
  Per-symbol comparison against the preceding full report found zero
  exact-pair losses. Report:
  `A:/Galaxy on Fire new version/_work/globals_getline_full_20261011.json`.

## Remaining limit and next priority

The ARM control-flow comparison is evidence for branch order, not a live
text-rendering or Unicode-width test. Full native executable linking
remains incomplete. Next audit downstream `getLineArray` construction and
trimming against ARM before making broader text-layout claims.
