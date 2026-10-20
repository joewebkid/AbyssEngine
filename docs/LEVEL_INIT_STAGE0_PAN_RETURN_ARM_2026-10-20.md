# Pan Level::init stage-zero return — 2026-10-20

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::init` at Thumb `0xadf94`. On the mode-3 path it calls
`Level::createPlayer` at `0xae1fc`. For the ordinary no-stream-out path,
it places the player after that call; at `0xae290–0xae296` it checks the
stage at `Level+0x134`, and at `0xae35c–0xae364` increments that stage
and returns zero. This is the first staged return, not completion of all
Level initialization.

`tools/run_level_stage0_pan_gate.py` removes the test stop after
`createPlayer` and runs the full production stage-zero return. Windows
SwiftShader and Android ARMv7 pass at `-O0/-O2` with original Pan assets:
the player transform is placed at `(10, 10, 10000)`, `Level::init` returns
`0`, `field_134` becomes `1`, and **38/38** original-file operations are
closed. The preceding production `createPlayer` body returns after reading
original `weapons_hd.bin`, creating four engine geometries and cleaning up
temporary vectors. This gate has **73** meshes and **82** transforms.

The continuous fixture has zero used gun slots and non-weapon synthetic
equipment. Its weapon-only services fail closed. A separate full-body CPU
contract still passes 118 weapon cases at `-O0/-O2`, including 1,576
released loader vectors, 472 type-3 geometry pairs and 30 turret cases;
that does not make the armed path continuous with the Level gate. Platform
material creation and particle allocation/rendering remain bounded test
services. Stage one, mission/asteroid creation and a playable native frame
are not verified.

No production C++ changed. The last full ARM corpus for the identical
source tree is `createplayer-rotation-full.json` (SHA-256
`9e9bbe7f8ed74bc81dc5b0889ddcf93cdebdb8469f9c4e7c880b8c786dbdac5f`):
**4,323/4,524** originals in **4,735** comparisons,
**93.95%/83.24%** mean source-shape/strict fuzzy, **2,551 linked-exact**,
**1,079 byte-exact** and zero skipped units. Stage one and the remaining
bounded services are the next integration priorities.

The fresh focused comparison on this source tree measures
`Level::createPlayer` at **86.1%/35.2%** and
`FileRead::loadWeaponPositions` at **97.5%/49.9%**
source-shape/strict fuzzy. Neither function is exact.
