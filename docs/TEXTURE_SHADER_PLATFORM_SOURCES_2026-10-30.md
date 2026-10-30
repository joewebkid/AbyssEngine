# Texture shader source paths across platforms (2026-10-30)

Primary reference: Android ARM libgof2hdaa.so, SHA-256
6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58.
The package covers TextureShader and TextureAlphaTestShader, which the
engine registers and selects for textured meshes. Their ARM64 source
recovery and fog dispatch were documented previously in
ARM64_ALPHA_TEST_SHADER_2026-10-26.md and
ARM_TEXTURE_FOG_DISPATCH_2026-10-26.md.

The existing embedded GLSL arrays were checked directly against the
original ELF ranges. TextureShader uses vertex 0x1f8140 (1,155 bytes),
fragment 0x1f85c4 (439), and fog fragment 0x1f877c (603).
TextureAlphaTestShader uses vertex 0x1ea920 (974), fragment 0x1eacef
(460), and fog fragment 0x1eaebc (628). All six arrays match
byte-for-byte.

Previously the ARMv7 and Windows branches passed filenames such as
TextureShader.vsh to ES2LoadProgram and queried placeholder names
such as texture / mvp; the ARM64 branch alone used the embedded GLSL
and original GL identifiers. ShaderBaseStruct::ES2LoadProgram treats
its arguments as shader source text, so those filename arguments
could not compile the intended programs. The original ARMv7 TextureShader::Init
at 0x91084 passes its embedded vertex and two fragment strings to
ES2LoadProgram. Both shader classes now use their confirmed embedded
strings and query identifiers on every platform. The PC branches
also read Engine::fogEnabled for program and uniform slot selection
instead of static zero bytes. Both UpdateMeshData functions also pass
Engine::fogColor through the original Vector-to-float-pointer
conversion instead of bypassing that call with a cast. The ARM
disassembly contains this conversion immediately before glUniform3fv.

Native gof2 and ARM64 static builds pass. ARMv7 compiles 209/209
translation units. Focused TextureShader verification rises from 6/8
to **7/8 linked-exact**, **100% source-shape**, and **100% strict fuzzy**;
UpdateMeshData becomes linked-exact. TextureAlphaTestShader remains
**6/8 linked-exact**, while mean source-shape rises from 99.5% to
**99.8%** and mean strict fuzzy from 95.9% to **98.4%**. Its
UpdateMeshData is **98.0% source-shape / 87.0% strict fuzzy**, still
nonexact because register allocation and attribute pointer instruction
order differ. Focused reports:
D:\gof2-arm64-crypto\texture-shader-fog-vector-20261030.json and
D:\gof2-arm64-crypto\texture-alpha-fog-vector-20261030.json.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.88%** mean source-shape, **84.49%** strict fuzzy,
**2,656 linked-exact**, **1,087 byte-exact**, zero skipped, and
**201 missing** (all absent). Compared with
D:\gof2-arm64-crypto\bump-rimcube-full-20261030.json, the only
linked-exact gain is TextureShader::UpdateMeshData; no linked-exact
or byte-exact symbols were lost. Full report:
D:\gof2-arm64-crypto\texture-shader-platform-full-20261030.json.

The ARM64 game shared library/APK has not been rebuilt with this
package. GL compilation and visual behavior on Poco, including fog
and alpha test transitions, remain unverified until the phone is
available again.
