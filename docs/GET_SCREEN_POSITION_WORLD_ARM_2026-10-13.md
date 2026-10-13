# PaintCanvas world-vector screen projection (2026-10-13)

The Android ARMv7 `GetScreenPosition(Matrix &, const Vector &, Vector &)`
at `0x7bd58` first transforms the world vector by the supplied matrix.
If a camera exists, it inverse-transforms the point by the camera's
matrix. When canvas orientation is initialized, it obtains the second
gravity double from `Engine::GetGravValue`, multiplies it by
`1.57079637`, chooses the rotation sign by landscape orientation,
multiplies the camera matrix by that rotation, and inverse-transforms
the point. The method returns whether the projected point satisfies
the original depth, horizontal-divisor, X, and upper-Y tests. It
does not reject a zero vertical divisor or a negative Y coordinate.

The local method previously ignored its matrix, used a zero gravity
scale and unresolved transform/math shims, declared a `void` return,
and discarded the visibility result. It now uses the existing real
`AEMath` and `Engine` methods and returns the source-backed boolean.
The two-vector overload at `0x7bce8`, used by in-tree game callers,
now passes a typed identity matrix to this method instead of an
unresolved `paintcanvas_ext_getscreenpos_m` shim. Its explicitly
initialized matrix preserves its **100.0% source-shape / 100.0%
strict fuzzy** ARM comparison. This is not an exact-link claim.

A native smoke compiles the two production method bodies with fixture
camera/math services and checks world and camera transforms, the
wrapper, absent camera, depth and horizontal-divisor failures,
negative X and Y behavior, and gravity rotation in both orientations.
Run `python tools/run_getscreen_world_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`.
The fixture simplifies matrix multiplication for an identity camera;
it does not test the real renderer, original assets, original-device
execution, or non-identity camera rotation composition.

Focused world-matrix source-shape improves **79.4% → 82.1%** and strict
fuzzy **21.6% → 36.9%**; it remains non-exact. Native `gof2`, the
smoke, all **206/206** ARM translation units, and 36 verifier tests
pass (one skipped). Full corpus has **4,714** comparisons,
**4,323/4,524** original functions covered and **201** missing.
Average source-shape rises **92.5573% → 92.5578%**, strict fuzzy
**80.8502% → 80.8534%**. The **2,378 linked-exact** and **1,008
byte-exact** symbol sets have no losses or gains.

Evidence: Android IDA dump `analysis/gof2_libgof2hdaa_full_ida.c`
at `0x7bce8` and `0x7bd58`; baseline
`A:/Galaxy on Fire new version/_work/getscreen_matrix_full_20261013.json`,
focused `A:/Galaxy on Fire new version/_work/getscreen_world_family_focused_20261013.json`,
and full `A:/Galaxy on Fire new version/_work/getscreen_world_family_full_20261013.json`.
Next inspect other canvas gravity/orientation paths that still use
zero-valued placeholder scales, starting with `DrawSpriteSystem`.
