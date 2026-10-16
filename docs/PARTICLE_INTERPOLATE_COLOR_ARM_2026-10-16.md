# Particle color interpolation, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`IParticleSystem::interpolateColor` is at `0x181c1c`–`0x181d54`.
The original loads age and signed set index from the particle arrays,
uses a `0xa0`-byte set stride, reads lifetime at `+0x28`, colors at
`+0x34/+0x38`, and fade frames at `+0x3c`. The `vmin.f32` at
`0x181c72` caps the age fraction at one; the fade branch at
`0x181d06` compares age with fade frames. When `alphaFade` at
`IParticleSystem+0x45` is set, it scales alpha/red/green; otherwise
it scales blue. The source matches these observed choices. There is
no evidenced production correction in this pass.

The existing mesh-trail fixture now extracts the production
`IParticleSystem::interpolateColor` body instead of providing fixed
RGBA values. Its set definition locks the observed field offsets and
`0xa0` stride. Host `-O0/-O2` tests cover a half-lifetime blend,
clamping past lifetime, both fade modes, a second set, and propagation
of computed RGBA values to current and adjacent/wrapped trail vertices.
`ParticleSystemSprite` also calls this shared method, although sprite
render output is not directly tested here. Float comparisons use a
small tolerance and do not claim instruction-identical arithmetic.

The extracted-body smoke test, native `gof2` and 40 verifier tests
(one existing skip) pass. No production C++ changed. Fresh focused
ARM report
`A:/Galaxy on Fire new version/_work/particle_interpolation_baseline_20261016.json`
gives **91.4% source-shape / 42.8% strict fuzzy** for this method;
it is neither linked-exact nor byte-exact. Fresh full `--no-build`
report
`A:/Galaxy on Fire new version/_work/particle_interpolation_full_20261016.json`
compares **4,323/4,524** original functions, with **201 missing**,
**123 extras**, zero skipped, **93.67%** average source-shape,
**82.77%** strict fuzzy, **2,513 linked-exact** and **1,074
byte-exact**. Both exact `(unit,symbol)` sets have zero differences
against `particle_trail_edge_sign_full_20261016.json`. Current-source
ARM compilation remains the previously verified **208/208** units;
it was not rerun for this test-only change.

Unverified: live ARM32/GPU color output, sprite canvas propagation,
and unusual inputs such as zero/negative lifetime or NaN. Next
priority: audit the separate `ParticleSystemSprite::setAlpha` output
path (currently 80.6% source-shape) against ARM, then address its
neighboring `init` recovery (57.0%). Do not tune this nonexact
interpolator solely for compiler shape.
