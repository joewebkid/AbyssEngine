# GenericShader2 GLSL and attribute ABI (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`GenericShader2::Init` at `0x8e15c` passes an embedded vertex shader
from file offset `0x1f3680` (447 source bytes) and fragment shader
from `0x1f3840` (280 source bytes) to `ES2LoadProgram`. REA references
at `0x8e160` and `0x8e164` identify those exact strings. The
recovered C++ literals match the original bytes including NUL.
The similar `GenericShader1` strings have different vertex whitespace
and an extra trailing tab in the fragment; they are not substituted
for this class.

The original `Init` stores five attribute locations at `+0x20` through
`+0x30` in position, texCoord, normal, tangent and bitangent order.
`UpdateMeshData` at `0x8e2ba` feeds the corresponding position,
two-component UV, normal, tangent and binormal arrays. The old header
misnamed the latter four slots and `Init` queried the wrong attributes
for those arrays. The field names now match their ARM-observed slots
without moving them. The nine uniform queries also use the original
names; the old implementation passed `.vsh/.fsh` file names as GLSL
and queried `u_m0` through `u_m8` placeholders.

## Verification and limits

ARMv7 compiles 209/209 translation units. Native and ARM64 static
libraries build. The focused six-function report is identical before
and after: **99.95% source-shape** unweighted, **99.90%** weighted by
original instructions, **99.75% strict fuzzy**, and five linked-exact
functions. `Init` was already linked-exact with wrong shader text and
attribute names; that metric did not verify the runtime behavior.

The full ARMv7 corpus compares **4,323/4,524** original functions over
4,745 pairs, with **94.85% source-shape**, **84.40% strict fuzzy**,
**2,619 linked-exact**, **1,085 byte-exact**, and zero skipped units.
The pair set and both exact-match sets are identical to the preceding
full report; there are no exact-match losses.

The ARM64 shared library links without unresolved symbols and exports
all 53 JNI entry points. The silent APK contains that exact library,
passes ZIP alignment and verifies under Android signature schemes v1,
v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-generic2-debug-20261028.apk`
(SHA-256 `7662b9c28fc9618cfbca7f463aa4c1e60082ba445deec0dc3050c4e46a06306d`).

The target ARM64 phone is absent from ADB, so live shader compilation
and material rendering remain unverified. Next, confirm a foreground
scene and inspect the `BumpMapping` placeholder shader family.
