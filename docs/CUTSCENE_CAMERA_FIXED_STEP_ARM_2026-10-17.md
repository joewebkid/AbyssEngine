# CutScene fixed camera update step (2026-10-17)

In Android ARM `CutScene::process(int)`, the optional follow camera is
loaded from `CutScene+0x68`. If present, instructions at `0x98eea`
through `0x98eee` pass the literal `30` in `r1` to
`TargetFollowCamera::update(int)`. The source instead passed the
variable frame delta from `CutScene+0x58`. That argument controls the
camera's active-path damping and motion, so the difference changes
behavior as well as instruction shape. The caller now passes `30`;
the time accumulator and the separate `Level` update path are
unchanged.

Focused ARM verification moves `process` from **87.5% source-shape /
42.1% strict fuzzy** to **87.6% / 43.6%**. It remains nonexact at
`644/706` rebuilt/original instructions. Native `gof2`, **208/208**
ARM translation units, 40 verifier tests (one existing skip), and the
previously added host 64-bit time smoke pass. Full report
`A:/Galaxy on Fire new version/_work/cutscene_camera_step_full_20261016.json`
compares **4,323/4,524** originals, with **201 missing** and
**123 extras**. The average source-shape/strict fuzzy is **93.73% /
82.85%**; **2,517 linked-exact** and **1,076 byte-exact** pairs are
unchanged against `cutscene_time64_full_20261016.json`, with zero
pair losses.

Live ARM32 camera behavior is untested, and the large `process` body
still has other nonexact branches. Next priority is the bounded
mode-2 camera-matrix path: the current source passes an integer camera
ID as a `Matrix` pointer, whereas Android ARM copies and rotates the
camera's local matrix before `CameraSetLocal`. That path requires its
own call/ownership check before editing.
