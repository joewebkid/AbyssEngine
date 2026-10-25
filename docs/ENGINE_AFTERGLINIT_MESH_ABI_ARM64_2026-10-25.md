# Engine GL quad creation and vendor strings (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Engine::AfterGLInit` at `0x8428c` calls
`AbyssEngine::MeshCreate(Engine*, ushort, ushort, signed char, Mesh**)`
at `0x842b2`, then assigns two temporary `String` objects to
`Engine::vendor` and `Engine::renderer` at `0x842e6` and `0x8430c`.
The original static symbols are each 12 bytes, consistent with the
ARMv7 `String` layout. `Engine::~Engine` calls
`AbyssEngine::MeshRelease(Engine*, Mesh**)` at `0x841c6`.
The rebuilt ARMv7 object also places the two static fields at a
12-byte stride.

The recovered Engine instead declared global `MeshCreate` and
`MeshRelease` functions with wrong signatures. It also wrote the GL
vendor/renderer strings through two null-initialized pointers while
declaring the actual static objects as math vectors. The source now
uses the ARM-confirmed namespace, signatures and `String` statics.

Focused `AfterGLInit` source-shape/strict fuzzy improves from
**92.0%/68.1%** to **94.5%/76.4%**, still nonexact. Both destructor
aliases remain **88.0%/75.8%**, nonexact. Native `gof2`, the AArch64
static archive, and **209/209** ARMv7 translation units build. A
source-extracted host test checks mesh arguments, quad indices and GL
strings at `-O0/-O2`. The strict AArch64 whole-archive link now
reports **580** unresolved symbols, down from **582**. It still does
not produce a shared library or prove a live GL-context startup.

The full ARM corpus compares **4,323/4,524** original functions over
**4,743** pairs with zero skips. Mean source-shape/strict fuzzy remain
**94.71%/84.21%**; **2,601 linked-exact** and **1,083 byte-exact**
symbols remain, with zero symbol-level gains or losses against the
previous full report.

Next integration priority: resolve the remaining actual engine and
platform calls on the startup path, then exercise `InitGL` with an
Android GL context and original assets.
