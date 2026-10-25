# Engine projection matrix call targets (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Engine::SetOrthoMatrix` at `0x854e8` calls
`AbyssEngine::esMatrixMultiply(ESMatrix*, ESMatrix*, ESMatrix*)`
at `0x855a8` with output projection, copied view and input projection.
`Engine::SetWorldViewMatrix` at `0x853b0` calls the same function at
`0x8542c` with output world-view-projection, local world-view and
input projection. The callee is present at `0x84618`.

The recovered Engine instead declared an undefined global
`esMatrixMultiply(void*, const void*, const void*)`. Both call sites
now use the existing namespaced function with its real `ESMatrix`
signature and the ARM-confirmed argument order. The ortho call
aliases its output with the right input; the recovered callee writes
from a complete local result, so that use is preserved.

Focused source-shape/strict fuzzy remains **55.2%/37.0%** for
`SetOrthoMatrix` and **80.8%/53.1%** for `SetWorldViewMatrix`, both
nonexact. The previous scores had masked the wrong call destination.
Native `gof2`, the AArch64 static archive and **209/209** ARMv7
translation units build. A source-extracted host test uses
noncommutative matrices to check multiplication order and in-place
output at `-O0/-O2`. Strict AArch64 whole-archive link gaps fall
**577 → 576**. No shared library, live GL projection or visual
rendering is claimed.

The full ARM corpus compares **4,743** symbol pairs, covering
**4,323/4,524** original functions with **201** missing and no skipped
comparisons. Mean source-shape is **94.71%** and strict fuzzy is
**84.21%**; **2,601 linked-exact** and **1,083 byte-exact** matches
remain. Compared with the preceding FBO-dispatch baseline, the
linked-exact and byte-exact symbol sets have no losses or gains.

Next integration priority: recover the remaining shader update
boundaries and execute one frame with a live Android GL context.
