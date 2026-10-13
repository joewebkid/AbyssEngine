# PaintCanvas::DrawSpriteSystem(index, Matrix) ARM audit (2026-10-13)

The Android ARMv7 function at `0x7ac20` checks the sprite-system slot
and inverts its by-value matrix when the canvas is uninitialized. In
the initialized branch it constructs an identity rotation, reads the
second gravity value, multiplies it by `1.57079637`, negates the
floating-point angle unless `gameOrientation == 1`, writes `Cosf` and
`Sinf` into the rotation matrix, multiplies the input matrix by it,
and inverts the result. It then calls `SpriteSystemDraw` with an
identity view, the inverted matrix, and the selected sprite system.
Unlike the two-matrix overload, this angle is **not integer-truncated**.
The ARM call sequence at `0x7ad0e`–`0x7ad70` confirms the multiply,
inverse, and draw argument order.

The local implementation had a zero-valued gravity scale and eight
unresolved matrix, trigonometry, and draw shims. It now calls the
existing `Engine`, `AEMath`, and `SpriteSystemDraw` methods. The index
and null guards remain. Explicit identity construction preserves the
ARM source shape without changing the matrices' values.

The extracted-production-body host smoke checks invalid and null
slots, the uninitialized inverse translation, fractional gravity
rotation under both orientation signs, identity view, and draw
dispatch. Its matrix-multiply fixture covers identity inputs only;
it does not prove arbitrary-matrix composition, real GL output,
original assets, or ARM32 device behavior. The adjacent
`SpriteSystemDraw` smoke also passes.

Focused source-shape rises **87.5% → 92.8%** and strict fuzzy
**36.4% → 57.2%**; the method is not linked- or byte-exact. Native
`gof2`, **206/206** ARM translation units, both host smokes, and 36
verifier tests pass (one skipped). Full corpus: **4,714** comparisons,
**4,323/4,524** original functions covered and **201** missing.
Average source-shape rises **92.5560% → 92.5571%** and strict fuzzy
**80.8547% → 80.8591%**. The **2,378 linked-exact** and **1,008
byte-exact** symbol sets have no losses or gains.

Evidence: Android IDA dump `analysis/gof2_libgof2hdaa_full_ida.c`
and ARM disassembly at `0x7ac20`–`0x7ad70`; baseline
`A:/Galaxy on Fire new version/_work/draw_sprite_two_matrix_full_20261013.json`,
focused `A:/Galaxy on Fire new version/_work/draw_sprite_one_matrix_focus_20261013.json`,
and full `A:/Galaxy on Fire new version/_work/draw_sprite_one_matrix_full_20261013.json`.
Next priority: audit the remaining canvas gravity/orientation paths,
starting with `CameraIsSphereinViewFrustum` at `0x7bcd8`, which still
uses a zero gravity scale and unresolved shims.
