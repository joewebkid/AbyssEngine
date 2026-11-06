# PaintCanvas DrawRegion2D matrix composition (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass follows the call-target recovery in
`PAINTCANVAS_DRAW_REGION_2D_CALLS_ARM_2026-11-06.md` and covers the
matrix tail of `PaintCanvas::DrawRegion2D` at `0x78818`.

The original calls `AEMath::MatrixSetScaling` at `0x789b2` with a
separate identity matrix at stack `+0x88`. It then makes three
`AEMath::operator*(Matrix const&, Matrix const&)` calls through GOT
slot `0x20fe9c` (confirmed by `R_ARM_GLOB_DAT`):

1. rotation (`+0x108`) times translation (`+0x188`);
2. pivot translation (`+0x148`) times that result (`+0xc8`);
3. the second result (`+0x10`) times the scaled identity (`+0x88`).

The original assigns the final matrix and passes it to
`PaintCanvas::SetWorldViewMatrix` at `0x789ee`. The reconstruction
previously used four undefined matrix and world-view helper names.
Its last multiply passed the pivot matrix a second time, so the
intended scale was omitted. It now uses the existing `Matrix`,
`MatrixSetScaling`, multiplication, and `SetWorldViewMatrix` APIs in
the confirmed order. `AEMath::Matrix` is 0x3c bytes, matching the
original matrix operand layout.

Focused ARMv7 verification compiles **211/211** translation units.
The target overload rises from **91.4% to 91.7%** source-shape and
from **29.3% to 29.4%** strict fuzzy; it remains non-exact. Windows
native and ARM64 static builds pass, as does strict ARM64 shared
linking. The raw 211-object ARM32 unmatched-symbol inventory falls
**217 -> 213**: only the four matrix helper names disappear, with
no new unresolved name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
skipped, zero wrong-type, and 149 extra symbols. Linked-exact and
byte-exact membership sets are unchanged.

Live region rendering and full ARM32 shared linking remain
unverified. The next priority is the construction of the translation,
pivot, and rotation matrices in this overload: its strict fuzzy
score remains low, so the remaining coordinate semantics need direct
ARM and device checks before declaring this path recovered.
