# VertexColorShader original GLSL and ARM gate (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`VertexColorShader::Init` at `0x901fc` loads two PC-relative strings before
its `ES2LoadProgram` call. They resolve to ELF addresses `0x1f63b0`
(vertex, 721 bytes, SHA-256
`deeef1f45579525f38c7360f72053ac03cdc1c68ac4137c0b2cbc1a682f8b3ac`)
and `0x1f6682` (fragment, 583 bytes, SHA-256
`417043ab962b86c0740606636933dc5ca40ae3cc180f2fa081615aec14d69483`).
The C++ string bytes in `EmbeddedVertexColorShader.h` were compared
byte-for-byte with both original ranges. A UTF-8 comment in the
fragment source is encoded with octal escapes so its bytes survive
the C++ source conversion exactly.

The same ARM `Init` branch queries six attributes in this order:
`a_vertexColor`, `a_position`, `a_texCoord`, `a_normal`, `a_tangent`,
`a_bitangent`. It then queries nine uniforms in this order:
`u_ModelViewProjectionMatrix`, `u_ModelMatrix`, `u_lightdirmodel`,
`u_eyeposmodel`, `s_texture`, `glColor`, `u_AmbientColor`,
`u_DiffuseColor`, `u_SpecularColor`. The source now supplies these exact
strings instead of the previous file-name placeholders and generic
names. The existing field layout and `UpdateMeshData` body are retained;
both already compare linked-exact in the ARM corpus.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles 209/209
translation units. Focused ARM comparison covers six VertexColorShader
functions: **100% source-shape / 100% strict fuzzy**, **six linked-exact**,
zero byte-exact, unchanged from the previous code snapshot. The full
corpus compares **4,323/4,524** original functions across **4,745**
pairs, with **94.86%** mean source-shape, **84.43%** strict fuzzy,
**2,638 linked-exact** and **1,086 byte-exact**. It has zero skipped
units and 201 missing original functions. The complete `(unit, symbol)`
linked-exact and byte-exact sets are identical to the preceding
`settextures-full-20261030.json` snapshot. Reports are saved outside
Git in `D:\gof2-arm64-crypto`.

A strict ARM64 shared library links without undefined symbols and
retains exactly the previous 53 JNI exports. A parallel-install APK
with this library is signed and verified outside Git at
`D:\gof2-arm64-crypto\gof2-arm64-vertexcolor-poco-clone-debug-20261030.apk`.
The original GOF2 package and OBB remain installed on the Poco X3.
The first parallel-install attempt, using the preceding SetTextures
build, was rejected by phone-side Play Protect verification with
`INSTALL_FAILED_VERIFICATION_FAILURE`. A second regular install with
this VertexColorShader APK displayed the same Play Protect block:
"Suspicious app blocked" because the app targets an older Android
version. The pending install was cancelled and its temporary uploaded
APK removed from the phone. No clone package was installed or run.
No runtime shader compilation or visual improvement is claimed for
this package. Next, complete a user-permitted device install and check
the live shader status, GL errors, and frame output; then prioritize
the remaining original-source shader gaps.
