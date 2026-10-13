# PaintCanvas point-frustum ARM audit (2026-10-13)

The Android ARMv7
`PaintCanvas::CameraIsPointinViewFrustum(const Vector &)` starts at
`0x7ba98`. It returns visible when the selected camera index is
invalid (`0x7bab4`–`0x7babe`). With an uninitialized canvas it calls
the engine-level point-frustum function with a null extra matrix and
the selected camera (`0x7bac0`–`0x7bb90`). Otherwise it constructs an
identity rotation, reads the second double from
`Engine::GetGravValue`, scales it by `1.57079637`, negates the
floating-point angle unless `gameOrientation == 1`, writes sine and
cosine into the rotation matrix, and passes that matrix to the same
frustum function (`0x7bac6`–`0x7bb50`). The camera pointer is loaded
at each call site, after the orientation work in the initialized path.

The local wrapper previously used a zero gravity scale and five
unresolved shims. It also omitted the uninitialized null-matrix
branch. It now calls the typed engine-level frustum function and uses
the real `Engine` and `AEMath` methods. The existing extracted-body
frustum smoke was extended to check invalid-index return, null-matrix
dispatch without a gravity read, point/camera forwarding, and both
orientation signs. The lower-level frustum calculation is stubbed in
that smoke, so real camera geometry, scene rendering, and ARM32
runtime behavior are not established by it.

Focused source-shape rises **86.0% → 100.0%** and strict fuzzy
**50.3% → 100.0%**. This is normalized instruction-shape agreement,
not linked- or byte-exactness. Native `gof2`, **206/206** ARM
translation units, the host smoke, and 36 verifier tests pass (one
skipped). Full corpus: **4,714** comparisons, **4,323/4,524** original
functions covered and **201** missing. Average source-shape rises
**92.5600% → 92.5630%**, strict fuzzy **80.8612% → 80.8717%**, and
100%-fuzzy functions **2,527 → 2,528**. The **2,378 linked-exact**
and **1,008 byte-exact** symbol sets have no losses or gains.

Evidence: original Android disassembly at `0x7ba98`–`0x7bb90` and
`analysis/gof2_libgof2hdaa_full_ida.c`; baseline
`A:/Galaxy on Fire new version/_work/canvas_sphere_full_20261013.json`,
focused baseline
`A:/Galaxy on Fire new version/_work/canvas_point_baseline_20261013.json`,
focused final `A:/Galaxy on Fire new version/_work/canvas_point_focus_20261013.json`,
and full final `A:/Galaxy on Fire new version/_work/canvas_point_full_20261013.json`.
Next priority: audit `PaintCanvas::DrawTransform(unsigned, const Matrix *)`
at `0x796a0`; its canvas gravity scale remains zero and it retains
unresolved orientation/dispatch shims.
