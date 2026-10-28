# TextureVtxColorShader GLSL and uniform source (2026-10-28)

## Android ARM evidence

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA decompilation of `TextureVtxColorShader::Init` at `0x8bed0`
shows one embedded vertex shader and two embedded fragment shaders
passed to `ES2LoadProgram`. The source strings already stored in
`EmbeddedBasicShaders.h` match the original library, including their
terminating NUL bytes, at file offsets `0x1ef460` (1,273 source
bytes), `0x1ef95a` (573 bytes), and `0x1efb98` (691 bytes).
Previously only ARM64 used those strings; the ARMv7 and native path
passed incomplete GLSL, including a newline as the fog fragment.
All builds now use the original strings.

The original `UpdateMeshData` at `0x8bf6e` loads the three values
for `u_eyeposmodel` from `Engine + 0x34c` (instructions
`0x8c010`–`0x8c016`). The current 32-bit layout calls that field
`lightColor`. ARMv7 already used it, while the ARM64 path used
`eyePosition` from a different offset. Both paths now use the
observed value source. The historical semantic meaning of the
`lightColor` member name remains unverified.

## Verification and limits

ARMv7 compiles 209/209 translation units. Focused verification
compares eight functions and remains at **99.59% source-shape /
90.69% strict fuzzy**, with four linked-exact and zero byte-exact.
The original GLSL text is data behind relocations, so this metric
does not establish that the old shader text was correct. The full
corpus remains **4,323/4,524** originals across 4,745 pairs,
**94.85% source-shape / 84.41% strict fuzzy**, **2,620 linked-exact /
1,085 byte-exact**, zero skips. The pair set and both exact-match
sets are identical to the preceding full report.

Native and ARM64 static libraries build. The ARM64 shared library
links without unresolved symbols and exports 52 Java entry points
plus `JNI_OnLoad`. The signed silent APK embeds the exact library,
passes ZIP alignment and verifies under Android signature schemes
v1, v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-texture-vtx-debug-20261028.apk`
(SHA-256 `05d354e94061dc8c45da818dfb8632d30763bfad780435a7afae6d0bdabb0f23`).

Live compilation and rendering of the normal and fog programs
remain unverified because the target ARM64 phone is absent from ADB.
The next source target is the remaining basic shader family,
beginning with `SimpleShader`.
