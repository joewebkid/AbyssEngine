# ParticleSystemMesh color and expiry output, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `updateSingleColor` at `0x1863d0` selects the masked
`color1` when the preceding trail slot is inactive, otherwise calls
`IParticleSystem::interpolateColor`. It writes the current section's
front vertices and, when the next slot is active, the next section's
back vertices. The original `updateSingle` at `0x186968` colors before
testing expiry; its branch at `0x186984` leaves a particle alive at
`age == lifetime`, while `age > lifetime` marks it inactive and clears
all its vertices.

The smoke fixture now extracts both production method bodies and
records the canvas calls. Host `-O0` and `-O2` runs cover both
`alphaFade` masks, interpolation dispatch, adjacent-section coloring,
ring wrap, color-before-clear order, the zero coordinates of four
cleared vertices and the equality/greater-than lifetime boundary.
The interpolator is stubbed to known RGBA values: these tests check
dispatch and output placement, **not** its arithmetic. They do not
render on a live GPU or execute the code on ARM32.

No production C++ changed. The smoke test, native `gof2` build and
40 verifier tests (one existing skip) pass. Current-source ARM
compilation remains the previously verified **208/208** translation
units; it was not rerun for this test-only change. A fresh focused
`--no-build` report at
`A:/Galaxy on Fire new version/_work/particle_color_expiry_focused_20261016.json`
gives `updateSingle` **97.2% source-shape / 70.5% strict fuzzy** and
`updateSingleColor` **99.8% / 74.6%**, neither exact. The unchanged
full report at
`A:/Galaxy on Fire new version/_work/particle_trail_edge_sign_full_20261016.json`
covers **4,323/4,524** original functions, **201 missing**, **123
extras**, **93.67%** average source-shape, **82.77%** strict fuzzy,
**2,513 linked-exact** and **1,074 byte-exact**. A test-only change
cannot alter those generated code pairs.

Next: validate base-particle interpolation arithmetic against the
Android implementation, then choose a separate missing or low-shape
function family for production recovery. Do not increase the overall
completion claim from this test alone.
