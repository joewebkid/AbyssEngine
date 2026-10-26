# PaintCanvas matrix construction on AArch64 (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump's `PaintCanvas` constructor at `0x77674` contains
one `AEMath::Matrix::Matrix` call for the `identityMatrix` member at
offset `0xf8`. The declared `Matrix identityMatrix` member already
causes C++ to call its default constructor before the `PaintCanvas`
constructor body. The extra `paintcanvas_ctor_matrix` call in that
body was a duplicate with no implementation on AArch64. It is now
removed, leaving the member's normal construction as the sole call.
The rebuilt ARMv7 and AArch64 constructor objects each have one
relocation to `AEMath::Matrix::Matrix` in this constructor.

Focused ARMv7 verification compares both constructor entry symbols.
Their source-shape falls from **75.4% to 71.5%** and strict fuzzy from
**45.8% to 35.3%**; neither was linked-exact or byte-exact before or
after. This lower shape score is recorded rather than restoring a
second constructor call solely for the metric. The call count and
member lifetime match the original constructor evidence.

The full ARM corpus compares **4,323/4,524** original functions over
**4,745** pairs. Mean source-shape changes **94.79% to 94.78%** and
strict fuzzy **84.29% to 84.28%**. It retains **2,607 linked-exact /
1,083 byte-exact** functions, zero skips, and no exact-set losses or
gains.

Native and AArch64 `gof2` static-library builds pass. The strict
JNI-initialize AArch64 link diagnostic falls from **90 to 89**
unresolved names, and the broader JNI-onload diagnostic from
**306 to 305**, with no new names. Both remove
`paintcanvas_ctor_matrix(void*)`.

The game shared library and APK remain unbuilt. The matrix's runtime
use during live Android initialization is unverified.
