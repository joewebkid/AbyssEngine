# Pan stage-one mode-zero scene cleanup (2026-10-20)

## Original evidence and scope

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::createScene` at `0xb42ec`. At `0xb4308–0xb431c` it checks the
enemy array at `Level+0xf8`, releases its elements and deletes the array.
At `0xb4320–0xb4336` it reads the mode at `Level+0xc0`, clears the enemy
pointer and dispatches modes 2, 4 and 23; mode zero goes to the return
epilogue at `0xb4b2c`. The calling `Level::createMission` writes mode zero
at `0xb01ce`, calls `createScene` at `0xb01d2`, then restores mode three at
`0xb01d8` before its next `Status::getSystem` call at `0xb01de`.

`tools/run_level_stage1_scene_zero_pan_gate.py` extends the original-asset
Pan gate with a **source-extracted mode-zero slice** of `createScene`.
Unsupported scene modes fail closed. An empty allocated enemy array is
presented before stage one, so the production cleanup code must release it;
the gate checks `enemies == nullptr` and restored `missionPtr == 3` after
the source-extracted `createMission` reaches its next boundary. Inherited
checks still cover the Pan assets, complete asteroid field, gas clouds,
non-null empty mission, and time-derived RNG reset.

Windows SwiftShader and a physical Android ARMv7 device pass at both `-O0`
and `-O2`. This test exercises deletion of an **empty** array, not deletion
of populated enemy objects. The complete production bodies of `createScene`
and `createMission` are not linked into the gate. Their other branches, the
post-scene mission logic, and stage-one return remain untested. The earlier
null-mission concern is unchanged.

## Metrics and next boundary

No production C++ changed. The last canonical NDK r18b full corpus remains
**208/208** ARM translation units, **4,323/4,524** original functions over
**4,735** pairs, **93.97%** mean source-shape, **83.29%** strict fuzzy,
**2,554 linked-exact**, **1,082 byte-exact**, zero skipped and zero
wrong-type. This test-only gate claims no new exact match. Native `gof2`
and the 93-test verifier suite were green on the same production revision
(one verifier skip when temporary files used D:).

The next [hostile-route gate](LEVEL_STAGE1_HOSTILE_ROUTE_ARM_2026-10-20.md)
executes the post-scene system/station/RNG prefix through `Route(int*, 3)`.
Continue through the post-route decisions toward stage-one return. Real
Galaxy probability distributions, nonempty and alien mission modes,
populated enemy cleanup, cloud updates and a playable frame remain open.
