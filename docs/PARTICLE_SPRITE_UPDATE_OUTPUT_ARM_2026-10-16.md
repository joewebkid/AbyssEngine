# Sprite particle update output, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ParticleSystemSprite::updateSingle(int,float)` is at
`0x1830b4`–`0x18335c`. Its ARM body branches to
`updateAreaExitParticle` for flag `0x80` at `0x1830da`–`0x1830e0`.
Otherwise it truncates the new age to an integer, keeps
`age == lifetime` alive, and on `age > lifetime` marks the slot `-1`,
sets its position to `4294967296.0f` on all axes, and sets size to
zero (`0x183114`–`0x183168`). The live path calls AddSize,
`interpolateColor`, SetRGBA, optional SetUv after a frame boundary,
and finally AddPosition (`0x18319c`–`0x18332a`). The current source
agrees with this observed order and boundary.

The sprite smoke fixture now extracts the production `updateSingle`
body. Host `-O0/-O2` checks the exact-lifetime and first expired
updates, sentinel position and zero size, size delta and movement,
RGBA dispatch, UV frame transition with rotation dispatch, and the
area-exit branch. The recording canvas checks call order and
arguments. `interpolateColor`, `rotateUVs`, and
`updateAreaExitParticle` are fixture stubs in this test, so their
internal behavior is **not** established here; the shared
interpolator has a separate extracted-body test. No GPU is involved.

No production C++ changed. The fixture, native `gof2` build and 40
verifier tests (one existing skip) pass. Fresh focused ARM report
`A:/Galaxy on Fire new version/_work/particle_sprite_update_output_focused_20261016.json`
is **92.9% source-shape / 49.0% strict fuzzy**, nonexact. Because
production source is unchanged, the last full report
`A:/Galaxy on Fire new version/_work/particle_init_void_full_20261016.json`
remains the generated-code baseline: **4,323/4,524** originals
compared, **201 missing**, **123 extras**, **93.68%** average
source-shape, **82.78%** strict fuzzy, **2,515 linked-exact** and
**1,075 byte-exact**. The prior **208/208** ARM translation-unit
build remains applicable; it was not rerun for this test-only pass.

Unverified: actual ARM32/GPU rendering, area-exit behavior, and
negative/unusual lifetime inputs. Next priority: audit the separate
`ParticleSystemSprite::updateAreaExitParticle` path against its ARM
body and focused output cases, especially its boundary and re-entry
behavior.
