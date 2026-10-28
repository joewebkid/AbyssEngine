# BumpMapping GLSL and update branch (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BumpMapping::Init` at `0x912f4` passes embedded vertex and fragment
GLSL to `ES2LoadProgram`. The recovered vertex string matches the
NUL-terminated original at file offset `0x1f8a00` (517 source bytes),
and the fragment matches `0x1f8c06` (848 source bytes). REA references
from `Init` identify both addresses. The previous source passed the
file names `BumpMapping.vsh` and `.fsh` as shader text. Its fourth
attribute query used `a_binormal`; the original uses `a_bitangent`.
The four uniform queries now use the ARM-observed `u_WorldMatrix`,
`u_lightposmodel`, `s_texture_base`, and `s_texture_normal` names.

`BumpMapping::UpdateMeshData` at `0x913f8` tests its `dirty` byte
before clearing it. The previous C++ cleared it unconditionally.
The reconstructed branch now follows the original control flow while
preserving behavior for both flag values. The original then uploads
the matrix and light vector and conditionally supplies UV and
normal/tangent/bitangent arrays according to the mesh format bits;
those data paths were already present and are unchanged.

## Verification and limits

The restored GLSL strings were compared byte for byte with the
original library, including their terminating NUL bytes. ARMv7
compiles 209/209 translation units; native and ARM64 static libraries
build. Focused six-function source-shape improved **99.57% → 99.92%**
unweighted and **99.29% → 99.86%** weighted by original instructions.
Strict fuzzy improved **98.88% → 99.60%**. The long
`UpdateMeshData` function improved **97.4% → 99.5% source-shape**;
five other family functions remain linked-exact. `Init` was already
linked-exact despite wrong shader strings and names, so matching
instructions alone did not verify its behavior.

The full ARMv7 corpus compares **4,323/4,524** original functions over
4,745 pairs, with **94.85% source-shape**, **84.40% strict fuzzy**,
**2,619 linked-exact**, **1,085 byte-exact**, and zero skipped units.
The pair set and both exact-match sets are identical to the previous
full report: no exact-match gains or losses. The focused improvement
is too small to change the rounded whole-corpus means.

The ARM64 shared library links without unresolved symbols and exports
all 53 JNI entry points. The signed silent APK embeds that exact
library, passes ZIP alignment and verifies under Android signature
schemes v1, v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-bumpmapping-debug-20261028.apk`
(SHA-256 `dd6a2edf0f0455e8fe09035584aacca942f3b2dee2d3bc2e454d702a0d1fd70e`).

The target ARM64 phone is absent from ADB, so live shader compilation
and bump-mapped material output remain unverified. Next, confirm a
foreground scene and inspect `DNSShader`'s placeholder GLSL.
