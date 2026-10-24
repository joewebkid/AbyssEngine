# Pan stage-one first ordinary friend dispatch (2026-10-24)

## Source and boundary

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::createMission` at `0xaffd8`. After ordinary array allocation at
`0xb2a8c`, its no-Wanted branch calls
`Globals::getRandomEnemyFighter(race)` at `0xb2b0c`, obtains the ambient
route waypoint at `0xb2b14`, calls `Level::createShip(race, 0, type,
waypoint, true, false)` at `0xb2b26`, and stores the returned pointer into
the enemy array at `0xb2b32`. The production random selector is independently
anchored at `0xe49dc`, including the Android 37-entry race table at
`0x1fe2a0`.

`tools/run_level_stage1_first_friend_dispatch_pan_gate.py` extends the
original-asset Pan chain to the **first call** to `createShip`. It uses the
source-extracted ordinary friend call body, production
`Globals::getRandomEnemyFighter` and production `Route::getWaypoint`.
`createShip` is a recording, throwing boundary: the real method and
`PlayerFighter` constructor do **not** execute, and the array slot is not
filled. The fixture fails closed for a Wanted target and confirms Pan's
ordinary race is zero. An independent replay of the documented race table
and bounded RNG draws checks the first selected type, call arguments,
waypoint identity and final RNG state. The replay shares the reconstructed
`AERandom` implementation, so it is not an independent proof of that
algorithm.

## Validation and next step

Both `-O0` and `-O2` Windows SwiftShader executions with the original Pan
assets returned zero and printed the first-friend dispatch checkpoint. The
inherited chain exercised the ETC1 sun upload, asteroid field, mission count
overrides and ordinary array allocation. Python syntax checking passed.
Both NDK r18b ARMv7 `-O0/-O2` executables also compile and link using the
runner's ARM math fixture, but the physical device runtime gate is pending
because ADB lists no device. The native `gof2` build passes. The test does
not certify a constructed friend, array insertion,
remaining friends/enemies, stage-one return or a playable frame.

No production C++ changed. The last applicable full ARM corpus remains
**4,323/4,524** originals and **4,736** pairs, **94.09%/83.41%** mean
source-shape/strict fuzzy, **2,557 linked-exact** and **1,082 byte-exact**,
with zero skipped and wrong-type symbols; these were not remeasured for this
test-only addition. Next, execute the real `Level::createShip` →
`PlayerFighter` → `setLevel` path for the first Pan friend and observe the
array write, then continue the ordinary population and stage-one return.
