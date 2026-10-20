# Pan stage-one empty-mission prefix (2026-10-20)

## Evidence and executed boundary

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::createMission` at `0xaffd8`. Its entry calls
`Status::getMission` at `0xafffe` and reads the player's geometry position
at `0xb0012`. The alien-orbit test follows at `0xb0018`. On the non-alien,
empty-mission path, `Mission::isEmpty` is called at `0xb01b4`,
`AERandom::reset` at `0xb01c6`, mission mode is set to zero at `0xb01ce`,
and `Level::createScene` is called at `0xb01d2`.

`tools/run_level_stage1_empty_mission_prefix_pan_gate.py` continues the
original-asset Pan gate through production `Level::init` and a **source-
extracted empty-path slice** of `Level::createMission`. The slice retains the
production entry, mission/position reads, RNG reset, mission-pointer write,
and `createScene` call. It replaces the alien and nonempty mission branches
with fail-closed test boundaries. The `createScene` stub stops the test at
entry. The gate checks the same non-null empty mission, `missionPtr == 0`,
and the actual post-reset RNG seed against the wall-clock interval enclosing
stage one. Existing asteroid, gas-cloud, original-asset, and synthetic
probability/equipment assertions remain active.

Windows SwiftShader and physical Android ARMv7 pass at both `-O0` and `-O2`.
The complete production `createMission` body is **not** linked or executed:
its other branches depend on many additional gameplay services. A trial
full-body harness failed to link on those dependencies, and broad substitute
traps were rejected because they would overstate integration. Nor is
`createScene` executed, `createMission` returned, or stage one completed.
The null-mission concern from the preceding dispatch check remains open.

## Metrics and next boundary

Production C++ is unchanged. The last canonical NDK r18b full corpus is
still **208/208** ARM translation units, **4,323/4,524** original functions
across **4,735** pairs, **93.97%** mean source-shape, **83.29%** strict
fuzzy, **2,554 linked-exact**, **1,082 byte-exact**, zero skipped and zero
wrong-type. No exact-match gain or loss is claimed from this test-only gate.
Native `gof2` and the 93-test verifier suite were green on the same
production revision (one verifier skip with temporary files on D:).

The next [mode-zero scene gate](LEVEL_STAGE1_SCENE_ZERO_ARM_2026-10-20.md)
exercises an empty-array cleanup and returns to the restored mission mode.
Continue through the remainder of `createMission` and stage-one return.
Nonempty and alien mission paths, real Galaxy probability distributions,
cloud updates, and a playable frame remain untested.
