# MiningGame layer membership (2026-10-14)

## Evidence and recovery

Android `MiningGame::isInCurrentLayer()` starts at `0x11f0a0` in the
IDA export (`analysis/gof2_libgof2hdaa_full_ida.c`, lines
157146–157159). It computes the squared distance from the current
mining center, reads a diameter from the 7×7 `LAYER_DIAMETERS` table,
uses `Layout+0xe8` as the radius scale, calls `Globals::sqrt` through
`Globals::globals`, and compares against half the integer diameter.

The original ELF exports `LAYER_DIAMETERS` at `0x215d64` as a
196-byte object. Its 49 little-endian 32-bit values were recovered
into the source table. The function now uses that table,
`Globals::layout`, and `Globals::sqrt` directly instead of three
unassigned holders and a placeholder square-root call. The integer
division by two remains before conversion to `float`, matching the
decompiled expression.

The production-method host smoke checks the table endpoints,
inside/outside threshold boundaries at scales 1.0 and 1.5, a second
table row, two-axis distance, and a zero-diameter slot. Native
`gof2`, all 206 ARM translation units and 37 verifier tests pass
(one skipped). Focused source-shape rises **0.5% → 99.1%** and strict
fuzzy **0.0% → 81.8%**. Both generated and target bodies contain 44
instructions, but the function is neither linked-exact nor
byte-exact.

The full ARM corpus has 4,718 comparisons and 4,323/4,524 original
functions covered (201 missing). Average source-shape moves from
92.98% to 93.00%, while strict fuzzy moves from 81.41% to 81.43%.
All 2,403 linked-exact and 1,008 byte-exact pairs are unchanged, as
is the 123-symbol extra set; there are no exact-set losses. The
focused 99.1% is a shape comparison, not a claim of exact code or
validated ARM32 runtime behavior.

`MiningGame::update`, its constructor and rendering path still have
unassigned global/service holders. This host smoke validates only the
layer-membership calculation, not an executable ARM32 mining
sequence. Next audit the `update` dependencies and the exported
`LAYER_SPEEDS` table before claiming runtime progress.
