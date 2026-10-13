# PaintCanvas::DrawSpriteSystem(index, Matrix, Matrix) ARM audit (2026-10-13)

The Android ARMv7 function at `0x7adb0` checks the sprite-system slot.
Without orientation adjustment it inverts the second matrix. In the
initialized branch it reads the second double returned by
`Engine::GetGravValue`, multiplies by `-1.57079637`, **converts the
result to an integer**, negates it when landscape orientation is one,
then converts that integer to float for `Sinf` and `Cosf`. It
multiplies the second matrix by this rotation, replaces it with its
inverse, multiplies the first matrix by the same rotation, and calls
`SpriteSystemDraw(engine, first, second, system)`. ARM instructions
at `0x7ae9e`–`0x7af2c` confirm the conversion, call order, and
matrix argument addresses. The integer truncation is not replaced
with a smoother floating-point rotation.

The local overload previously had a zero-valued gravity scale and
unresolved matrix, trigonometry, and draw shims. It now uses the
existing real `Engine`, `AEMath`, and `SpriteSystemDraw` methods while
retaining the original index/null guards and by-value matrix inputs.
`ParticleSystemSprite::render` is the in-tree caller.

The extracted-production-body host smoke covers invalid/null slots,
the uninitialized inverse of the second matrix, zero rotation after
integer truncation, both orientation signs, and final argument
order. It uses a simplified matrix-multiply fixture for identity
rotation inputs, so it does not prove arbitrary-matrix composition,
real GL output, original assets, or ARM32 device behavior. The
adjacent `SpriteSystemDraw` smoke also passes. Run
`python tools/run_canvas_sprite_draw_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`.

Focused source-shape improves **79.7% → 85.5%** and strict fuzzy
**27.2% → 34.9%**; the method is not linked- or byte-exact. Native
`gof2`, **206/206** ARM translation units, both host smokes, and 36
verifier tests pass (one skipped). Full corpus: **4,714** comparisons,
**4,323/4,524** original functions covered and **201** missing.
Average source-shape rises **92.5547% → 92.5560%** and strict fuzzy
**80.8531% → 80.8547%**. The **2,378 linked-exact** and **1,008
byte-exact** symbol sets have no losses or gains.

Evidence: Android IDA dump `analysis/gof2_libgof2hdaa_full_ida.c`
and ARM disassembly at `0x7adb0`–`0x7af2c`; baseline
`A:/Galaxy on Fire new version/_work/draw_sprite_canvas_full_20261013.json`,
focused `A:/Galaxy on Fire new version/_work/draw_sprite_two_matrix_focus_20261013.json`,
and full `A:/Galaxy on Fire new version/_work/draw_sprite_two_matrix_full_20261013.json`.
Next priority: audit the remaining one-matrix `DrawSpriteSystem`
overload at `0x7ac20`, which still uses a zero gravity scale and
unresolved orientation/draw shims.
