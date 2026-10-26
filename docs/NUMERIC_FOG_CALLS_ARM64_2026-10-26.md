# Numeric conversion and fog calls (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`CutScene::process` at `0x98e8c` converts the signed frame delta to
`float`; its negative form negates the 32-bit delta before conversion.
`PaintCanvas::FogSetParameter` at `0x7cf28` calls `glFogf` and `glFogfv`
on the fixed-function path. The original `glFogf` at `0x6dacc` and
`glFogfv` at `0x6dace` are empty compatibility functions, matching
the existing `GLES1Compat.cpp` implementations.

The recovered code now uses those conversions and calls directly rather
than four undefined stand-ins. An unused conversion declaration in
`HangarWindow` was removed; it had no call sites. Signed negation in
`CutScene` retains 32-bit wraparound before conversion.

Native and AArch64 static archives build. Focused ARMv7 verification
raises `CutScene::process` from **94.0% to 95.0% source-shape**;
strict fuzzy changes **63.5% to 63.0%**. It raises
`PaintCanvas::FogSetParameter` from **92.0% to 100.0% source-shape**
and **96.2% to 100.0% strict fuzzy**. The CutScene and PaintCanvas
focused units retain all their linked-exact and byte-exact symbols.

The strict JNI-initialize AArch64 link probe falls **124 → 120**
unresolved names. The broader `JNI_OnLoad` probe falls **341 → 338**;
no new undefined names appear in either probe. The removed names are
`VectorSignedToFloat` and the three `paintcanvas_ext_fsp_*` stand-ins;
the broader probe did not reach the CutScene conversion. These are link
diagnostics, not a game `.so`. Live fog behavior and Android gameplay
remain unverified.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,745** pairs with zero skips. Mean source-shape rises **94.78% →
94.79%**; strict fuzzy remains **84.29%** after rounding. Strict
fuzzy-perfect pairs rise **2,777 → 2,778**. The full
**2,607 linked-exact** and **1,083 byte-exact** symbol sets have no
losses or gains against the previous report.
