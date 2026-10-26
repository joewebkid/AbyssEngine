# Android texture-light shader recovery (2026-10-26)

## Original evidence and reconstruction

The reference is Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`TextureLightShader::Init` at `0x8db68` passes embedded vertex and fragment
sources to `ES2LoadProgram`. The original null-terminated bytes are at file
offsets `0x1f2620` (2,786 bytes) and `0x1f3103` (666 bytes); they are copied
exactly to `EmbeddedTextureLightShader.h`.

The same ARM function obtains `a_position`, `a_texCoord`, `a_normal`,
`u_WorldMatrix`, `u_FullModelMatrix`, `u_ModelMatrix`, `u_UvMatrix`,
`u_UVAnimation`, `glColor`, `s_texture`, both members of the light direction,
ambient, diffuse, and specular arrays, `u_EyePosModel`, and
`u_SpecularPower`. The previous ARM64 path passed filename placeholders as
GLSL and queried synthetic names such as `u0` and `a0`. Both Android ARMv7
and ARM64 now use the original source and queried names. The existing
reconstructed member names remain historical; values are assigned to the
original member offsets. The PC native path keeps its existing source.

`UpdateMeshData` at `0x8dd14` was checked against the original ARM behavior.
Its matrix, color, light, animation, and vertex-buffer inputs already map
to the observed slots, so this change leaves the update path intact.

## Validation and unverified behavior

Both GLSL literals pass vertex/fragment stage validation. Native and ARM64
static builds pass; the ARM64 game shared library links with no undefined
symbols, and the silent test APK is rebuilt and signed. ARMv7 compiles
209/209 translation units. Focused ARMv7 verification compares six pairs.
`Init` rose from 86.3% to 100% source-shape and gained linked-exact status;
the family rose from 97.5% to 99.8% unweighted average source-shape
(99.41% weighted by original instruction count) and from four to five
linked-exact matches. The other four linked-exact matches are retained.
There are no byte-exact matches in this family. `UpdateMeshData` remains
98.5% source-shape; its behavior has not been retested in a game scene.

The full ARMv7 corpus compares 4,323/4,524 original functions across 4,745
pairs, with 94.80% average source-shape, 84.31% strict fuzzy, 2,608
linked-exact, 1,083 byte-exact, and zero skipped units. Compared with the
immediately preceding full report, the `(unit, symbol)` pair set is identical,
the only new linked-exact pair is `TextureLightShader::Init`, and there are
no linked-exact or byte-exact losses.

The revised APK has not been installed or visually tested on the target
ARM64 phone, which remains absent from ADB. GPU compilation, lighting, and
later scene behavior therefore remain unverified. The next device test
should confirm GOF2 is the foreground activity before capturing its screen.
