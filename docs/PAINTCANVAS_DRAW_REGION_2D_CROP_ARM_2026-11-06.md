# PaintCanvas cropped DrawRegion2D geometry (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers the ten-argument `PaintCanvas::DrawRegion2D` overload
at `0x78538`, used to draw a cropped image region.

The original writes twelve position floats for its four vertices.
At `0x785da`, it explicitly stores zero at position offset `+44`,
the last float. The reconstruction wrote only the first eleven,
leaving that component from an earlier use of the mesh. It now
clears the twelfth component as the original does.

The original constructs three 0x3c-byte matrices. ARM stack writes
at `0x78748–0x78754` place `(-pivotX, -pivotY)` in the translation
matrix at stack `+0x108` and `(x + pivotX, y + pivotY)` in the matrix
at `+0xc8`. The angle is passed to `AEMath::Sinf/Cosf`; their results
form the rotation matrix at `+0x88`. Calls at `0x78778` and `0x78790`
compute `rotation * unpivot` and then
`position * (rotation * unpivot)`, with `Matrix::operator=` calls
between and after them. The previous source flattened the products
into direct translation expressions. It now uses the same matrix
operations and the same integer-before-float calculations for the
positive and negative pivots.

The existing UV code was also checked against the original VFP
sequence at `0x785e2–0x78674`: it retains the original unsigned
region dimensions and signed-short conversion of `srcX` in the
upper U coordinate. No UV change was made in this pass.

Focused ARMv7 verification compiles **211/211** translation units.
The target overload rises from **74.2% to 90.4%** source-shape and
from **19.1% to 31.5%** strict fuzzy; its reconstructed body grows
from 158 to 195 instructions versus 218 original instructions. It
remains non-exact. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. The raw 211-object ARM32
unmatched-symbol inventory remains **213**, with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape rises **96.81% -> 96.82%** and
strict fuzzy **86.88% -> 86.89%**. It retains **2,798 linked-exact**
and **1,113 byte-exact**, 201 missing, zero skipped, zero wrong-type,
and 149 extra symbols. Exact-match membership sets are unchanged.

Live cropped-region rendering and full ARM32 shared linking remain
unverified. Next priority: inspect `PaintCanvas::SetProjectionMatrix3d`
and its remaining external math and GL placeholders by original ARM
call targets.
