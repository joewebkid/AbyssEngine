# CutScene mode-4 race-3 process path (2026-10-17)

The Android ARM race-3 path in `CutScene::process(int)` at
`0x992ac..0x993d4` reads the enemy array twice and selects element
`count - 3`, as in race 1. It updates that player's geometry
transform (`KIPlayer+0x08`, `AEGeometry+0x0c`) with sign-extended
frame delta and `false`. It then updates optional `CutScene+0x28`
(`geom28`) and mandatory `CutScene+0x2c` (`geom2c`) primary
transforms with the same arguments.

The `+0x7c/+0x80` timers advance together. Above 1000, and only
when `geom28` exists, the first timer resets; a bounded
`AERandom::nextInt(100)` result below 40 starts animation states
3 then 1 on `geom28`. Above 2000, the second timer resets; a
bounded result below 30 starts states 3 then 1 on `geom2c`.
The final state is a tail call through the same
`Transform::SetAnimationState` veneer audited for race 1. These
offsets, calls, thresholds and argument registers were checked in
the original ARM instructions.

The prior C++ discarded the enemy-array results, used transform ID
zero, called unbounded `nextInt()`, passed the low frame-delta byte
as the update flag, and omitted the final state-1 call. Extra
`getEnemies()` calls in its return path were stack-canary artifacts.
The recovered branch now follows the observed calls and values.
An extracted host `-O0/-O2` smoke test checks both random thresholds,
all three transform routes, optional `geom28`, high-bit frame delta,
both timers and paired animation states. It does not exercise the
surrounding race dispatch.

Focused ARM `process` source-shape/strict fuzzy rises from
**90.7%/56.1%** (`703/706` rebuilt/original instructions) to
**92.1%/59.7%** (`688/706`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the preceding mode-4 race-1/common smokes pass. Full-corpus
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race3_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_process_mode4_race1_full_20261017.json`.

The host test does not prove live ARM32 animation or GPU output.
The remaining mode-4 race-2 path still has placeholder transform IDs
and needs its own ARM audit.
