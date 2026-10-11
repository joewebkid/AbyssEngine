# TouchSlider touch-padding source — Android ARM (2026-10-11)

## Evidence and correction

`TouchSlider::TouchSlider(int, int, int, float)` at Android 2.0.16 ELF
address `0xa228c` reads `Globals::layout` through GOT slot `0x210224`,
then loads the integer at layout offset `0x7c` and stores it at slider
offset `0x38`. Both the IDA dump and ARM instructions at `0xa230e`–`0xa231a`
show this. The former C++ read the same offset from a translation-unit-local
null placeholder misidentified as an application handle. It now reads
`Globals::layout->field_0x7c`; the unused fake config object was removed.

The canvas path was left untouched: its native pointer is initialized
through `PaintCanvas::gCanvas`, and `TouchSlider::draw` already has a
linked-exact ARM match. This package fixes only the proven null access.

## Verification

- Native `gof2` static-library build: pass.
- ARM objects: 206/206 built; focused `TouchSlider` verification: 15 functions.
- Verifier tests: 36 run, 35 passed, one skipped.
- Constructor aliases: source-shape 86.3% to 85.1%; strict fuzzy 68.7%
  to 72.5%. Family average source-shape 98.17% to 98.01%, strict fuzzy
  95.59% to 96.09%; 12 linked-exact and nine byte-exact retained.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions
  compared, 201 missing, zero skipped or wrong-type symbols; 92.32%
  source-shape, 80.46% strict fuzzy, 2,353 linked-exact, 1,008 byte-exact.
  Per-symbol comparison to the prior report found zero exact-pair losses.
  Report: `A:/Galaxy on Fire new version/_work/touchslider_layout_full_20261011.json`.

## Remaining limit

The corrected field source is ARM-confirmed, but a live touch interaction
has not been run because the full native executable still has unresolved
dependencies. Next audit an independent UI placeholder family; avoid
changing already exact functions without separate behavior evidence.
