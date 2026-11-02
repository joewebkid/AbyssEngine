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

Later the corrected APK was observed in the foreground on an awake,
unlocked Redmi. The screenshot
`D:/gof2-arm64-crypto/redmi-image2d-awake-20261102.png` was entirely
black. Android recorded an input-dispatch ANR at 17:17:50 and stopped
the process after user interaction. Its `exit-info` also records a
similar ANR at 16:26:37, before this title-image correction; this is
not evidence that the correction introduced the stall. Android denied
access to the ANR stack trace without root. A separate signed APK with
bounded `GOF2FRAME` logging was prepared outside Git, but no fresh
foreground run of it has been made yet. The current source tree contains
none of that diagnostic logging.

The Poco X3 original OBB was copied to the separate clone package's OBB
directory without modifying the original. Play Protect blocked the test
APK installation there pending manual confirmation. The cloned OBB uses
about 1.01 GB of the phone's storage and can be removed after testing.
The next priority is a fresh unlocked run of the diagnostic APK to locate
the first frame stall before changing rendering code again.

## Title 3D pass and awake Redmi frame

The original Android ARM `MTitle::OnRender3D` at `0x97bc4` calls
`ClearBuffer(0xff)`, `Begin3d()` and `End3d()`. The recovered source
omitted the last call; it is now restored. Focused ARMv7 comparison
compiled 209/209 units and changed this function from **76.2% to 93.3%**
source-shape and **71.4% to 73.3%** strict fuzzy. It remains nonexact.
The full corpus (`_work/mtitle_end3d_full_20261102.json`) compares
**4,323/4,524** originals in **4,762** pairs at **96.21%** mean
source-shape and **85.96%** strict fuzzy, with **2,741 linked-exact**,
**1,096 byte-exact**, 201 missing and zero skipped. Against
`_work/image2d_title_full_20261102.json`, the comparison pair set and
both exact sets have zero gains and zero losses. Windows native and
ARM64 static builds pass.

On an awake, unlocked Redmi, a temporary diagnostic APK with this
change reached `MTitle::OnRender3D` and `OnRender2D` repeatedly;
`End3d()` returned on every sampled frame. The five-argument image
draw received a 378×431 title region, four vertices and six indices.
The OpenGL program was 3, framebuffer 0, and the sampled center pixel
after the draw was black (`0,0,0,255`) with `glGetError()==0`.
The image's `textureId` is an engine handle; `Engine::SetTextures`
resolves it to the bound OpenGL texture, so differing numeric IDs are
expected. No visible logo or menu was verified. A follow-up build with
matrix and color probes was prepared outside Git but could not be
installed after Redmi disconnected from ADB. All temporary probes were
removed from the source. Next: inspect that matrix and color state on
device, then texture content and shader uniforms if the projected
geometry is valid.
