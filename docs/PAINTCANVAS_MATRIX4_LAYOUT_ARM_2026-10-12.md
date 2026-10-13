# PaintCanvas matrix-field layout — Android ARM audit (2026-10-12)

## Evidence and correction

In Android 2.0.16 `libgof2hdaa.so`,
`PaintCanvas::SetProjectionMatrix3d` at ELF `0x7b808` clears 64 bytes
at Canvas `+0x38..+0x77`. `PaintCanvas::Initialize` at `0x7e6bc`
clears the next 64-byte projection block at `+0x78..+0xb7` and writes
the world-view block through `+0xf4`. The Canvas constructor at
`0x77674` calls `AEMath::Matrix::Matrix` at Canvas `+0xf8`, after that
third 64-byte block. The ten-argument `DrawRegion2D` at `0x78538`
reads the image Array count/data at `+0x14c/+0x150`. These independent
offsets support three 16-float Canvas matrices followed by one
15-float `AEMath::Matrix` and the resource arrays.

The previous header put two unused words before the matrices, modeled
all four as 15-float `Matrix` members, and added a compensating pad
after them. Its `projMatrix3d` and `projOrthoMatrix` started eight and
four bytes too late; `worldViewMatrix` started correctly, but every
`m[15]` access to these three fields was out of the declared array.
The first three are now `CanvasMatrix4` (`float m[16]`), and the unused
words and compensating pad are gone. ARM-only static assertions check
`0x38`, `0x78`, `0xb8`, `0xf8`, resource `0x134`, and image `0x14c`
offsets. The standalone `AEMath::Matrix` type is unchanged.

`MaterialDraw` was the only non-Canvas caller that passed one of the
retyped fields as an `AEMath::Matrix`. It now copies the first 15
floats into that interface without a type-punning reference. Android
`MaterialDraw` at `0x870b8` has a separate matrix-product/model-matrix
sequence that the current C++ still does not recover; this patch does
not claim its semantics are fixed.

## Verification and limits

- Native `gof2` static library: pass. ARM: 206/206 translation units,
  zero failed; the offset assertions compile. Existing production-body
  region and native math host smokes pass. Verifier suite: 37 run,
  36 pass, one skipped.
- Five focused Canvas symbols average 78.0% to 77.8% source-shape and
  37.7% to 39.5% strict fuzzy. `Initialize` rises 81.7% to 84.2%
  source-shape; `MaterialDraw` falls 85.8% to 82.5% and remains
  non-exact. Focused reports:
  `A:/Galaxy on Fire new version/_work/paintcanvas_matrix_layout_before_20261012.json`,
  `A:/Galaxy on Fire new version/_work/paintcanvas_matrix_layout_after_20261012.json`,
  `A:/Galaxy on Fire new version/_work/materialdraw_matrix_before_20261012.json`,
  and `A:/Galaxy on Fire new version/_work/materialdraw_matrix_after_20261012.json`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  rises 92.5365% to 92.5371%; strict fuzzy rises 80.8140% to
  80.8212%. Linked-exact rises 2,376 to 2,378 (`CameraGetLocal` and
  `TransformGetLocal`); byte-exact stays 1,008. No previously exact
  symbol is lost. Full report:
  `A:/Galaxy on Fire new version/_work/paintcanvas_matrix_layout_full_20261012.json`.

The host smokes do not execute the complete Canvas object, OpenGL,
original assets, or ARM32 gameplay. The explicit copy in `MaterialDraw`
preserved the numeric first 15 floats but did not recover its original
matrix chain. The subsequent
[MaterialDraw audit](MATERIAL_DRAW_MATRIX_LIGHT_ARM_2026-10-13.md)
replaces that interim copy with the ARM-backed product and routing.
