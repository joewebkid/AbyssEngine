# PaintCanvas line drawing and ARM64 link closure (2026-10-26)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PaintCanvas::DrawRectangle` occupies `0x79368–0x794d4`, and
`PaintCanvas::DrawLine` occupies `0x794d8–0x7968c`.

The recovered functions previously called thirteen undefined external
signatures for matrix setup, integer-to-float conversion, GL operations,
and client-state changes. They also read separate zero-initialized local
flags, which forced the fixed-function branch. The original ARMv7 code
loads `Engine::enableShader` via its GOT relocation at `0x20fe08` and
calls `Engine::AEClientState` through the relocation at `0x20feb4`.
Both functions now use that shared shader flag and the directly observed
GL/Engine calls. The four integer coordinates in `DrawLine` convert to
float in place. Both stack matrices are typed 15-float `Matrix` values.

The original `Engine::InitGL` at `0x6db20` stores its `shaders` argument
to the same `Engine::enableShader` symbol at `0x6db70` (GOT
`0x20fe08`) and reads it again before `ShaderInit`. The recovered code
only initialized a separate internal `g_Engine_useShaders` flag, so
`PaintCanvas` and other users of the public static byte would always
see zero. `InitGL` now initializes both flags from the same argument.
Its focused source-shape/strict fuzzy changes **74.6%/59.3% →
76.4%/57.1%**; it remains nonexact. The rebuilt ARMv7 object stores
the byte at offset `0x1360` through a GOT relocation for
`Engine::enableShader` at object offset `0x1430`.

For `DrawRectangle`, the ARM calls at `0x7941e`, `0x79438`,
`0x79442–0x794a0` establish the matrix setup, shader path, and
fixed-function sequence. The shader path calls
`Engine::DrawLine2D(lineVerts, 4, true)`. The fixed-function path
disables texture 2D, enables only the vertex client array, and draws
four vertices as GL mode 2 before restoring texture 2D.

For `DrawLine`, the equivalent calls are at `0x79538`, `0x7957c`,
and `0x7958a–0x79656`. The shader path calls
`Engine::DrawLine2D(lineVerts, 2, false)`. The fixed-function path
disables texture 2D and draws two vertices as GL mode 1. The previous
placeholder incorrectly enabled texture 2D before that draw.

The rebuilt ARMv7 object's relocations now target
`PaintCanvas::SetWorldViewMatrix`, `Engine::DrawLine2D`,
`Engine::GlEnable`, `Engine::AEClientState`, `glLineWidth`,
`glVertexPointer`, and `glDrawArrays` directly, with no remaining
`paintcanvas_ext_dl_*` or `paintcanvas_ext_dr_*` references. Focused
source-shape/strict fuzzy changes **72.9%/44.2% → 89.0%/50.0%** for
`DrawLine` and **88.3%/82.1% → 97.4%/77.7%** for `DrawRectangle`.
Neither is exact. The rectangle's strict fuzzy decrease is retained
because its call targets, shader branch, and GL state changes are
confirmed by the original code. The focused unit compares 198
functions, with 92 linked-exact and 23 byte-exact.

The Windows native and AArch64 static libraries build, as do all
**209/209** ARMv7 translation units. A strict AArch64 whole-archive
link probe has **555** unique unresolved names, down from **568**;
the probe still fails and no game `.so` or APK was produced. Real
Android line drawing and shader switching remain unverified until a
working host, full dependency closure, and audio implementation exist.

The complete ARM corpus compares **4,743** symbol pairs and covers
**4,323/4,524** original functions, with **201** missing and no
skipped comparisons. Mean source-shape is **94.71%**, strict fuzzy
**84.21%**, linked-exact **2,601**, and byte-exact **1,083**. Both
exact-symbol sets are identical to the preceding RocketGun baseline:
zero gains and zero losses. The unchanged corpus averages reflect the
small number of functions in this package, not a claim of exactness for
the three edited functions.
