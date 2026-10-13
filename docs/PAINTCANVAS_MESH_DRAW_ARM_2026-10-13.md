# PaintCanvas recursive mesh draw ARM audit (2026-10-13)

The Android ARMv7 `PaintCanvas::DrawMesh(Mesh *, Matrix &, Matrix &,
unsigned, Matrix &)` starts at `0x7e978`. It copies the incoming world
and UV matrices. For an animated mesh it composes world, animation
position, positive pivot, rotation without translation, and negative
pivot, replaces the UV matrix with the animation's local matrix, and
multiplies the packed color channels.

The method transforms the mesh's bounds center and radius vector by
the resulting world matrix. A mesh with vertices is frustum-tested;
rejection increments `culledCount` and returns. A visible mesh without
a material sets the engine color, passes `view * world` as the
world-view matrix while preserving `world` as the model matrix, sets
the UV matrix, draws, then resets UV. A material-backed mesh queues
the mesh, world/UV/view matrices, and color. After the parent mesh,
animated child meshes are visited in reverse order; child transforms
are tested against the *incoming* world matrix and drawn with the
animated world matrix.

The local body formerly used unresolved shims throughout this path,
including a zero constant in the color divisor and a matrix mutated
while forming world-view. It now uses the existing typed math, engine,
material, and canvas APIs. An extracted-production-body host smoke
checks frustum acceptance/rejection, zero-vertex traversal, distinct
world/view/UV matrices, RGBA scaling and animation color, material
queueing, animated pivot composition, reverse child-mesh order, and
child-transform world-matrix arguments. The smoke uses a reduced
one-axis affine matrix fixture and stub renderer; it does not establish
full 3D, GL, asset, or ARM32 runtime behavior.

Focused source-shape rises **81.0% → 91.3%**, strict fuzzy **15.9% →
28.4%**; the method is not linked- or byte-exact. Native `gof2`, **206/206**
ARM translation units, the new and adjacent canvas smokes, and 36
verifier tests pass (one skipped). Full corpus: **4,717** comparisons,
**4,323/4,524** original functions covered and **201** missing.
Average source-shape rises **92.5667% → 92.5708%**. Average strict fuzzy
moves **80.8845% → 80.8823%** because three newly emitted material-cache
template comparisons enter the denominator; the target method itself
improves. The **2,378 linked-exact** and **1,008 byte-exact** symbol sets
have no losses or gains.

Evidence: Android disassembly `0x7e978`–`0x7ee88` and
`analysis/gof2_libgof2hdaa_full_ida.c`; focused baseline
`A:/Galaxy on Fire new version/_work/canvas_mesh_draw_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_mesh_draw_focus_20261013.json`,
and full corpus
`A:/Galaxy on Fire new version/_work/canvas_mesh_draw_full_20261013.json`.
Next priority: the indexed `PaintCanvas::DrawMesh(unsigned)` wrapper,
which still delegates through an unresolved drawing shim.
