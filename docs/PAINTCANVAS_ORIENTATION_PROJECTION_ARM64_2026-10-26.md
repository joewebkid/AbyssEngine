# PaintCanvas orientation and projection recovery (2026-10-26)

The original Android ARMv7 functions `PaintCanvas::Initialize`
(`0x7e6bc`), `SetGameOrientation` (`0x7e3fc`), and
`SetProjectionMatrix3d` (`0x7b808`) call the existing `Engine`
display/orientation methods and `AEMath::Sinf/Cosf`. The local IDA
extraction also identifies orthographic matrix entries `m[10] =
-0.05`, `m[12] = -1`, perspective `m[11] = -1`, the four screen
rotation matrices, and writes to `Engine::{nearPlane,farPlane}` and
`AbyssEngine::{i_fov,i_zNear,i_zFar}`. The independent local Ghidra
extraction confirms the orientation branches and matrix field offsets.

The recovered code now makes those calls and writes directly. It
replaces zero stand-ins in the projection matrices, restores the
missing `-1` rotations and translations for portrait/reverse
orientations, and clears the stale translation when changing to
landscape. The focused ARMv7 `PaintCanvas` verifier compares **198**
symbols; source-shape changes are `Initialize` **84.2% → 97.8%**,
`SetGameOrientation` **78.0% → 91.9%**, and
`SetProjectionMatrix3d` **75.8% → 95.3%**. All three remain nonexact.
The focused exact sets remain **93 linked-exact / 23 byte-exact**.
The full corpus compares **4,323/4,524** original functions over
**4,745** pairs, with zero skips. Mean source-shape rises **94.76% →
94.77%** and strict fuzzy **84.27% → 84.28%**. All **2,606
linked-exact / 1,083 byte-exact** symbols remain in their exact sets.

The native and AArch64 static libraries build. The production method
bodies pass the native orientation smoke test at `-O0` and `-O2` for
all four rotations, the initial portrait/landscape projections, the
perspective globals, and the active-camera refresh. Strict
JNI-reachable AArch64 unresolved names fall **153 → 143**: ten
orientation/init placeholders disappear, with no new names.

The isolated smoke fixture supplies fixed dimensions and mock engine
methods. Live Android surface rotation, GL rendering, camera lifetime,
and device-specific dimensions remain unverified. The shared library
and APK still do not link or run; additional renderer/game calls and
compatible audio remain open.
