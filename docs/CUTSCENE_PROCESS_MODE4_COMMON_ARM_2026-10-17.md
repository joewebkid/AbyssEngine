# CutScene mode-4 common transform updates (2026-10-17)

The Android ARM mode-4 path in `CutScene::process(int)` at
`0x98f08..0x98f58` loops over enemies, skips null entries, and for
each present enemy reads its geometry at `KIPlayer+0x08`. It passes
the geometry's primary transform ID at `AEGeometry+0x0c` to the
global canvas, then calls `Transform::Update` with the sign-extended
32-bit `CutScene+0x58` frame delta and `false`.

When the system race is zero, the separate path at
`0x9924c..0x99264` reads `CutScene+0x30` (`geom30`), obtains that
geometry's `+0x0c` transform, and calls the same update. The
original has no null guard for `geom30` in this branch. The previous
C++ used transform ID zero in both paths and converted the low byte
of frame delta to the update flag. The recovered paths now follow
the observed IDs and argument values.

An extracted host `-O0/-O2` smoke test exercises a three-entry
enemy array with a null middle entry and both ordinary and high-bit
frame deltas, checking the two enemy transforms and race-zero
geometry separately. Focused ARM `process` source-shape/strict fuzzy
rises from **90.1%/50.8%** (`669/706` rebuilt/original instructions)
to **90.2%/53.2%** (`680/706`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the preceding mode-23 process smokes pass. Full-corpus
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode4_common_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_process_mode23_anim_full_20261017.json`.

Live ARM32 gameplay/GPU output remains untested. The mode-4 race-1,
race-2 and race-3 transform branches still have placeholder IDs and
need their own ARM audits.
