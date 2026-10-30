# BumpRimCubeShader_new original ARM recovery (2026-10-30)

Primary source: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `Init` at ELF `0x888ac` first calls `LoadBindShader` with
`data/shader/BumpRimCubeShader_new.vs` and
`data/shader/BumpRimCubeShader_new.fs`. When that returns zero, it calls
`ES2LoadProgram` with embedded GLSL, not those paths. Its PC-relative
literal references identify the vertex string at `0x1e9430` (2,195
bytes, SHA-256
`bdb4a67dff1babc32f6f02f912d8d77cd03b0c53b3a1c078a630c6a29f01b962`)
and the fragment string at `0x1e9cc4` (2,809 bytes, SHA-256
`ace075d43aa3ae78d26f1f086431d82d6b72725ec5298b5b809927925010df88`).
Both C++ arrays were decoded and compared byte-for-byte with the
original ELF ranges. All five attribute and 25 uniform query names
match the original ARM PC-relative literal order. The existing
`uniform13` then `uniform12` storage sequence was preserved.

The original `UpdateMeshData` at `0x88b58` loads four GOT entries.
Their `R_ARM_GLOB_DAT` relocations name `Engine::lodBiasDiffuse` at
`0x210028`, `Engine::lodBiasNormal` at `0x21002c`,
`Engine::LodDistShader` at `0x20ff48`, and `Engine::fogEnabled` at
`0x20fed4`. The previous source used private zero-valued placeholders.
The ARM call to `glUniform1f` passes the four-byte value of
`Engine::LodDistShader`; its C++ declaration was previously `void *`
and is now `float`. The original ARM symbol and new ARMv7 and ARM64
object symbols are each four bytes. Original code also invokes
`AEMath::Vector::operator float*` for `fogColor` before `glUniform3fv`;
the source now does the same instead of casting its address.

Focused ARM comparison: `UpdateMeshData` improves from **96.7% to
100% source-shape** and from **87.2% to 100% strict fuzzy**, becoming
linked-exact. All **6/6** functions in this unit compare linked-exact.
`Init` was already linked-exact with incorrect file paths, GLSL, and
input names; linked code comparison alone did not detect those wrong
referenced string bytes. The separate binary checks above cover them.

Native `gof2` and ARM64 static builds pass, and ARMv7 compiles all
**209/209** translation units. A strict ARM64 shared library links
without undefined symbols and retains the preceding **53 JNI exports**.
The signed, zip-aligned parallel-install APK at
`D:\gof2-arm64-crypto\gof2-arm64-bumprimcube-new-poco-clone-debug-20261030.apk`
contains that library byte-for-byte; its signature verifies. The APK
has not been installed or run because Play Protect blocked earlier
clone installations on Poco X3. Shader compilation, rendering, and
gameplay remain unverified on device.

Focused report:
`D:\gof2-arm64-crypto\bump-rimcube-new-focused-20261030.json`.
The full ARM corpus compares **4,323/4,524** original functions across
**4,745** pairs, with **94.87%** mean source-shape, **84.45%** strict
fuzzy, **2,643 linked-exact**, **1,086 byte-exact**, zero skipped, and
201 missing originals. Against the preceding
`bump-refract-full-20261030.json` snapshot, complete `(unit, symbol)`
sets show one linked-exact gain (`BumpRimCubeShader_new::UpdateMeshData`)
and zero linked-exact or byte-exact losses. The rounded corpus-wide
source-shape and strict fuzzy averages are unchanged. Full report:
`D:\gof2-arm64-crypto\bump-rimcube-new-full-20261030.json`.
