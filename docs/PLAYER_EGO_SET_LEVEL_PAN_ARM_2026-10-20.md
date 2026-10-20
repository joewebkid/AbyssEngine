# PlayerEgo level binding and particle handles — 2026-10-20

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `PlayerEgo::setLevel` from `Level::createPlayer` at Thumb `0xaf2be`.
The callee at `0x9b0b8–0x9b148` stores `Level*` at `PlayerEgo+0xc`, adds
particle set 9 to `Level+0x74`, and disables emission. If the campaign
mission is at most 1, it additionally adds sets 15 and 42 to managers at
`Level+0x78` and `Level+0x84`, also with emission disabled. The resulting
handles occupy `PlayerEgo+0x2fc/+0x300/+0x304`.

The recovered C++ had written the `Level*` through an `int&`. That is a
pointer-width and aliasing hazard on 64-bit hosts. Direct pointer assignment
now preserves the same ARMv7 instructions: focused source-shape and strict
fuzzy remain **100%**, with **one linked-exact** match for this function.

`tools/run_level_player_particles_pan_gate.py` continues the original-asset
Pan `Level::init → createPlayer → setShip` chain through the production
`setLevel` function. The normal mission-50 path creates set 9 on the
already-constructed manager; a separate call with mission 1 probes all
three particle-set/manager/handle pairs. Windows SwiftShader and an Android
ARMv7 device pass both `-O0` and `-O2`, retaining five original ship AEM
loads, **73** meshes, **78** transforms, and **37/37** closed files.

The particle-manager `addSystem` and `enableSystemEmit` methods are bounded
observer services in this gate. It checks routing, order, handles and the
geometry matrix, not actual particle allocation or rendering. The early
campaign branch is a separate direct call, not the uninterrupted Level path.
The gate stops before hull rotation, weapon positions and the return of
`Level::createPlayer` and `Level::init`.

The complete ARM corpus compares **4,323/4,524** original functions in
**4,735** pairs at **93.95%/83.24%** mean source-shape/strict fuzzy,
with **2,551 linked-exact** and **1,079 byte-exact** `(unit, symbol)`
matches, zero skipped units and no changed exact pairs. The report is
`player-set-level-full.json` (SHA-256
`fcd8ec2b7f2e3ff9c694dcc94e95bc1ee314160cda570539f29d666a8430bd8c`).
