# Level::createPlayer yaw axis and empty gun buckets — 2026-10-20

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
stores the calculated yaw at stack `+0x44` at Thumb `0xaf2e8`, between
two zero vector components at `+0x40/+0x48`, before the geometry rotation
call at `0xaf2ec`. The angle comes from `Level+0x138` and two floating
multiplications at `0xaf2dc/0xaf2e4`. The neighboring `createSpace`
rotation uses the same Y axis. The recovered `createPlayer` had placed the
angle in Z; its `Vector` now places it in Y.

`tools/run_level_player_rotation_pan_gate.py` continues the original-asset
Pan `Level::init → createPlayer → setShip → setLevel` chain through the
production `AEGeometry::setRotation` and the three empty gun-bucket
allocations. On Windows SwiftShader and an Android ARMv7 device, `-O0/-O2`
pass. The test checks the signed `±1600` starting angle, Y-only rotation,
the transform matrix against independent sine/cosine values, three null
gun buckets, five original Midorian AEM loads, and **37/37** closed file
operations. The production matrix and geometry methods run without a
rotation mock.

Focused ARM `Level::createPlayer` source-shape rises **86.0% → 86.1%**;
strict fuzzy falls **35.5% → 35.2%**. It is still non-exact. This is a
source-backed behavior correction, not an exact-match claim. The full
ARM corpus compares **4,323/4,524** original functions in **4,735** pairs
at **93.95%/83.24%** mean source-shape/strict fuzzy, with **2,551
linked-exact** and **1,079 byte-exact** matches. There are zero skipped
units, and neither exact `(unit, symbol)` set changed. The report is
`createplayer-rotation-full.json` (SHA-256
`9e9bbe7f8ed74bc81dc5b0889ddcf93cdebdb8469f9c4e7c880b8c786dbdac5f`).

The current fixture has zero used gun slots. Nonempty buckets, equipment,
weapon-position file parsing, the return from `Level::createPlayer`, and
the return from `Level::init` are not yet established. The particle-manager
and material boundaries inherited from earlier gates remain test services.
