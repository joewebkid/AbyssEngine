# ARM64 alpha-test texture shader recovery (2026-10-26)

## Source evidence

The reference is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`TextureAlphaTestShader::Init` at `0x89230` passes one vertex source and two
fragment sources to `ES2LoadProgram`. The exact null-terminated sources are
at file offsets `0x1ea920` (974 bytes), `0x1eacef` (460), and `0x1eaebc`
(628); their byte copies are in `EmbeddedTextureAlphaTestShader.h`.

`ConnectShaderComponents` at `0x8916c` queries `s_texture`, `a_position`,
`a_texCoord`, `u_WorldMatrix`, `glColor`, `u_fogColor`, `u_fogMaxDist`,
`u_fogMinDist`, `u_EnableFog`, and `u_eyeposmodel`. `UseShader` at `0x8914c`
selects the second program when `Engine::fogEnabled` is true and that program
exists. `UpdateMeshData` at `0x892a4` uses the same state as its uniform-slot
index and passes it to `u_EnableFog`.

The ARM64 implementation now uses these embedded sources, names, and fog
selection. Existing field names in the reconstructed class are historical;
the restored queries map each field to its observed original slot. The ARMv7
matching path is unchanged.

## Checks and limits

All three source strings pass GLSL stage validation. Native and AArch64
static libraries build; the AArch64 game library links without undefined
symbols and the signed silent test APK was rebuilt. ARMv7 compiles 209/209
translation units. The focused ARMv7 comparison contains eight pairs,
91.9% average source-shape, five linked-exact and zero byte-exact; the five
linked-exact pairs are unchanged from the prior full corpus. The source-shape
figure is for ARMv7 and does not prove AArch64 runtime rendering.

The full ARMv7 corpus compares 4,323/4,524 originals across 4,745 pairs,
94.79% average source-shape, 84.31% strict fuzzy, 2,607 linked-exact and
1,083 byte-exact, with zero skipped units. Exact `(unit, symbol)` sets have
no losses against the preceding full report.

The new APK's image output remains unverified. The ARM64 phone disappeared
from ADB before this package could be installed. The next device capture
must first confirm GOF2 as the top activity. The remaining shader classes
still include placeholder sources; later rendering paths are not yet
verified.
