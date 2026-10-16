# Sprite particle spawn size argument, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ParticleSystemSprite::setParticle` is at
`0x182c7c`–`0x182d6c`. Under the ARM soft-float calling convention,
`r2` is the second explicit argument (`p2` in the current source),
while `p9` is a later stack argument. The original saves `r2` at
`0x182c88`, converts it to an integer at `0x182cb0`–`0x182cc4`,
and passes it to `SpriteSystemSetSize` at `0x182cc8`. It does not
load `p9` for size. The recovered C++ instead ignored `p2` and used
`p9`, so callers could create a sprite with the wrong initial size.
The one-line behavioral correction uses `p2` and marks `p9` unused.

The original also calls SetPosition, SetUv and SetRGBA in that order
around SetSize, masks the low color byte when the clear-color flag is
set, and truncates `resourceOffset + currentParticle` to a 16-bit
sprite id. A smoke test extracts the production method and records
those calls. With deliberately different `p2=23.75` and `p9=99.25`,
the test failed on the prior source at `-O0` (size 99 instead of 23)
and passes after the fix at host `-O0/-O2`. It also checks index wrap,
UV arguments, normalized RGBA and the masked-color branch. The
velocity and `p10` arguments are unused by this original callee; the
broader caller argument map remains a separate audit.

Native `gof2`, **208/208** ARM translation units and 40 verifier
tests (one existing skip) pass. Focused ARM reports
`A:/Galaxy on Fire new version/_work/particle_sprite_spawn_before_20261016.json`
and `particle_sprite_spawn_after_20261016.json` show
source-shape/strict fuzzy **90.8%/43.5% → 89.1%/38.4%**; the
method remains nonexact. This metric decline is accepted because
the original register-to-call argument and the behavioral failure
both identify a real error. Do not restore the wrong size source
merely for a higher shape score.

Full ARM report
`A:/Galaxy on Fire new version/_work/particle_sprite_spawn_full_20261016.json`
compares **4,323/4,524** original functions, with **201 missing**,
**123 extras**, zero skipped or wrong signatures, **93.68%** average
source-shape, **82.78%** strict fuzzy, **2,515 linked-exact** and
**1,075 byte-exact**. Both exact `(unit,symbol)` sets have zero
differences against `particle_init_void_full_20261016.json`; no
exact matches were lost to this semantic correction.

Unverified: live ARM32/GPU output, independent iOS/PC argument names,
and whether all base-emitter call sites pass semantically correct
values in the remaining positions. Next priority: audit the shared
`IParticleSystem::emit` to virtual `setParticle` argument mapping
against Android ARM, especially the source names for `p2` and `p9`.
