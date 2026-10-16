# Sprite particle area exit, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ParticleSystemSprite::updateAreaExitParticle(int,float)` is at
`0x182d6c`–`0x183028`. Its original ARM body integrates velocity,
reads the sprite position and emitter center, compares squared
distance with the outer/middle/inner radii, and routes color through
`setAlpha`. Beyond the outer radius it clears the chosen channel,
then tests against `outer² * 1.01`; only beyond that second threshold
does it consume four `nextInt(2000)` values. A candidate whose
four-component squared length is below one is left outside. Otherwise
the body normalizes the quaternion-derived position to the outer
radius, sets the sprite position and replaces velocity with the
set-definition base velocity (`0x182e56`–`0x182f84`). The recovered
C++ follows these observed branches and formulas.

The sprite smoke fixture now extracts the production area-exit and
`setAlpha` bodies. Its recording canvas checks the movement, position
read, color and optional rebirth order at host `-O0/-O2`. Cases cover
the outer radius equality, near-outside band, far-outside candidate
rejected at squared length zero, far-outside candidate accepted at
length one, middle fade, middle boundary, inner band, inner fade and
inner core. The accepted candidate must set position to the expected
emitter-centered outer-radius point and load all three base velocity
components. A scripted random source controls the four draws; this
does not test the production RNG sequence. Vector/Matrix helpers and
the canvas are fixture implementations, not live ARM32/GPU output.

No production C++ changed. The extracted-body smoke test, native
`gof2` and 40 verifier tests (one existing skip) pass. Fresh focused
ARM report
`A:/Galaxy on Fire new version/_work/particle_sprite_area_exit_focused_20261016.json`
is **94.6% source-shape / 66.7% strict fuzzy**, nonexact. The last
full report
`A:/Galaxy on Fire new version/_work/particle_init_void_full_20261016.json`
remains valid for the unchanged source: **4,323/4,524** originals
compared, **201 missing**, **123 extras**, **93.68%** average
source-shape, **82.78%** strict fuzzy, **2,515 linked-exact** and
**1,075 byte-exact**. The prior **208/208** ARM build still applies;
it was not rerun for this test-only change.

Unverified: live ARM32/GPU rendering, real RNG timing/seed and
unusual radii/NaN inputs. Next priority: audit the separate sprite
`setParticle` spawn path, including parameters currently ignored in
source, against Android ARM and canvas output.
