# CutScene mode-4 race-2 process path (2026-10-17)

The Android ARM race-2 path in `CutScene::process(int)` at
`0x995c2..0x99634` reads `Level::getEnemies()` twice to select
array element `count - 2`, then reads it twice more to select
`count - 1`. Each selected player's geometry at `KIPlayer+0x08`
supplies its primary transform ID at `AEGeometry+0x0c` to the global
canvas. Each `Transform::Update` gets the sign-extended 32-bit
`CutScene+0x58` frame delta and `false`. The second update reuses
the shared tail at `0x9925e`. These indices, offsets and argument
registers were checked directly in the original Android ARM.

The previous C++ discarded all four enemy-array results, used
transform ID zero twice and converted the low frame-delta byte into
the boolean flag. The recovered path now follows the observed data
flow. An extracted host `-O0/-O2` smoke test checks the two distinct
array indices, four list reads, both IDs, ordinary and high-bit frame
deltas, and the non-race-2 return. The original has no bounds or null
checks for these selected enemies; none were invented in this patch.

Focused ARM `process` source-shape/strict fuzzy rises from
**92.1%/59.7%** (`688/706` rebuilt/original instructions) to
**93.9%/62.8%** (`692/706`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the preceding mode-4 race-3/race-1 smokes pass. Full-corpus
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race2_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_process_mode4_race3_full_20261017.json`.

The host test does not prove live ARM32 gameplay or GPU output.
Mode-2 `CutScene::process` ship motion still contains unverified
placeholder coefficients and needs a separate ARM audit.
