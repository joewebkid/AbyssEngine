# Explosion globals and first PlayerAsteroid — 2026-10-20

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `Explosion::Explosion(int)` at Thumb `0xa7af4`, the load at `0xa7b8e`
resolves through GOT `0x2101f4` to `_ZN7Globals6CanvasE` (`Globals::Canvas`).
In `Explosion::setScaling(float)`, the load at `0xa7efc` resolves through
GOT `0x2101f8` to `_ZN7Globals3rndE` (`Globals::rnd`). These addresses were
checked against the original ELF relocations, not inferred from C++ names.

The reconstruction instead read `Explosion_paintCanvas` and
`Explosion_random`, local statics with no writer in the source tree. On a
64-bit host the canvas slot was also an `int`, so even an assigned pointer
would be truncated. The shared accessors now read the two original `Globals`
slots. This affects every `Explosion` method using those accessors; the
production `Explosion` constructor/reset/scaling path is exercised below.

Focused `game/mission/Explosion` ARM verification covers 17 `Explosion`
symbols. Mean source-shape improves **90.93% → 92.25%**. The four
linked-exact and three byte-exact symbols in this family remain the same;
the constructor itself is still nonexact (source-shape **87.7% → 88.4%**,
strict fuzzy **61.1% → 59.0%**). These metrics are code-shape evidence,
not proof of all explosion effects.

The complete ARM report `explosion-globals-full.json` (SHA-256
`4e60e6fae9369f893d984612bd969311fd99a196dfbf250c6a830035963b7deb`)
compares **4,323/4,524** original functions in **4,735** pairs. Mean
source-shape improves **93.95% → 93.96%**; strict fuzzy remains **83.24%**.
The **2,551 linked-exact** and **1,079 byte-exact** `(unit, symbol)` sets
have zero losses or gains, and no units were skipped. Native `gof2`, all
**208** ARM translation units and **93** verifier tests pass (one existing
skip).

`tools/run_level_stage1_first_asteroid_pan_gate.py` extends the continuous
original-asset Pan `Level::init` gate past first geometry to execute the
production `PlayerAsteroid` constructor, production `Explosion` constructor,
reset and scaling, insertion into the asteroid array, `setLevel`, and
`setAsteroidCenter`. Windows SwiftShader and an Android ARMv7 Adreno 618
device pass at `-O0/-O2`. Checks cover the first asteroid's position,
geometry, kind, scale, quality, `Player` back-reference, health/radius,
minable flag, and both explosion meshes (`0x4213`, `0x4211`). A new material
ID `20051` is allowed only as an existing kind-6 resource.

The Galaxy probability provider is a bounded fixture returning `[0x9a,
100]`. Non-executed `PlayerAsteroid` virtual methods are fail-closed traps,
not claimed as validated implementations. The gate throws after the first
object; remaining asteroids, probability generation, stage-one return,
collision/update/render behavior, other mission/alien/iPad paths, and a
playable frame remain unverified. Next: replace the relevant virtual traps
with production methods as the gate proceeds through additional asteroids,
then reach field completion and `createGasClouds`.
