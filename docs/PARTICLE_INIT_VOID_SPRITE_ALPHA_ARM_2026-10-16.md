# Particle init return ABI and sprite alpha output, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ParticleSystemSprite::init(uint32_t,uint16_t)` at
`0x182c48`–`0x182c58` stores resource, offset and initialized, then
tail-jumps through virtual slot `+8` to `reset`. The mesh overload at
`0x186268`–`0x18639c` has the same terminal tail-jump at
`0x18637c`–`0x186390`. Neither creates an integer return value, and
both `ParticleSystemManager` call sites discard the result. This is
evidence for `void init`, though the C++ return type is not encoded in
these mangled symbols. The base virtual declaration and both overrides
now use `void` and no longer synthesize a zero result.

Fresh ARM compilation succeeds for **208/208** translation units.
Before the change, the sprite init was **57.0% source-shape / 62.5%
strict fuzzy** at 10/6 base/target instructions, and mesh init was
**97.6% / 97.9%** at 118/117. Afterward both are **100% / 100%**:
sprite init is byte-exact and mesh init is linked-exact. Focused
reports:
`A:/Galaxy on Fire new version/_work/particle_sprite_init_alpha_after_20261016.json`
and `particle_mesh_init_after_20261016.json` (before values from
`particle_interpolation_full_20261016.json`).

Full ARM report
`A:/Galaxy on Fire new version/_work/particle_init_void_full_20261016.json`
compares **4,323/4,524** original functions, with **201 missing**,
**123 extras**, zero skipped or wrong signatures, **93.68%** average
source-shape and **82.78%** strict fuzzy. It has **2,515 linked-exact**
and **1,075 byte-exact** pairs. Against
`particle_interpolation_full_20261016.json`, the only exact-set
changes are the two new linked-exact `init` pairs, one of them also
byte-exact; there are zero losses.

The ARM `ParticleSystemSprite::setAlpha` at
`0x183028`–`0x1830b4` loads color channels from low to high bytes,
normalizes by `1/255`, multiplies only blue when `alphaFade == 0`
or alpha/red/green otherwise, and passes RGBA to
`SpriteSystemSetRGBA` at a 16-bit resource-offset index. The current
C++ follows those output semantics; its focused comparison stays
**80.6% source-shape / 46.9% strict fuzzy**, nonexact. An extracted
production-body smoke test records the canvas call and checks both
fade branches, half/zero fade, channel order and 16-bit index wrap at
host `-O0/-O2`. It also checks that sprite init stores its fields and
calls `reset` once. The prior mesh-trail extracted-body test still
passes. Native `gof2` and 40 verifier tests (one existing skip) pass.

Unverified: live ARM32/GPU output and an independent iOS/PC return-type
declaration. The latter is a remaining evidence limit, not a reason
to retain an integer result unsupported by the Android code. Next
priority: inspect the separate sprite `updateSingle` expiry/output
path for behavior gaps; do not contort `setAlpha` solely for compiler
shape.
