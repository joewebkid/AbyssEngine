# PaintCanvas::DrawTransform(index, viewMatrix) ARM audit (2026-10-13)

The Android ARMv7 wrapper at `0x796a0` skips invalid transform indices
and transforms whose `visible` byte at `+0xec` is clear. It starts with
an identity world matrix. If a current camera exists, it calls
`Transform::InCameraVF` with a null extra matrix while uninitialized,
or with a gravity/orientation rotation while initialized. A failed
frustum test increments `culledCount` and returns without updating
the eye position or drawing. The rotation uses the second double from
`Engine::GetGravValue`, scaled by `1.57079637`, with its floating-point
sign negated unless `gameOrientation == 1` (`0x79752`–`0x797b2`).

After a successful frustum test, the uninitialized path inverts the
provided `viewMatrix`, or the camera projection matrix when the
pointer is null. The initialized path copies the same selected matrix,
multiplies it by the rotation, then inverts it (`0x797b8`–`0x79840`).
Both update `Engine::SetEyePosition` from camera matrix entries at
offsets `+0x18`, `+0x28`, and `+0x38`, then call the typed recursive
`PaintCanvas::DrawTransform(Transform *, const Matrix &, Matrix &)`
with an identity view and the computed world matrix (`0x79842`–`0x7988e`).
Without a current camera, the wrapper dispatches the transform with
both matrices left at identity.

The local wrapper previously had a zero gravity scale, ten unresolved
frustum/matrix/eye/draw shims, and inverted an identity matrix rather
than the selected source in its uninitialized path. The recovered
wrapper now uses existing typed `Transform`, `Camera`, `Engine`,
`AEMath`, and recursive draw methods. An extracted-production-body
host smoke covers index/visibility guards, no-camera dispatch,
both selected-source cases, both culling branches, orientation signs,
eye coordinates, and draw argument order. Its matrix arithmetic is a
simplified 2D fixture and the nested draw is stubbed: arbitrary 3D
matrix composition, recursive mesh/child rendering, original assets,
and ARM32 device behavior remain untested.

Focused source-shape rises **86.6% → 93.5%** and strict fuzzy
**45.0% → 74.5%**; the method is not linked- or byte-exact. A
shared-inverse trial reached 93.9% source-shape but only 49.2% strict
fuzzy, so it was rejected. Native `gof2`, **206/206** ARM translation
units, the host smoke, and 36 verifier tests pass (one skipped). Full
corpus: **4,714** comparisons, **4,323/4,524** original functions
covered and **201** missing. Average source-shape rises **92.5630% →
92.5644%** and strict fuzzy **80.8717% → 80.8780%**. The **2,378
linked-exact** and **1,008 byte-exact** symbol sets have no losses or
gains.

Evidence: original Android disassembly at `0x796a0`–`0x7988e` and
`analysis/gof2_libgof2hdaa_full_ida.c`; baseline
`A:/Galaxy on Fire new version/_work/canvas_point_full_20261013.json`,
focused baseline
`A:/Galaxy on Fire new version/_work/canvas_draw_transform_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_draw_transform_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_draw_transform_full_20261013.json`.
Next priority: the nested recursive `PaintCanvas::DrawTransform` at
`0x798e4`, which still has unresolved mesh/frustum/child shims.
