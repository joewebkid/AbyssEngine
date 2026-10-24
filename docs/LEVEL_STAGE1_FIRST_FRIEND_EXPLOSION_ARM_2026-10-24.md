# Pan first-friend Explosion prerequisite (2026-10-24)

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
maps `Explosion::addFireStreaks` to `0xa811c`. Recovered
`PlayerFighter::PlayerFighter` at `0xdc2f8` calls `Explosion(0)` and then
`addFireStreaks` after its route/loot setup. This is a prerequisite check,
not a claim that the full Fighter constructor executed.

`tools/run_level_stage1_first_friend_explosion_pan_gate.py` extends the
original-asset Pan first-friend Route gate. It source-extracts production
`Explosion::addFireStreaks` and executes the already-extracted production
`Explosion(0)` in the same process after the Player/EMP and Route checks.
The fixture permits material IDs `27252` and `27253`, both kind-6 resources
used by explosion meshes in `BuildResourceList.cpp`. It checks primary mesh
`0x41b5`, absent secondary mesh, fire-streak count and RNG advance, each
streak's `0x37d4` mesh and `10000.0f` transform radius, repeat-call
idempotence, and balanced original-asset file opens/closes.

NDK r18b produced ELF32 ARM executables; both `-O0` and `-O2` passed on an
Android 16 device using original Android OBB assets through the test's
ADB-reverse file boundary. Executable SHA-256: `-O0`
`3428bbe8fa799f0b8c0b056041a9167b7ed6d00cc7c8e6918f1c1837cb811f90`,
`-O2`
`8508e46e24b1921444de34253d2e41cc27dd35064fa6198e122699ed270c93d2`.
Native `gof2` was rechecked and remains green. The two temporary device
executables were removed after the pass.

This fixture calls Explosion separately rather than through a linked
`PlayerFighter` constructor. `Generator::getLootList`, the rest of Fighter
setup, enemy-array insertion, rendering, and Explosion destruction are not
covered; the fixture's allocated Explosion survives until process exit.
Production C++ did not change, so the last full ARM corpus remains
**4,323/4,524** originals, **4,736** pairs, **94.11%/83.43%** mean
source-shape/strict fuzzy, **2,557 linked-exact**, and **1,082 byte-exact**
with zero skipped or wrong-type symbols. The next priority is a linkable
Fighter constructor with real `Generator` dependencies and array insertion.
