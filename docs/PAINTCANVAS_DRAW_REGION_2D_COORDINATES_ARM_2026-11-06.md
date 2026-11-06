# PaintCanvas DrawRegion2D coordinates (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers the float-angle `DrawRegion2D` overload at `0x78818`,
following its call-target and matrix-order recovery.

The ARM entry receives the angle in `r2`, local pivot X in `r3`,
and the remaining pivot Y, screen X/Y, and scales on the stack.
The angle bits are saved at `0x78840`, loaded at `0x7895c`, and
passed unchanged to `AEMath::Sinf` at `0x78986` and `Cosf` at
`0x78992`. The reconstruction had marked the angle unused and took
the sine and cosine of `-pivotX * scaleX` instead.

The original VFP conversions and arithmetic at `0x78948–0x78982`
write the two translation matrices as follows:

- local translation: `(-pivotX * scaleX, -pivotY * scaleY)` at
  stack `+0x188`;
- screen translation: `(x + pivotX * scaleX, y + pivotY * scaleY)`
  at stack `+0x148`.

The reconstruction had these values in the opposite matrices and
misnamed the integer arguments. The source and header now use
`angle, pivotX, pivotY, x, y, scaleX, scaleY`, matching the register
and stack order. The rotation matrix still contains `cos(angle)`,
`-sin(angle)`, and `sin(angle)` in the original positions; the
matrix multiplication order recovered in the previous pass remains.

Focused ARMv7 verification compiles **211/211** translation units.
The target overload rises from **91.7% to 91.9%** source-shape and
from **29.4% to 31.2%** strict fuzzy; it remains non-exact. Windows
native, ARM64 static, and strict ARM64 shared builds pass. The raw
211-object ARM32 unmatched-symbol inventory remains **213**, with no
added name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
skipped, zero wrong-type, and 149 extra symbols. Linked-exact and
byte-exact membership sets are unchanged.

Live region rendering and full ARM32 shared linking remain
unverified. Next priority: inspect the other `DrawRegion2D` overload
and compare rendered coordinates on a device when one is available.
