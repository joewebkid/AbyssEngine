# PaintCanvas draw dispatch and AArch64 link closure (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump shows `PaintCanvas::SetColor(unsigned)` at `0x78460`
and `SetColor(bytes)` at `0x7c938` delegating to `Engine::SetColor`;
`SpriteSystemSetAllSize` at `0x7a7a0` delegates to the sprite helper;
`End2d` at `0x7e378` calls `PaintCanvas::SetWorldViewMatrix` followed by
`MeshDraw` on the quad mesh. `Begin2d` sets the engine color to white.

The recovered `PaintCanvas.cpp` now calls those existing typed methods
instead of four unresolved `paintcanvas_ext_*` helpers. The color
conversion, current sprite selection, 2D quad count, identity matrix,
and draw order are unchanged. No alternative forms were accepted in
this package; the direct calls retained the focused ARM comparison.

Before and after focused ARM verification compare **198** PaintCanvas
symbols. Both retain **96.2% mean source-shape / 83.1% strict fuzzy**,
**94 linked-exact / 23 byte-exact**, with zero exact-set gains or losses.
The full ARM corpus compares **4,323/4,524** original functions over
**4,745** symbol/unit pairs, with **94.79% source-shape / 84.29% strict
fuzzy**, **2,607 linked-exact / 1,083 byte-exact**, and zero skips.
The linked-exact and byte-exact symbol sets have zero gains and losses
against the preceding full report.

Native `gof2` and AArch64 `gof2` static-library builds pass. The strict
JNI-initialize AArch64 link diagnostic drops from **100 to 96** unique
unresolved names; the broader JNI-onload diagnostic drops from **317 to
313**, with no new names in either. A runnable game `.so` or APK has
still not been produced. Live 2D drawing, sprite sizing, and color
output on Android remain unverified.
