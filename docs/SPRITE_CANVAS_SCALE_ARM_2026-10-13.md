# Sprite canvas and scale dispatch — Android ARM (2026-10-13)

Android `Sprite` constructors at `0xd0d18`/`0xd0dd0`, `draw` at
`0xd0e68`, and `drawRegion` at `0xd0f8c` call the canvas through
`Globals::Canvas`. The recovered source used a separate
`PaintCanvas::gCanvas` pointer. Those globals are assigned in different
initialization paths, so equality cannot be assumed. All four Sprite
methods now use the ARM-backed canvas route.

In the multi-image branch of `Sprite::draw`, Android takes the unscaled
`DrawImage2D` path only when `scaleX == 1.0f`; it does not test `scaleY`.
The recovered source used `scaleX == 1.0f || scaleY == 1.0f`, dropping
horizontal scaling when `scaleX != 1` but `scaleY == 1`. That condition
is corrected. The single-image `DrawRegion2D` path is unchanged.

An extracted-body host smoke runs both production Sprite constructors,
`setFrame`, `setPosition`, `draw`, `drawRegion`, the destructor, and the
production `ImageFactory::reload`. It gives `Globals::Canvas` and
`PaintCanvas::gCanvas` different fixture objects, checks all calls go to
the former, tests asymmetric scales and both image modes, and reloads
the six-frame sprite twice. This verifies routing and the frame-array
destruction call on the host, not actual game image-handle ownership,
OpenGL, or ARM32 execution.

The focused Sprite `draw` source-shape changes **94.4% → 92.7%** while
strict fuzzy rises **45.6% → 50.0%**. The measured local shape decrease
was accepted because ARM control flow and the host behavior agree on the
correct scale condition. All **206/206** ARM translation units compile;
native `gof2`, this smoke, the adjacent ImageFactory portrait smoke, and
37 verifier tests pass (one skipped).

The 18-symbol Sprite family averages **98.02% → 97.92%** source-shape
and **85.71% → 85.96%** strict fuzzy. The full ARM corpus has **4,720**
comparison rows, **4,323/4,524** original functions covered and **201**
missing, **92.67619%** average source-shape (was 92.67655%),
**81.04487%** strict fuzzy (was 81.04394%), **2,388** linked-exact
and **1,008** byte-exact (both unchanged). There are zero skipped units
and wrong-type symbols, and neither previous exact comparison-row set
lost a member. The tiny source-shape decrease is not evidence of wrong
runtime behavior; the ARM branch and host test support the corrected
scale dispatch.

Evidence: Android IDA output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
focused baseline
`A:/Galaxy on Fire new version/_work/sprite_canvas_scale_baseline_20261013.json`
and focused final
`A:/Galaxy on Fire new version/_work/sprite_canvas_scale_focus_20261013.json`,
plus full baseline
`A:/Galaxy on Fire new version/_work/image_factory_family_full_final_20261013.json`
and full final
`A:/Galaxy on Fire new version/_work/sprite_canvas_scale_full_20261013.json`.
Next recover a separate high-impact stub, `ParticleSystemMesh::emitTrail`,
while keeping real image rendering and ARM32 runtime as untested gates.
