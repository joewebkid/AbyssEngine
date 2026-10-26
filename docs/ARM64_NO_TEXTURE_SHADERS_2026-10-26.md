# ARM64 no-texture shader recovery (2026-10-26)

## Evidence and change

The original Android ARM `libgof2hdaa.so` has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Its `NoTexShader::Init` at `0x90654` passes embedded vertex and fragment
sources to `ES2LoadProgram` and queries `a_position`, `u_WorldMatrix`, and
`glColor`. `NoTexVtxColorShader::Init` at `0x8dfb8` also queries
`a_VertexColor`. The four exact, null-terminated GLSL strings are at file
offsets `0x1f6900` (209 bytes), `0x1f69d2` (227), `0x1f33d0` (327), and
`0x1f3518` (308). `EmbeddedNoTexShaders.h` copies their bytes exactly.

On AArch64, both classes now compile those sources and use the recovered
attribute and uniform names. The ARMv7 matching paths remain as before.
Their existing `UpdateMeshData` calls are consistent with the original
functions at `0x906b6` and `0x8e03a`.

## Verification and limit

All four literals pass `glslangValidator` for their vertex or fragment
stage. Native and AArch64 static builds pass, the AArch64 shared game
library links without undefined symbols, and the debug APK verifies under
signature schemes v1, v2, and v3. ARMv7 compiles 209/209 units. Focused
ARMv7 comparison reports 6 pairs, 99.1% source-shape, 5 linked-exact for
`NoTexShader`; 6 pairs, 99.5% source-shape, 5 linked-exact for
`NoTexVtxColorShader`. Both retain their exact matches.

The full ARMv7 corpus compares 4,323/4,524 original functions across 4,745
pairs, with 94.79% average source-shape, 84.31% strict fuzzy, 2,607
linked-exact, 1,083 byte-exact, and zero skipped units. `(unit, symbol)`
comparison against the pre-change corpus found no linked-exact or byte-exact
losses and no change to the compared pair set.

The APK was installed on the 25053RT47C, but live image output from this
build is unverified. Another application was in the foreground when the
screenshot was captured, so that image is not evidence of GOF2 rendering.
The remaining shader classes still include placeholder sources. The menu's
visual output remains unverified until GOF2 is confirmed in the foreground.
