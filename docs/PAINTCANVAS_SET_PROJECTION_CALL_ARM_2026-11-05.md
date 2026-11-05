# PaintCanvas orthographic projection call target (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `PaintCanvas::SetProjectionMatrix3d(float, float, float)` from
`PaintCanvas::SetProjOrthoMatrix` at `0x7c9f0`, through PLT entry
`0x5fd90`. The three arguments are the recovered perspective and
near/far values, and `this` is passed as the receiver.

The reconstructed method called an undefined
`paintcanvas_ext_setprojmatrix3d` helper at that site. It now calls
the existing typed `SetProjectionMatrix3d` method. This restores the
call target without changing the surrounding matrix calculation.

Focused ARMv7 verification compiles **211/211** translation units.
`SetProjOrthoMatrix` remains **81.3%** source-shape and **66.7%**
strict fuzzy; it is not exact. The raw unmatched-symbol inventory
over 211 ARM32 objects falls **239 -> 238**, removing only
`_Z31paintcanvas_ext_setprojmatrix3dPvfff` with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No linked-exact or byte-exact pair is gained or
lost relative to the preceding report. Windows native and ARM64
static builds pass, as does strict ARM64 shared linking. Device
rendering and full ARM32 shared linking remain unverified.
