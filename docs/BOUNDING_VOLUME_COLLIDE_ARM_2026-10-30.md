# BoundingVolume::collide ARM child-loop recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BoundingVolume::collide(float, float, float)` spans `0x11f870`–`0x11f8aa`.
The original ignores its three incoming coordinates. At `0x11f884`–
`0x11f892`, it loads this volume's center from object offset `+8` and
passes that center to each child's virtual `collide` slot at `+8`.
The reconstruction already did this, so the argument selection was
retained.

At `0x11f896`–`0x11f89e`, the original reloads `children` and its count
after each child returns false. The prior range-based C++ loop captured
the end of the array before the calls. It has been replaced by an index
loop that repeats the ARM-observed reads. This matters if a child call
changes the parent array; no change was made to the center values or the
early true return.

Focused ARM verification improves from **74.5% to 100.0%** source-shape
and **40.8% to 100.0%** strict fuzzy. The function becomes both
linked-exact and byte-exact. The 14-function BoundingVolume unit rises
from six to seven linked-exact and from three to four byte-exact, with
no exact-symbol losses. ARMv7 compiles 209/209 translation units;
native `gof2` and ARM64 static libraries build.

The full corpus compares **4,323/4,524** original functions over
**4,745** pairs: **94.91%** mean source-shape, **84.55%** strict fuzzy,
**2,660 linked-exact**, **1,088 byte-exact**, zero skipped and 201
missing. Against the preceding full report, the complete `(unit,
symbol)` exact sets have no losses; `BoundingVolume::collide` is the
sole new linked-exact and byte-exact match.

Live behavior when a child mutates the array remains unverified. The
focused and full-corpus reports are stored outside Git in
`D:\gof2-arm64-crypto`.
