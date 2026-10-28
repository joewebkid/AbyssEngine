# GenericShader1 GLSL and attribute ABI (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`GenericShader1::Init` at `0x8ec2c` passes embedded GLSL to
`ES2LoadProgram`. Its vertex string is byte-identical to the
NUL-terminated original at file offset `0x1f4350` (454 source bytes),
and its fragment string matches offset `0x1f4517` (281 source bytes).
REA references from `Init` point to both exact string addresses. The
other similar fragment at `0x1f3840` lacks the trailing tab and is not
the one used by this function.

The ARM stores attribute locations at `+0x20` through `+0x30` in this
order: `a_position`, `a_texCoord`, `a_normal`, `a_tangent`,
`a_bitangent`. `UpdateMeshData` feeds them position, two-component UV,
normal, tangent and binormal arrays in the same order. The reconstructed
header previously named the second through fifth slots as normal,
tangent, binormal and texCoord, causing `Init` to query mismatched
names for the arrays. Their field names now match the original slot
meaning while preserving all offsets. The nine uniform queries now
use their ARM-observed names. Previously `Init` passed `.vsh/.fsh`
file names as GLSL and queried `u_m0` through `u_m8` placeholders.

## Verification and limits

The restored strings were independently compared byte for byte with
the original library, including their terminating NUL bytes. ARMv7
compiles 209/209 translation units; native and ARM64 static libraries
build. The focused six-function ARM report is identical before and
after: **99.95% source-shape** unweighted, **99.90%** weighted by
original instructions, **99.75% strict fuzzy**, and five linked-exact
functions. `Init` was linked-exact even before the GLSL and name
correction; the instruction metric alone did not verify its behavior.
After the final literal formatting, ARMv7 rebuilt 209/209 units and
the `GenericShader1` object hash remained identical to the object
used by the full verifier. The final focused report remained identical.

The full ARMv7 corpus compares **4,323/4,524** original functions over
4,745 pairs, with **94.85% source-shape**, **84.40% strict fuzzy**,
**2,619 linked-exact**, **1,085 byte-exact**, and zero skipped units.
The pair set and both exact-match sets are identical to the preceding
full report; there are no exact-match losses.

The ARM64 shared library links without unresolved symbols and retains
all 53 JNI exports. The signed silent APK embeds the rebuilt library
and passes ZIP alignment and Android signature schemes v1, v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-generic1-debug-20261028.apk`
(SHA-256 `1314686eefbb027cf5f6be3c3a6332305f9aa3f6f0a53964315f5af2c2b94ad6`).

The target ARM64 phone is absent from ADB, so live shader compilation,
rendering and material semantics remain unverified. Next, test a
foreground game scene on that phone and inspect the separate
`GenericShader2` placeholder family.
