# PaintCanvas perspective reset (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers `PaintCanvas::ResetPersMatrix` at `0x7d0b4`.

The original reads `i_fov`, `i_zFar`, and `i_zNear` through relocated
global data slots, calls `AEMath::Sinf/Cosf`, and converts display width
and height from signed integer to float in VFP. The reconstruction used
zero-filled local stand-ins and undefined math/conversion helpers. The
real globals and calls now replace those stand-ins.

At `0x7d1b2`, the original writes `0xbf800000` (`-1.0f`) into matrix
element 11; the reconstruction wrote zero. Its depth divisor is the
float subtraction `near - far`. It computes matrix element 10 in
float and element 14 in double from `double(far) + double(far)`,
`double(near)`, and `double(near - far)`, then converts the result to
float. The restored source follows these operations.

The global-data relocation at `0x20fe08` identifies
`Engine::enableShader`. When enabled, the original tail veneer
`0x19b3c8` calls `Engine::SetPerspMatrix`; otherwise, it sets up the
OpenGL texture and projection matrices, scales Y by `-1.0f`, and tail
veneer `0x19b3d8` calls `glLoadIdentity`. The reconstruction had an
undefined `glFinish` stand-in for that final call. These targets are
now restored.

Focused ARMv7 verification compiles **211/211** translation units.
Target source-shape rises **76.0% -> 98.7%** and strict fuzzy
**50.9% -> 75.6%**; reconstructed size is 124 instructions versus
122 original. The function remains non-exact. Windows native and
ARM64 static builds pass, as does strict ARM64 shared linking. The
raw 211-object ARM32 unmatched-symbol inventory falls **213 -> 205**:
eight undefined perspective-reset helpers disappear, with no new
unmatched names.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape stays **96.82%**, strict fuzzy
**86.89%**, **2,798 linked-exact**, **1,113 byte-exact**, 201 missing,
zero skipped, zero wrong-type, and 149 extra symbols. Exact-match
membership sets are unchanged. These rounded means do not show the
large improvement in this one function.

Live perspective-reset rendering and full ARM32 shared linking remain
unverified. Next priority: compare `SetProjectionMatrix3d` and the
projection consumers against original ARM and, when a device is
available, check orientation and depth visually.
