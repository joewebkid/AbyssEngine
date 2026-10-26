# Android texture shader fog dispatch (2026-10-26)

## Original evidence and reconstruction

The reference is Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`TextureShader::UseShader` at `0x90f74` and
`TextureAlphaTestShader::UseShader` at `0x8914c` both read
`Engine::fogEnabled` before selecting the second GL program, and fall back
to the first program when the second program is zero. Their previous ARMv7
source paths read separate static bytes initialized to zero, so the fog
program could not be selected there. Both Android builds now use the
original engine flag.

`TextureShader::UpdateMeshData` at `0x910f8` and
`TextureAlphaTestShader::UpdateMeshData` at `0x892a4` select uniform and
attribute slot 0 or 1 from the same engine flag. The original functions
read the flag again when setting `u_EnableFog`; the source now does so on
ARMv7 as well as ARM64. The PC path retains its existing logic.

The original `TextureShader::UpdateMeshData` at `0x911ba` loads three
floats from `engine + 0x34c` for the `u_eyeposmodel` uniform. The source
layout maps this offset to `Engine::lightColor`; the previous ARM64 path
read `Engine::eyePosition` at another offset. ARM64 now uses the observed
field. This documents the machine-code value source, not the inferred
semantic meaning of the historical member name.

## Validation and remaining uncertainty

ARMv7 compiles 209/209 translation units. Native and ARM64 static library
builds pass. The ARM64 shared library links with no undefined symbols; the
silent test APK is rebuilt and its signature verifies. Runtime rendering
on the ARM64 phone is still unverified because that device is not visible
to ADB.

Focused ARMv7 verification compares eight function pairs per family.
`TextureShader` source-shape rises from 91.92% to 99.65% unweighted and
from 95.95% to 99.14% weighted by original instruction count.
`TextureAlphaTestShader` rises from 91.86% to 99.42% unweighted and from
95.77% to 98.68% weighted. In both families, `UseShader` rises from 42.8%
to 100% source-shape and becomes linked-exact. `UpdateMeshData` rises from
92.6% to 97.2% in `TextureShader`, and from 92.1% to 95.4% in
`TextureAlphaTestShader`; neither is linked-exact. The residual difference
in each update function has not been accepted as a semantic match.

The full ARMv7 corpus compares 4,323/4,524 original functions across 4,745
pairs, with 94.82% source-shape, 84.35% strict fuzzy, 2,610 linked-exact,
1,083 byte-exact, and zero skipped units. Compared with the preceding full
report, the pair set is identical; the two `UseShader` functions are the
only new linked-exact pairs. No linked-exact or byte-exact pairs are lost.
