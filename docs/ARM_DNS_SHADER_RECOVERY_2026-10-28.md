# DNSShader source recovery (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA locates `DNSShader::Init` at `0x8f360` and
`UpdateMeshData` at `0x8f504`. The original initialization first
loads `data/shader/DNSShader.vs` and `.fs` through
`LoadBindShader`, then passes the embedded GLSL to
`ES2LoadProgram` only if that load returns zero. The previous source
passed placeholder file names directly as GLSL. The embedded vertex
and fragment strings are copied from original file offsets
`0x1f48d0` (178 source bytes) and `0x1f4983` (95 source bytes).
Both were compared byte for byte with the library.

The five attribute slots now use the original position, texture
coordinate, normal, tangent and bitangent order. Their member offsets
are unchanged. The 12 uniform lookups now use the names observed in
the original ARM routine, rather than `u_m*` placeholders.
`UpdateMeshData` passes `Engine::lodBiasDiffuse` and
`Engine::lodBiasNormal` to the corresponding uniforms; the old
reconstruction always passed zero from unrelated static variables.
The mesh array and VBO paths retain their original field offsets.

## Verification and limits

ARMv7 compiles 209/209 translation units; native and ARM64 static
libraries build. The focused six-function family improves from
**97.32% to 99.97% source-shape** (94.95% to 99.91% weighted by
original instructions) and from **97.83% to 99.85% strict fuzzy**.
`Init` improves from 86.0% source-shape to 100.0% and becomes
linked-exact. `UpdateMeshData` improves from 97.9% to 99.8%
source-shape. Five of six family functions are linked-exact.

The full ARMv7 corpus compares **4,323/4,524** original functions
over 4,745 pairs, with **94.85% source-shape**, **84.41% strict
fuzzy**, **2,620 linked-exact**, **1,085 byte-exact**, and zero
skipped units. The pair set is unchanged from the previous full
report. `DNSShader::Init` adds one linked-exact match; neither exact
set loses any member.

The ARM64 shared library links without unresolved symbols and exports
52 Java entry points plus `JNI_OnLoad`. A signed silent APK embeds
that exact library, passes ZIP alignment and verifies under Android
signature schemes v1, v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-dns-aligned-debug-20261028.apk`
(SHA-256 `6bd1b953779f9dde90aac3b10fbe9e1393bfaed2cd0cddcfeb102a6a8d12fcce`).

The target ARM64 phone is not present in ADB, so shader compilation
from packaged resource files, fallback selection and live material
rendering remain unverified. The next priority is a foreground device
run and inspection of the remaining shader families.
