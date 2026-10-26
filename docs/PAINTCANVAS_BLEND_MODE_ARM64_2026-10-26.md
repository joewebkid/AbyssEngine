# PaintCanvas blend-state recovery (2026-10-26)

The original Android ARMv7 `PaintCanvas::SetBlendMode` at `0x7cb48`
calls `Engine::LightEnable`, `Engine::LightSetLight`, and
`Engine::GlEnable`, then changes GLES blend, depth-write, alpha-test,
and texture-combine state for the selected mode. The IDA extraction in
`analysis/gof2_libgof2hdaa_full_ida.c` is the source for this call
mapping in the shared workspace. `Engine::enableShader` selects shader versus fixed-function
handling. The previous recovered function used a constant-zero stand-in
for that global, a zero stand-in for the shader capability, and external
placeholder calls for the engine and GLES operations. In particular,
the placeholder named `paintcanvas_ext_sbm_setlight` represented
`glDepthMask`, not a light-setting operation.

The implementation now calls the existing `Engine` methods and GLES
entry points directly and tests the real shader flag. The focused ARMv7
verifier compares 198 `PaintCanvas` symbols. For `SetBlendMode`,
source-shape rises **92.9% → 99.0%** and strict fuzzy rises
**83.4% → 99.1%**; the function is still nonexact. The focused exact
sets stay at **93 linked-exact / 23 byte-exact**, with no losses.
The full corpus compares **4,323/4,524** original functions over
**4,745** pairs, with zero skips. Mean source-shape is **94.76%**;
strict fuzzy rises **84.26% → 84.27%**. All **2,606 linked-exact /
1,083 byte-exact** symbols remain in their exact sets. The native and
AArch64 static libraries build. A strict JNI-reachable
AArch64 link probe falls **162 → 157** unresolved names, removing five
blend-mode placeholders and adding none. The shared library still does
not link.

Device rendering remains unverified. The remaining work includes more
game/engine call closure, an Android host, and an AArch64-compatible
audio path for the original FMOD Ex `.fev` projects; a static-library
build or source-shape score does not establish a playable game.
