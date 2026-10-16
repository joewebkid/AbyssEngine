# CutScene mode-23 process transform (2026-10-17)

The Android ARM `CutScene::process(int)` tail for mode 23 at
`0x9950a..0x995aa` reads the first enemy's geometry and uses its
`AEGeometry+0x0c` transform ID for `TransformGetLocal`. It copies
60 bytes of that local matrix, rotates the copy around Y by
`CutScene+0x04` (`cameraRotX`), re-reads the first enemy and submits
the copy through `TransformSetLocal` using the same geometry field.

The following loop checks each enemy pointer, then obtains that
enemy's geometry transform and calls `Transform::Update` with the
sign-extended 32-bit frame delta from `CutScene+0x58` and `false`.
The transform IDs, `cameraRotX` load, 60-byte copy, signed high word
and zero boolean are all visible in ARM. The previous C++ instead
used transform ID zero, zero rotation, interpreted an integer ID as
`Matrix*`, and passed the low byte of frame delta as the boolean.

An extracted host test checks matrix copy/rotation/submission,
skipping a null enemy, both non-null transform IDs and signed delta
propagation for ordinary and high-bit frame deltas at `-O0/-O2`.
Focused ARM `process` improves from **88.3% source-shape / 43.7%
strict fuzzy** (`650/706` rebuilt/original instructions) to
**89.1% / 47.8%** (`665/706`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the earlier CutScene setup smokes pass. Full-corpus results
are recorded in
`A:/Galaxy on Fire new version/_work/cutscene_process_mode23_full_20261017.json`:
**4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded source-shape/strict fuzzy averages,
**2,517 linked-exact** and **1,076 byte-exact**. The exact
`(unit,symbol)` sets are unchanged from
`cutscene_reset_camera_full_20261017.json`.

The host test does not verify live ARM32 gameplay or GPU output.
Earlier mode-23 animation/fog paths and the mode-4 process branches
still contain placeholders; those need separate ARM audits.
