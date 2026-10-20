# Pan stage-one empty-mission dispatch boundary (2026-10-20)

## Source and scope

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::init` at `0xadf94`. Its instructions call `createGasClouds` at
`0xae2ac`, then `Status::getMission` at `0xae2bc`. With mode `3` and an
existing empty mission, the `Mission::isEmpty` branch at `0xae2de` reaches
`Level::createMission` at `0xae314`. This is an instruction-backed control-flow
claim, not a claim that `createMission` has run successfully.

The new `tools/run_level_stage1_mission_dispatch_pan_gate.py` continues the
original-asset Pan `Level::init` gate through production asteroid and gas-cloud
creation and the mission selection code. The fixture checks the production
`Mission` constructor, `isEmpty` and `isCampaignMission`; `Level::init`
executes `Status::getMission` and `Mission::isEmpty` before dispatch.
`Status::setMission` is linked but its null-mission branch is not taken. A
non-null empty `Mission` is supplied before stage one. The
only new boundary is at entry to `Level::createMission`; that body is not
executed. Existing bounded asteroid/plasma probabilities and the synthetic
plasma equipment item remain in the gate. Windows SwiftShader and the physical
Android ARMv7 device pass at both `-O0` and `-O2`; inherited checks include
the Pan assets, complete asteroid field, and gas-cloud branches.

The original null-mission branch is **not** covered. At `0xae2c2–0xae2d0`,
ARM calls `setMission(Mission::empty)` when the saved mission pointer is null,
but `0xae2dc–0xae2de` then passes that same saved pointer to `isEmpty`.
`Mission::isEmpty` begins with `ldr r0, [r0, #12]` at `0x15baa8`; a null
pointer therefore appears unsafe. This conclusion is instruction-derived,
not a reproduced original-device crash. The reconstructed C++ reloads the
status mission for the first check and may behave differently; do not use this
gate to certify null-mission behavior.

## Metrics and next boundary

Production C++ is unchanged. The last canonical NDK r18b full report remains
applicable: **208/208** ARM translation units, **4,323/4,524** original
functions across **4,735** pairs, **93.97%** mean source-shape, **83.29%**
strict fuzzy, **2,554 linked-exact**, **1,082 byte-exact**, zero skipped and
zero wrong-type. The full 93-test verifier suite passes with one skip when
temporary files use D:. This test-only extension creates no new machine-code
match and loses none. Native `gof2` was green on the same production revision.

Next, recover and execute a bounded `Level::createMission` branch using actual
mission data, then continue through static objects, player connection and the
stage-one return. Real Galaxy probability distributions, other mission modes,
cloud updates and a playable frame remain outside this gate.
