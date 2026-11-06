# PaintCanvas DrawRegion2D call targets (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers `PaintCanvas::DrawRegion2D(unsigned int, float, int,
int, int, int, float, float)` at `0x78818`.

The original branches to `RestoreImage2D` at `0x78860` when the image
restore flag is set, then calls `SetTexture` at `0x78874` with slot
`0xffffffff`. The first register passed to `RestoreImage2D` contains
the flag rather than the canvas pointer, but that callee reads only
its image argument and overwrites the first register before using it.
The reconstruction now calls the member functions directly, with
the missing texture slot restored.

The original converts six signed integers to floats with VFP
`vcvt.f32.s32` instructions, then calls `AEMath::Sinf` at `0x78986`
and `AEMath::Cosf` at `0x78992`. It calls `glDisable` at `0x789f6`,
`MeshDraw` at `0x78a08`, and `glEnable` at `0x78a10`. The reconstruction
previously used eight undefined `paintcanvas_ext_dr2_*` names for
these operations. They now resolve to the confirmed methods, math
functions, GL calls, and direct signed-to-float conversions.

Focused ARMv7 verification compiles **211/211** translation units.
The target overload rises from **87.6% to 91.4%** source-shape and
from **26.7% to 29.3%** strict fuzzy; it remains non-exact. The other
`DrawRegion2D` overload remains **74.2%** source-shape and **19.1%**
strict fuzzy. Windows native and ARM64 static builds pass, as does
strict ARM64 shared linking. The raw 211-object ARM32 unmatched-symbol
inventory falls **225 -> 217**: exactly eight `dr2` placeholders
disappear, with no new unresolved name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
skipped, zero wrong-type, and 149 extra symbols. Linked-exact and
byte-exact membership sets are unchanged.

Live region drawing and full ARM32 shared linking remain unverified.
The next priority is the remaining matrix and world-view calls in
this overload; their argument order still needs separate ARM and
layout checks before replacing their placeholders.
