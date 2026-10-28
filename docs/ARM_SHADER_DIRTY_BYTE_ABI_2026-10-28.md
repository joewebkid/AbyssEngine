# Shader dirty-byte ABI and SimpleShader source (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA instructions for `ShaderBaseStruct::ShaderBaseStruct` at
`0x8e424` show a halfword store of `0x0100` at object offset
`+8` (`0x8e45c`). `ShaderBaseStruct::Update` at `0x8e74a`
stores byte `1` at offset `+9`. The previous C++ declared a
two-byte `flags` member at `+8` followed by `dirty` at `+10`.
Consequently, the constructor set the original dirty byte while the
reconstructed update methods read and cleared a different byte.
`flags` and `dirty` are now adjacent one-byte members at `+8`
and `+9`. The constructor initializes them to `0` and `1`;
ARMv7 still combines those stores into the original `0x0100`
halfword write. The meaning of the lower `flags` byte is not yet
established beyond its observed initial value.

REA decompilation of `SimpleShader::Init` at `0x88fdc` passes
embedded vertex and fragment GLSL, then queries `a_position`,
`u_WorldMatrix`, and `glColor`. The strings in
`EmbeddedBasicShaders.h` match the original library byte for byte,
including terminating NULs, at file offsets `0x1ea7f0` (167 source
bytes) and `0x1ea898` (89 source bytes). Previously only ARM64 used
those strings and the `glColor` name; ARMv7 and native passed file
names as shader source and queried `u_color`. All builds now use
the confirmed source and name.

## Verification and limits

ARMv7 compiles 209/209 translation units. Focused
`ShaderBaseStruct` comparison improves **92.54% → 92.99%
source-shape**, with linked-exact **9 → 10** and byte-exact
**5 → 6**; its `Update` function becomes byte-exact.
`SimpleShader` improves **99.83% → 100.00% source-shape**,
with linked-exact **5 → 6**. Its `UpdateMeshData` becomes
linked-exact. `SimpleShader::Init` was linked-exact even before
the GLSL/data correction, illustrating that the code metric alone
did not verify its runtime shader text.

The full ARMv7 corpus compares **4,323/4,524** original functions
over 4,745 pairs, with **94.86% source-shape**, **84.43% strict
fuzzy**, **2,638 linked-exact**, **1,086 byte-exact**, and zero
skipped units. Compared by `(unit, symbol)` with the preceding
full report, the pair set is unchanged, 18 linked-exact functions
and one byte-exact function are gained, and neither exact set loses
any member. The new linked matches include updates in
`ShaderBaseStruct`, `SimpleShader`, `DNSShader`,
`BumpMapping`, `GenericShader` variants, and other shader
families.

Native and ARM64 static libraries build. The ARM64 shared library
links without unresolved symbols and exports 52 Java entry points
plus `JNI_OnLoad`. The signed silent APK embeds that library,
passes ZIP alignment and verifies under Android signature schemes
v1, v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-simple-abi-debug-20261028.apk`
(SHA-256 `8526f1d0f532a611fec1367822d90d6e1d2afc9fb3da1c9717362a1562963106`).

Live shader compilation and the effects of the corrected dirty
byte in gameplay remain unverified because the target ARM64 phone
is absent from ADB. Next, test the foreground game and inspect
`NoTexShader`'s placeholder GLSL.
