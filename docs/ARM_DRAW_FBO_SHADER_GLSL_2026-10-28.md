# DrawFBOShader GLSL recovery (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`DrawFBOShader::Init` at `0x884d4` passes embedded shader source to
`ES2LoadProgram`. The vertex string is 220 bytes at file offset
`0x1e9280`; the fragment string is 174 bytes at `0x1e935d`. Both
recovered strings match NUL-terminated bytes in the original library.
The original then queries `a_position`, `a_texCoord`, `u_WorldMatrix`,
and `s_texture_base`. The previous source passed file names as shader
source and queried `position`, `texCoord`, `worldViewMatrix`, and
`texture`, so its GL program and locations could not match the original.
The source now uses the confirmed embedded strings and names.

## Verification and limits

ARMv7 compiles 209/209 translation units; native and ARM64 static
builds pass. The ARM64 shared library links without undefined symbols
and exports all 53 JNI entry points. The silent test APK is signed and
verified under Android signature schemes v1, v2, and v3:
`D:\gof2-arm64-crypto\gof2-arm64-drawfbo-debug-20261028.apk`
(SHA-256 `70c6d1117a45703752d3a481b9efd6b00a50f97002d9c3f7c47a93fb16cdd86e`).

Focused ARMv7 verification compares nine `DrawFBOShader` functions:
98.63% mean source-shape unweighted, 98.11% weighted by original
instructions, and five linked-exact functions. These instruction
metrics are unchanged by the literal and input-name correction;
machine-code shape alone cannot establish shader-content correctness.
The long `RenderEffect` remains 98.3% source-shape and nonexact.

The full ARMv7 corpus compares 4,323/4,524 original functions across
4,745 pairs, with 94.85% source-shape, 84.40% strict fuzzy,
2,619 linked-exact, 1,085 byte-exact, and zero skipped units. The
`(unit, symbol)` pair set and both exact-match sets are unchanged from
the preceding full report: no exact gains or losses.

The target ARM64 phone is absent from ADB, so shader compilation and
live FBO output in the game are not verified. Next, test the signed APK
with GOF2 confirmed in the foreground, then inspect remaining render
state differences if the scene is wrong.
