# TexOnlyShader GLSL recovery (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`TexOnlyShader::Init` at `0x8b6c0` passes an embedded vertex shader
and fragment shader to `ES2LoadProgram`. The recovered 220-byte vertex
string matches the original library at file offset `0x1e9280`, and the
225-byte fragment string matches at `0x1ee32d`, both including their
NUL terminators. The original then queries `a_position`, `a_texCoord`,
`u_WorldMatrix`, and `s_texture_base`. The previous source passed
`.vsh/.fsh` file names as shader source and queried `s_texture` for the
sampler. It now uses the confirmed GLSL and uniform name.

## Verification and limits

ARMv7 compiles 209/209 translation units; native and ARM64 static
builds pass. The ARM64 shared library links without undefined symbols
and exports all 53 JNI entry points. The silent test APK is signed and
verified under Android signature schemes v1, v2, and v3:
`D:\gof2-arm64-crypto\gof2-arm64-texonly-debug-20261028.apk`
(SHA-256 `67dc890c341c0f8af29db9db1b9d1afc5fc86de2499f469dd3c27eb7aa8f60a5`).

Focused ARMv7 verification compares six `TexOnlyShader` functions:
99.88% mean source-shape unweighted, 99.83% weighted by original
instructions, and five linked-exact functions. These instruction
metrics are unchanged by the literal and sampler-name correction.
In particular, `Init` was already linked-exact while passing wrong
shader text; machine-code matching alone did not verify this behavior.

The full ARMv7 corpus compares 4,323/4,524 original functions across
4,745 pairs, with 94.85% source-shape, 84.40% strict fuzzy,
2,619 linked-exact, 1,085 byte-exact, and zero skipped units. The
`(unit, symbol)` pair set and both exact-match sets are unchanged from
the preceding full report: no exact gains or losses.

The target ARM64 phone is absent from ADB, so shader compilation and
live texture output in GOF2 remain unverified. Next, confirm the
foreground game scene on that phone, then inspect the remaining shader
families with placeholder GLSL inputs.
