# ARM32 vibration and frustum call ABI (2026-11-05)

The original Android ARMv7 library is
`android_2.0.16_libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
At `0x822bc`, `ApplicationManager::VibrateSupported` loads its engine
from offset `0xa8` and tail-branches through the Thumb/ARM veneer at
`0x19b318`. The veneer reaches PLT `0x5fdcc`, which names
`Engine::HasVibration`. The previous reconstruction declared a void
method and called an undefined `Engine::VibrateSupported`. The
recovered method returns the boolean from `HasVibration` and removes
the nonexistent engine declaration.

At `0x77538..0x7758c`, `Transform::InCameraVF` sets `r2` to zero,
puts the camera pointer in `r3`, and calls the four-argument
`CameraIsSphereinViewFrustum(const Vector&, float, Matrix*, Camera*)`
through PLT `0x5f874`. The previous ARM32-only call used a
three-argument declaration absent from the original library. It now
passes `nullptr` for the matrix, as the ARM instructions do.

The ARM32 object inventory was formed from **209** compiled objects:
undefined symbols across those objects minus their global definitions.
Its count falls **267 -> 265**, with exactly the two incorrect method
names above removed and no new names. This inventory is a candidate
list, not a shared-link failure count: it includes system, OpenGL,
FMOD and other external dependencies, plus code that a linker may
discard. In particular, the original exports `getAppVersion()` and
`Engine::InitGL(bool,int,int)` at the same address (`0x6db20`), while
the reconstruction emits separate functions; the three
`AE_getInitGL*` calls in its nullary `getAppVersion` body should be
investigated as a symbol-alias/ABI issue before implementing them.

Windows native, ARM64 static, ARMv7 **209/209**, and strict ARM64
shared linking pass. Focused `VibrateSupported` remains **90.0%**
source-shape and **50.0%** strict fuzzy because its two-instruction
shape is unchanged, although its call target is corrected.
`Transform::InCameraVF` source-shape changes **92.8% -> 93.2%** and
strict fuzzy **48.0% -> 47.8%**; neither function is exact. Device
behavior is unverified. Full ARMv7 corpus and exact membership:
**4,323/4,524** originals compared in **4,770** pairs;
**96.79%** mean source-shape, **86.79%** mean strict fuzzy,
**2,785 linked-exact**, **1,108 byte-exact**, 201 missing, zero
skipped and zero wrong-type functions. The complete linked-exact and
byte-exact member sets have no losses against the preceding full
report.
