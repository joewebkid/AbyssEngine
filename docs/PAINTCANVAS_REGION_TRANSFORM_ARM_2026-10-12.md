# PaintCanvas cropped-region transform — Android ARM audit (2026-10-12)

## Evidence and recovery

Android 2.0.16 `libgof2hdaa.so`, ELF `0x78538`, symbol
`_ZN11AbyssEngine11PaintCanvas12DrawRegion2DEjiiiifiiii`, takes
`(image, srcX, srcY, width, height, angle, pivotX, pivotY, x, y)`.
The IDA listing in `analysis/gof2_libgof2hdaa_full_ida.c` and ARM call
sequence agree on `T(x+pivot) * R(angle) * T(-pivot)`, followed by
`SetWorldViewMatrix`, disabled culling, mesh draw, and re-enabled
culling. The original writes index-buffer words `0x20000`, `1`, and
`0x20003`.

The previous local body ignored `angle`, derived the trigonometric input
from `y+pivotY`, passed uninitialized scratch storage through unresolved
matrix-product shims, and wrote zero for the third index word. It also
used unresolved conversion, texture, GL, and mesh-draw shims. The method
now computes the equivalent 2D affine matrix directly, forwards it as
an `AEMath::Matrix` to the real Canvas method, and uses the real engine
calls. The direct formula avoids `AEMath::operator*(Matrix, Matrix)`,
which at this snapshot wrote `m[15]` out of bounds on a 15-float result.
The subsequent [matrix audit](MATRIX_CTOR_PRODUCT_ARM_2026-10-12.md)
removes that write; this region patch did not change the matrix ABI.

The host smoke extracts the unchanged production method body and checks
out-of-range early return; the source crop, vertex, and index writes;
zero angle and 90-degree rotation around a nonzero pivot; and the
texture/matrix/cull/draw/cull call sequence.

## Verification and limits

- Native `gof2` static library: pass. ARM: 206/206 translation units,
  zero failed. Host smoke: `PaintCanvas region host smoke: OK`.
  Verifier suite: 37 run, 36 pass, one skipped.
- Focused source-shape: 82.4% before, 74.2% after. Strict fuzzy:
  18.5% before, 19.1% after. The method remains non-exact. The
  source-shape drop is expected from replacing two matrix products and
  two assignment calls with direct, defined arithmetic; it is not
  evidence of progress toward byte identity. Focused reports:
  `A:/Galaxy on Fire new version/_work/paintcanvas_region_before_20261012.json`
  and `A:/Galaxy on Fire new version/_work/paintcanvas_region_after_20261012.json`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  92.5338% before to 92.5321% after; strict fuzzy 80.8033% to
  80.8034%. Linked-exact 2,376 and byte-exact 1,008; exact per-symbol
  sets unchanged. Full report:
  `A:/Galaxy on Fire new version/_work/paintcanvas_region_full_20261012.json`.

The fixture does not render original assets, execute real OpenGL, or
establish ARM32 runtime correctness. Remaining recovery includes the
other `DrawRegion2D` overload and the `AEMath::Matrix` size contract.
The subsequent matrix audit fixes the product bounds but not every
remaining matrix-layout question.
