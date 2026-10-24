# Pan large-outpost turret loop — Android ARM and host gate (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` SHA256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`
contains the non-alternate large-outpost turret loop in
`Level::createMission` at `0xb329a–0xb3394`. It calls
`createStaticObject(nullptr, 6772, true)`, sets maximum hitpoints to
**1000**, rotates and positions each turret from a six-float record,
calls `PlayerTurret::setScaling(6.0f)`, clears cargo and assigns the
system race as `shipGroup`. Seven records of 24 bytes begin at
`.rodata` `0x1fce34`. The loop computes its origin through the
outpost's virtual `getPosition`; `PlayerFixedObject::getPosition`
returns the integer-truncated coordinates, not its float position.

`tools/run_level_stage1_large_outpost_turrets_pan_gate.py` extends
the original-asset Pan chain through the production turret loop,
the type-6772 branch of `createStaticObject`, the production
`PlayerTurret` constructor and its position, scaling and level
methods. Windows SwiftShader `-O0/-O2` passes, constructing **seven
distinct** turrets and checking type, mesh **6772/6773**, health,
parent group, rotation, helper scaling and world position. The
turret meshes use material resource **24076**. The host supplies
only a local text label for key 1666; unrelated turret virtual
methods fail closed. The scenario still forces a non-alternate large
outpost after preserving its selection RNG draws. Turret update,
combat, destruction, alternate/small outposts and live Android are
not verified.

Production C++ did not change. Native `gof2` remains green (no-op);
the last full ARM corpus remains **4,323/4,524** originals over
**4,736** pairs, mean **94.19% source-shape / 83.54% strict fuzzy**,
**2,564 linked-exact** and **1,082 byte-exact**. Since this package
adds only a host test and documentation, the exact sets are unchanged.
