# PaintCanvas::DrawSpriteSystem(index) camera/orientation path (2026-10-13)

Android ARMv7 `libgof2hdaa.so` at `0x7aa70` checks the sprite-system
slot, starts with an identity world matrix, and renders even if there
is no selected camera. With a camera and no orientation adjustment,
it assigns the inverse of that camera's local matrix. In the
initialized branch it builds a rotation from the second gravity
double multiplied by `1.57079637`, chooses its sign by landscape
orientation, multiplies it onto the camera matrix, and assigns that
matrix's inverse. Finally it calls the real `SpriteSystemDraw` with
an identity view and the computed world matrix.

The local method previously inverted its identity world matrix in
the no-orientation branch rather than the camera, multiplied gravity
by zero, and used nine unresolved draw/math shims. It now uses the
existing `Engine::GetGravValue`, `AEMath` matrix and trigonometry
methods, and the real `SpriteSystemDraw`. The index/null guards and
no-camera draw behavior remain. The `Matrix` and sprite layouts were
not changed.

An extracted-production-body native smoke checks invalid/null sprite
slots, no-camera identity draw, camera translation inversion, and the
two orientation signs. The adjacent `SpriteSystemDraw` host smoke
also passes. The new fixture simplifies matrix multiplication for an
identity-rotation camera, so it does not prove non-identity camera
composition, GL output, real game assets, or ARM32 runtime behavior.
Run `python tools/run_canvas_sprite_draw_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`.

Focused source-shape falls **89.5% → 74.9%** and strict fuzzy falls
**41.9% → 40.2%** as the unresolved shim calls are replaced with real
operations; this method remains non-exact. Native `gof2`, all
**206/206** ARM translation units, both host smokes, and 36 verifier
tests pass (one skipped). Full corpus: **4,714** comparisons,
**4,323/4,524** original functions covered, **201** missing.
Average source-shape changes **92.5578% → 92.5547%** and strict fuzzy
**80.8534% → 80.8531%**. The **2,378 linked-exact** and **1,008
byte-exact** symbol sets have no gains or losses.

Evidence: Android IDA dump `analysis/gof2_libgof2hdaa_full_ida.c`
at `0x7aa70`; baseline
`A:/Galaxy on Fire new version/_work/getscreen_world_family_full_20261013.json`,
focused `A:/Galaxy on Fire new version/_work/draw_sprite_canvas_focus_20261013.json`,
and full `A:/Galaxy on Fire new version/_work/draw_sprite_canvas_full_20261013.json`.
Next priority: audit the two-matrix `DrawSpriteSystem` overload at
`0x7adb0`, used by `ParticleSystemSprite`; it still has a zero-valued
gravity scale and unresolved orientation/draw shims.
