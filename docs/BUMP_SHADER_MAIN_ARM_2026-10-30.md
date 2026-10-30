# Main BumpShader original ARM source recovery (2026-10-30)

Primary evidence: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `BumpShader::Init` at `0x8b880` passes embedded GLSL to
`ES2LoadProgram`, not resource filenames. PC-relative literals point to
the vertex source at ELF `0x1ee430` (2,161 bytes, SHA-256
`5ee8fe86a368842dfe4e51e3de6f7b1aef1ca27cbda3c48dadf26bde54684d84`)
and fragment source at `0x1eeca2` (1,934 bytes, SHA-256
`943ea97c064499a050d9d88ac1fed38e0b00234448b04f38b80ffdc27a7e6933`).
Both C++ arrays were decoded and compared byte-for-byte to those original
ELF ranges. The same routine's literal references identify all three
attribute names and 22 uniform names, replacing placeholder queries.
The original `Init` and rebuilt `Init` compare linked-exact. The prior
placeholder version also compared linked-exact because the verifier's
code comparison does not establish equality of referenced GLSL or
uniform-name string contents. Binary string comparison was required
to detect and correct this runtime defect.

The original `UpdateMeshData` at `0x8ba88` reads GOT slots `0x210028`,
`0x21002c`, and `0x20fed4`. Their `R_ARM_GLOB_DAT` relocations identify
`Engine::lodBiasDiffuse`, `Engine::lodBiasNormal`, and
`Engine::fogEnabled`, respectively. The prior source read three private
zero-valued placeholders. The original also calls
`AEMath::Vector::operator float*` before sending `fogColor` through
`glUniform3fv`; the previous source bypassed that call with a cast.
The restored expression uses the existing conversion operator.

Focused ARM comparison: `Init` remains at **100% source-shape and strict
fuzzy**, linked-exact; `UpdateMeshData` improves from **95.4%
to 99.4% source-shape** and from **82.1% to 91.4% strict fuzzy**. Five
of six functions in this unit remain linked-exact. `UpdateMeshData`
remains nonexact, so its other behavior is not asserted fully recovered.

Native `gof2` and ARM64 static builds pass; ARMv7 compiles **209/209**
translation units. A strict ARM64 shared library links with no undefined
symbols, retains the previous **53 JNI exports**, and is embedded in a
zip-aligned, signed parallel-install APK at
`D:\gof2-arm64-crypto\gof2-arm64-bumpmain-final-poco-clone-debug-20261030.apk`.
The embedded library was verified byte-for-byte against the linked
library, and APK signature verification passes. Play Protect blocked
the earlier clone installation on Poco X3; this final APK has not been
installed or run. Rendering and gameplay remain unverified on device.

The focused ARM report is
`D:\gof2-arm64-crypto\bump-main-focused-final-20261030.json`.
The full ARM corpus compares **4,323/4,524** original functions across
**4,745** pairs, with **94.86% source-shape**, **84.44% strict fuzzy**,
**2,641 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. These rounded totals are unchanged from the preceding
`bump-variants-final-full-20261030.json` snapshot. Comparison of the
complete `(unit, symbol)` exact-match sets shows zero gains and zero
losses for both linked-exact and byte-exact. The new full report is
`D:\gof2-arm64-crypto\bump-main-full-final-20261030.json`.
