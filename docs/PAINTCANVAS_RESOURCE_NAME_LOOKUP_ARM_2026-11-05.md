# PaintCanvas resource-name comparisons (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `String::Compare(const char *)` through PLT `0x5fa3c` from
`PaintCanvas::GetMeshResourceId(String &)` at `0x78030` and
`PaintCanvas::GetTextureResourceId(String &)` at `0x7807c`. The
corresponding two-argument mesh-resource lookup was already corrected
in the preceding pass.

The remaining reconstructed calls to the undefined
`paintcanvas_ext_strcmp` helper now invoke the typed
`String::Compare(const char *)` method. This removes the helper
declaration and all of its uses in `PaintCanvas.cpp`.

Focused ARMv7 verification compiles **211/211** translation units.
Both changed lookup methods retain **100%** source-shape, **100%**
strict fuzzy and linked-exact membership. The raw unmatched-symbol
inventory over 211 ARM32 objects falls **246 -> 245**, removing only
`_Z22paintcanvas_ext_strcmpPvS_` with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match pair is gained or lost relative to
the preceding report. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. Device behavior and full ARM32
shared linking remain unverified.
