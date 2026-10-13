# PaintCanvas sphere-frustum ARM audit (2026-10-13)

The Android ARMv7 function
`PaintCanvas::CameraIsSphereinViewFrustum(const Vector &, float)` starts at
`0x7bbb8`. It returns visible when the radius is zero or the current
camera index is invalid (`0x7bbd0`–`0x7bbea`). When the canvas is
uninitialized, it calls the engine-level frustum check with a null
extra matrix and the selected camera (`0x7bbec`–`0x7bcbe`). Otherwise
it constructs an identity rotation, reads the second double from
`Engine::GetGravValue`, scales by `1.57079637`, negates the
floating-point angle unless `gameOrientation == 1`, writes sine and
cosine into the rotation matrix, and passes that matrix to the same
engine-level check (`0x7bbf2`–`0x7bc80`). The angle is not truncated
to an integer.

The local wrapper previously used a zero gravity scale and five
unresolved shims. It also missed the uninitialized null-matrix call.
The recovered wrapper now uses the typed `Camera *` array and the real
`Engine`, `AEMath`, and engine-level frustum function. An extracted
production-body host smoke covers zero radius, invalid camera index,
null-matrix dispatch without a gravity read, camera/point/radius
forwarding, and both orientation signs. It stubs the lower-level
frustum calculation, so actual camera geometry and ARM32 runtime
behavior are not verified by that smoke.

Focused source-shape rises **84.6% → 98.4%** and strict fuzzy
**68.9% → 78.6%**; this wrapper is not linked- or byte-exact. Native
`gof2`, **206/206** ARM translation units, the host smoke, and 36
verifier tests pass (one skipped). Full corpus: **4,714** comparisons,
**4,323/4,524** original functions covered and **201** missing.
Average source-shape rises **92.5571% → 92.5600%** and strict fuzzy
**80.8591% → 80.8612%**. The **2,378 linked-exact** and **1,008
byte-exact** symbol sets have no losses or gains.

Evidence: original Android disassembly at `0x7bbb8`–`0x7bcbe` and
`analysis/gof2_libgof2hdaa_full_ida.c`; baseline
`A:/Galaxy on Fire new version/_work/draw_sprite_one_matrix_full_20261013.json`,
focused baseline
`A:/Galaxy on Fire new version/_work/canvas_sphere_baseline_20261013.json`,
focused final `A:/Galaxy on Fire new version/_work/canvas_sphere_focus_20261013.json`,
and full final `A:/Galaxy on Fire new version/_work/canvas_sphere_full_20261013.json`.
Next priority: the adjacent `PaintCanvas::CameraIsPointinViewFrustum`
at `0x7ba98`, which still has a zero gravity scale and unresolved
orientation/frustum shims.
