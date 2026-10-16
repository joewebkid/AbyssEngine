# CutScene mode-23 process animation (2026-10-17)

The original Android ARM `CutScene::process(int)` kind-2 branch at
`0x99424..0x99508` iterates the `CutScene+0x38` geometry array. For
each geometry it passes the `AEGeometry+0x0c` primary transform ID
to the global canvas and calls `Transform::Update` with sign-extended
`CutScene+0x58` frame delta and `false`.

When `fogTimer84` exceeds 3000, the branch resets the timer and
iterates again. Each trial calls `AERandom::nextInt(100)` and proceeds
for results below 20. It checks `IsRunning()` on the geometry's
primary transform. If not running, it sets animation states 3 then 1
on the primary transform (`+0x0c`), then states 3 then 1 on the child
transform (`+0x14`). The bound, comparison, handle offsets, call
sequence and signed/boolean update arguments were confirmed directly
in the ARM instructions.

The recovered source previously used transform ID zero, converted
integer IDs into `PaintCanvas*`, used unbounded `nextInt()` and passed
the low frame-delta byte as the boolean update flag. Those paths are
now replaced with the observed canvas calls and arguments. An
extracted host `-O0/-O2` smoke test checks both transform updates,
the high-bit frame delta, the timer gate, a successful animation,
an already-running transform and a failed random trial.

Focused ARM `process` source-shape/strict fuzzy rises from
**89.7%/49.3% to 90.1%/50.8%**, still nonexact at **669/706**
rebuilt/original instructions. Native `gof2`, **208/208** ARM units,
40 verifier tests (one existing skip), this smoke and the preceding
mode-23 transform/fog smokes pass. Full-corpus results are recorded
in `A:/Galaxy on Fire new version/_work/cutscene_process_mode23_anim_full_20261017.json`:
**4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded source-shape/strict fuzzy averages,
**2,517 linked-exact** and **1,076 byte-exact**. Exact
`(unit,symbol)` sets are unchanged from
`cutscene_process_fog_full_20261017.json`.

The host test does not establish live ARM32 animation or GPU output.
Mode-4 enemy and race-specific transform updates still contain ID-zero
placeholders and need a separate ARM audit.
