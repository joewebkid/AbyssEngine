# PaintCanvas recursive DrawTransform ARM audit (2026-10-13)

The Android ARMv7
`PaintCanvas::DrawTransform(Transform *, const Matrix &, Matrix &)`
starts at `0x798e4`. It returns when the transform is null or its
`visible` byte at `+0xec` is clear. Otherwise it multiplies the parent
matrix by the transform's `worldMatrix`, then multiplies by
`rotationMatrix` when the keyframe array has nonzero count
(`0x7990e`–`0x79928`). It calls the typed `DrawMesh` for each mesh,
passing the combined matrix, incoming view matrix, transform id,
and transform local matrix (`0x79934`–`0x79950`).

Each child is independently tested with `Transform::InCameraVF` and
the combined matrix only when the current camera index is valid. A
passing child is drawn recursively with that same combined matrix and
incoming view matrix; a failed test or invalid camera increments
`culledCount` (`0x7995a`–`0x7999a`). The parent itself is not culled
by this method; the indexed wrapper performs that check.

The local body previously used five unresolved matrix, mesh, frustum,
and recursive-draw shims. It now calls the existing typed `AEMath`,
`Transform`, and `PaintCanvas` methods. An extracted-production-body
host smoke checks null/invisible guards, parent and child matrix
composition, keyframe rotation, mesh argument order, recursion,
frustum rejection, and missing-camera culling. The smoke uses a
translation-only matrix fixture and stubs `DrawMesh` and the
lower-level frustum calculation. It therefore does not verify full
3D matrix arithmetic, mesh rendering, GL state, assets, or ARM32
runtime behavior.

Focused source-shape rises **89.5% → 100.0%** and strict fuzzy
**64.2% → 94.9%**; the method is not linked- or byte-exact. Native
`gof2`, **206/206** ARM translation units, the new and adjacent
transform smokes, and 36 verifier tests pass (one skipped). Full
corpus: **4,714** comparisons, **4,323/4,524** original functions
covered and **201** missing. Average source-shape rises **92.5644% →
92.5667%** and strict fuzzy **80.8780% → 80.8845%**. The **2,378
linked-exact** and **1,008 byte-exact** symbol sets have no losses or
gains.

Evidence: original Android disassembly at `0x798e4`–`0x7999a` and
`analysis/gof2_libgof2hdaa_full_ida.c`; baseline
`A:/Galaxy on Fire new version/_work/canvas_draw_transform_full_20261013.json`,
focused baseline
`A:/Galaxy on Fire new version/_work/canvas_recursive_draw_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_recursive_draw_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_recursive_draw_full_20261013.json`.
Next priority: `PaintCanvas::DrawMesh(Mesh *, Matrix &, Matrix &,
unsigned, Matrix &)` at `0x7e978`, which still retains unresolved
matrix, frustum, and render shims.
