# Layout constructor: platform metrics (2026-10-14)

## Evidence and scope

The Android `Layout::Layout()` constructor starts at `0xd1028` in the
IDA export (`analysis/gof2_libgof2hdaa_full_ida.c`, around line 127730).
Its C1/C2 aliases each contain 2,225 target ARM instructions. The
existing source represented only 154 instructions and dereferenced
eight unassigned configuration pointers. This pass replaces those
pointers with the matching `Globals::iPadHD`, `n9`, `retinaDisplay`,
`iPad`, and `iPadLarge` flags.

The first 22 consecutive integer metrics (`Layout+0x4..+0x58`) now
follow the Android branches for iPad HD, N9, ordinary Retina, and
ordinary non-Retina. The directly observed tail values at
`+0x1f0..+0x200`, `+0x238`, `+0x27c..+0x290`, `+0x2bc`, and
`+0x2f4..+0x320` now include reward-box dimensions, choice-window
metrics, help-window placement, and the float bit pattern stored at
`+0x2f8`. Previously restored list-item and Hangar fields remain.

This is **not** a complete constructor recovery. The broad zero-fill
between the early metrics and the tail is still a source placeholder;
the original writes many nonzero platform-specific fields there.
The iPad HD scrollbar values and N9 byte flags in the IDA output have
ambiguous temporary-variable provenance and need direct ARM/dataflow
confirmation. Do not treat host smoke results as original ARM32 runtime
or visual-layout proof.

## Checks

The focused ARM comparison for both C1/C2 aliases rises from 8.7% to
23.9% source-shape and from 2.9% to 5.8% strict fuzzy. Our generated
body grows from 154 to 565 instructions against 2,225 target
instructions; neither alias is exact. The production-method host smoke
now exercises the constructor for ordinary, Retina, N9, iPad HD,
iPad, and iPad Large profiles. Native `gof2`, all 206 ARM translation
units, and 37 verifier tests pass (one skipped).

The full corpus remains at 4,323/4,524 covered (201 missing) across
4,718 comparisons. Average source-shape rises from 92.97% to 92.98%;
strict fuzzy remains 81.41%. All 2,403 linked-exact and 1,008
byte-exact pairs are preserved, and the 123 extra-symbol set is
unchanged.

Next: recover the omitted `+0x5c..+0x2d8` metrics in bounded,
consumer-backed groups, confirm ambiguous platform values from ARM,
then validate layout behavior on-device or with a faithful ARM32
runtime. Re-run the full corpus and compare exact and extra-symbol
sets before accepting each group.
