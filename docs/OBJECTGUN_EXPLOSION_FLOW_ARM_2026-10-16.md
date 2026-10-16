# ObjectGun scatter explosion flow (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`ObjectGun::update(int)` at `0x15fc8c-0x15fd3a` first copies
`Gun::delayActive` to `wasFiring`, then enters the explosion loop
only for weapon type `25` (scatter gun). It skips a slot with no
hit flag. A ready hit calls `Explosion::start(hitPosition, zeroVector)`
and clears that slot's ready byte; every flagged hit calls
`Explosion::update(dt, nullptr)`. If `isPlaying()` becomes false,
it clears that hit flag and `Gun::ignited`, sets the ready byte, and
calls `Explosion::reset()`. The loop advances over all gun positions.

`ObjectGun::render()` at `0x15fdc2-0x15fdf0` calls `Gun::render()`
first and, for scatter guns, renders only explosion slots whose hit
flags are set. The current production C++ follows these observed
branches; no source edit was needed.

`tools/run_objectgun_canvas_calls_smoke.py` now extracts these
production fragments and runs them under host `-O0` and `-O2`.
The fixture checks a skipped slot, first start/update, the zero
direction and null camera, no duplicate start, completion/reset,
cleared flags, and hit-gated render calls. The fixture passes;
native `gof2` has no pending rebuild and 40 verifier tests pass
(one existing skip).

Production C++ and ARM objects did not change. Focused ARM results
remain `ObjectGun::update` **87.9%/31.0%** and `render`
**89.6%/39.7%** source-shape/strict fuzzy, both nonexact. The last
full report at the same source revision,
`A:/Galaxy on Fire new version/_work/objectgun_dtor_full_20261016.json`,
compares **4,323/4,524** original functions, with **201 missing**,
**124 extras**, zero skipped or wrong signatures, **93.67%/82.77%**
average source-shape/strict fuzzy, **2,513 linked-exact / 1,074
byte-exact**. The focused audit is
`A:/Galaxy on Fire new version/_work/objectgun_explosion_audit_focused_20261016.json`.

The fixture proves this bounded call/flag sequence, not explosion
timing, effects, audio, full frame rendering, or live ARM32 behavior.
