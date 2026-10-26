# PaintCanvas camera and font calls in the AArch64 link path (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump identifies `PaintCanvas::CameraSetCurrent` at `0x7b7ec`,
the three-float `CameraSetPerspective` overload at `0x7b99e`, the two-float
overload at `0x7ba1e`, `FontSetSpacing` at `0x79c18`, and `FontGetSpacing`
at `0x79c30`. The camera calls pass the display width and height to the
existing `CameraSetPerspective` functions, then re-apply the current camera
when the changed index is active. Font spacing delegates to the existing
`ImageFontSetSpacing` and `ImageFontGetSpacing` functions.

`PaintCanvas.cpp` now calls those typed functions directly. The AArch64
`CameraSetCurrent` path calls the existing `SetProjectionMatrix3d` method
with the selected camera's three stored parameters. The ARM32 path keeps
the original raw-word call form: a direct typed call compiled to the same
nine normalized instructions but lost one pre-existing linked-exact result,
so that form was rejected for ARM32. The retained form preserves the
comparison corpus while the typed 64-bit path closes the runtime call.

Before and after focused ARM verification compare **198** PaintCanvas
symbols. Both retain **96.2% mean source-shape / 83.1% strict fuzzy**,
**94 linked-exact / 23 byte-exact**, with zero exact-set gains or losses.
The full ARM corpus compares **4,323/4,524** original functions over
**4,745** symbol/unit pairs, with **94.79% source-shape / 84.29% strict
fuzzy**, **2,607 linked-exact / 1,083 byte-exact**, and zero skips.
The linked-exact and byte-exact symbol sets have zero gains and losses
against the preceding full report.

Native `gof2` and AArch64 `gof2` static-library builds pass. The strict
JNI-initialize AArch64 link diagnostic drops from **107 to 100** unique
unresolved names; the broader JNI-onload diagnostic drops from **325 to
317**, with no new names in either. These are link-closure inventories,
not a runnable game `.so` or APK. Live projection changes, font rendering,
and the 64-bit camera parameter layout are still unverified on Android.
