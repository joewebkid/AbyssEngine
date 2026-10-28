# GenericShader GLSL recovery (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`GenericShader::Init` at `0x8e7dc` passes embedded vertex and fragment
GLSL to `ES2LoadProgram`. The restored vertex string is byte-identical
to the NUL-terminated original at file offset `0x1f39b0` (1,174 source
bytes); the fragment matches offset `0x1f3e47` (1,250 source bytes).
The original assembly and decompilation identify the `a_position`,
`a_texCoord`, `a_normal`, `a_tangent`, `a_bitangent` attributes and the
uniform names now queried in `Init`. The previous reconstruction passed
`.vsh/.fsh` file names as shader source and queried `a0` through `a4`
and `u0` through `u11`, none of which are names in these GLSL strings.

The ARM function also queries `u_ModelMatrix` and `glColor`, though
neither is declared in these strings. Those queries are retained as
observed; a driver may return `-1`. This patch does not change the
subsequent matrix or material-value assignments.

## Verification and limits

ARMv7 compiles 209/209 translation units. Native and ARM64 static
libraries build. The ARM64 shared library links with no unresolved
symbols and retains all 53 JNI exports. The signed silent APK embeds
that exact library and verifies under Android signature schemes v1,
v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-generic-debug-20261028.apk`
(SHA-256 `3f46f969838795ddbe0015cb9f4655a641dbfcfd19fdf7391ebf79008edcaed4`).

Focused ARMv7 verification compares six `GenericShader` functions:
**99.70% source-shape** unweighted, **99.84%** weighted by original
instructions, **99.17% strict fuzzy** unweighted, and five linked-exact
functions. The before and after focused reports are identical;
`Init` was already linked-exact while supplying the wrong shader text.
The full ARMv7 corpus compares **4,323/4,524** original functions over
4,745 pairs, with **94.85% source-shape**, **84.40% strict fuzzy**,
**2,619 linked-exact**, **1,085 byte-exact**, and zero skipped units.
The pair set and both exact-match sets are identical to the previous
full report, so there are no exact-match losses.

The target ARM64 phone is absent from ADB; live shader compilation,
material rendering, and the remaining `UpdateMeshData` semantics are
unverified. Next, verify a foreground scene on that phone and inspect
the adjacent `GenericShader1` and `GenericShader2` placeholders.
