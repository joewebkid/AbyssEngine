# CutScene mode-4 race-1 process path (2026-10-17)

The Android ARM race-1 path in `CutScene::process(int)` at
`0x98faa..0x990aa` first reads the enemy array twice: the first
result supplies its data pointer and the second supplies its count.
It selects element `count - 3`, then updates that player's geometry
transform (`KIPlayer+0x08`, `AEGeometry+0x0c`) using the sign-extended
frame delta and `false`.

If `CutScene+0x30` (`geom30`) is present, its primary transform is
updated; the `+0x7c` timer triggers above 20000 and a bounded
`AERandom::nextInt(100)` trial below 100 starts animation states
3 then 1 on that transform. If `CutScene+0x34` (`geom34`) is present,
the corresponding sequence uses the `+0x80` timer and threshold
22000. Its second animation state tail-branches through the
interworking veneer at `0x19b9c8` to the
`Transform::SetAnimationState` PLT entry at `0x5fd18`.
The transform IDs, timer comparisons, random bounds and signed/false
update arguments were checked directly in ARM instructions.

The prior C++ discarded both enemy-array results, used transform ID
zero, unbounded `nextInt()`, and omitted the final animation call.
It also emitted extra `getEnemies()` calls where the ARM code only
checks the stack canary and returns. The recovered branch now follows
the observed data flow and calls. An extracted host `-O0/-O2` test
checks the `count - 3` selection, all three transform updates,
animation states, timers and a high-bit frame delta with optional
geometries absent. It does not exercise the surrounding race dispatch.

Focused ARM `process` source-shape/strict fuzzy rises from
**90.2%/53.2%** (`680/706` rebuilt/original instructions) to
**90.7%/56.1%** (`703/706`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the preceding mode-4 and mode-23 smokes pass. Full-corpus
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race1_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_process_mode4_common_full_20261017.json`.

The host test does not establish live ARM32 animation or GPU output.
Race-2 and race-3 mode-4 paths still have placeholder transform IDs
and need separate ARM audits.
