# PaintCanvas display-size call targets (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `Engine::GetDisplayWidth()` at PLT `0x5e8cc` and
`Engine::GetDisplayHeight()` at PLT `0x5e8d8` from two matrix methods.
`PaintCanvas::SetProjOrthoMatrix` calls them at `0x7ca1a` and
`0x7ca38`; `PaintCanvas::ResetPersMatrix` calls them at `0x7d0c4`
and `0x7d0cc`. In both methods the engine pointer is loaded from
`this + 0x34`. The original reset method converts the returned sizes
as signed 32-bit integers for its floating-point arithmetic.

The reconstructed ARM32 code previously called four undefined
display-size helpers. All four sites now call the corresponding
typed `Engine` methods directly. The local `int` values retain the
signed conversion used by the original matrix calculations.

Focused ARMv7 verification compiles **211/211** translation units.
`SetProjOrthoMatrix` retains **81.3%** source-shape and **66.7%**
strict fuzzy; `ResetPersMatrix` retains **76.0%** source-shape and
**50.9%** strict fuzzy. Neither is exact. The raw unmatched-symbol
inventory over 211 ARM32 objects falls **245 -> 241**, removing exactly
the four display-size helper names and adding none.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match pair is gained or lost relative to
the preceding report. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. Device behavior and full ARM32
shared linking remain unverified.
