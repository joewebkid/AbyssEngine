# Title image drawing: Android ARM and ARM64 runtime

The original Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
shows `PaintCanvas::DrawImage2D(uint,int,int,uchar,uchar)` at `0x78dd0`.
Its first byte controls screen placement; its second byte aligns the image
region. The width and height are read from the image mesh's position buffer
at offsets `0x0c` and `0x1c`. Right and bottom placement negate the
corresponding input coordinate. `MTitle::OnRender2D` at `0x97a8c` calls this
overload with `0x44, 0x44`, then calls `End2d` and `SwapBuffer`.

The recovered five-argument `DrawImage2D` had the two byte roles reversed,
read dimensions by treating the 32-bit texture ID as a pointer, and mixed
X with Y in placement. Those operations now follow the original ARM data
flow. `MTitle` supplies both original alignment bytes and the final
`SwapBuffer` call. The unused `regionPtr` overlay name was removed.

Before the source correction, Redmi ARM64 logs confirmed that title
resources `7000` and `7001` successfully opened
`data/textures/gof2_logos_iphone4.aei`, loaded texture handle `3`, and
created image indices `57` and `58`. During the subsequent diagnostic run,
the phone was dozing behind its lock screen: only the initialization
`OnUpdate` ran. Consequently there is **no verified rendered title frame**
from the corrected APK. The normal signed clone APK is outside Git at
`D:/gof2-arm64-crypto/gof2-arm64-image2d-redmi-debug-20261102.apk`
(SHA-256 `6355829F83882C781E0A2A93356E1F88F513B082758A15287DFB227D97A3B733`).
It is installed separately from the original game on Redmi. Temporary
`GOF2IMAGE` instrumentation was removed from the source.

Windows native and ARM64 static builds pass. Focused ARMv7 verification
compiled **209/209** translation units. `DrawImage2D` changed from
**88.2% to 89.3%** source-shape and **39.4% to 34.1%** strict fuzzy;
`MTitle::OnRender2D` changed from **80.3% to 83.5%** source-shape and
**49.3% to 50.0%** strict fuzzy. Neither is exact. The lower strict fuzzy
score for `DrawImage2D` was accepted because the original's pointer and
coordinate flow and the ARM64 ABI are now represented correctly.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,762** pairs with **209/209** units, **96.20%** mean source-shape,
**85.96%** strict fuzzy, **2,741 linked-exact**, **1,096 byte-exact**,
201 missing and zero skipped. Compared with
`_work/redmi_runtime_full_20261102.json`, the pair set and both complete
exact-match sets have zero gains and zero losses. The new report is
`_work/image2d_title_full_20261102.json`.

Next: run the corrected APK with an unlocked, awake screen; capture a
title/menu screenshot and OpenGL errors. If it is still black, inspect
texture upload, shader selection and the 2D draw call on device.
