# SpecCubeMapping and SpecCubeAlphaMapping ARM shader sources (2026-10-30)

Primary evidence: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `SpecCubeMapping::Init` at ELF `0x9286c` first calls
`LoadBindShader` with `data/shader/SpecCubeMapping.vs` and
`data/shader/SpecCubeMapping.fs`.
The original `SpecCubeAlphaMapping::Init` at `0x87a10` similarly uses
`data/shader/SpecCubeAlphaMapping.vs` and
`data/shader/SpecCubeAlphaMapping.fs`. When file loading
fails, each passes embedded GLSL to `ES2LoadProgram`. The prior source
used short placeholder filenames for both paths.

| Shader | Vertex ELF address, bytes, SHA-256 | Fragment ELF address, bytes, SHA-256 |
| --- | --- | --- |
| SpecCubeMapping | `0x1fa8f0`, 1,777, `befc82c5f8f0e70724c5f2e26ab7d29eb2b69c5855c51687d4d8f653a32376f4` | `0x1fafe2`, 1,231, `eebc2e0bb83335ad2c44fc81171ba2d92692d540c1d354f8cb0dfa50af70a104` |
| SpecCubeAlphaMapping | `0x1e7dc0`, 1,539, `060cdebbef081b36f04eb34fa51dba363170bf828ef133bb843dd392a221756b` | `0x1e83c4`, 726, `a41c0500c75f2a186c1b4695d93913982a4c8241c8ae0478e1a072baf1c409e4` |

All four C++ string arrays were decoded and compared byte-for-byte
with those original ELF ranges. Each `Init` queries three attributes
and twelve uniforms; all 15 names for each shader were checked against
the original PC-relative literal order. This includes
`u_envValue` only in `SpecCubeMapping` and `u_cubeMapFactor` only in
`SpecCubeAlphaMapping`.

Focused ARM verification remains **6/6 linked-exact**, **100%
source-shape**, and **100% strict fuzzy** for each unit. These values
were already 100% before the edit: the code comparison does not prove
equality of referenced file paths, GLSL, or query-name bytes. The
separate string checks above establish that part. Native `gof2` and
ARM64 static builds pass; ARMv7 compiles **209/209** translation units.
The strict ARM64 shared library links without undefined symbols and
retains the preceding **53 JNI exports**. A signed, zip-aligned
parallel-install APK at
`D:\gof2-arm64-crypto\gof2-arm64-speccube-family-poco-clone-debug-20261030.apk`
contains that library byte-for-byte and its signature verifies. It
has not been installed or run because Play Protect blocked earlier
clone installations on Poco X3. Runtime shader compilation, rendering,
and gameplay remain unverified on device.

Focused reports:
`D:\gof2-arm64-crypto\speccube-mapping-focused-20261030.json` and
`D:\gof2-arm64-crypto\speccube-alpha-focused-20261030.json`.
The full ARM corpus compares **4,323/4,524** original functions across
**4,745** pairs, with **94.87%** mean source-shape, **84.45%** strict
fuzzy, **2,644 linked-exact**, **1,086 byte-exact**, zero skipped, and
201 missing originals. Against the preceding `bump-cloak-full-20261030.json`
snapshot, complete `(unit, symbol)` sets have zero linked-exact or
byte-exact gains and losses. The code metrics remain unchanged because
the repaired contents are referenced strings. The fresh full report is
`D:\gof2-arm64-crypto\speccube-family-full-20261030.json`.
