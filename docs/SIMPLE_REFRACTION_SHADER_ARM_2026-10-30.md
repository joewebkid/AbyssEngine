# SimpleRefractionShader ARM recovery (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`SimpleRefractionShader::Init` starts at ELF `0x8c74c`. The two
PC-relative arguments to `ES2LoadProgram` are embedded GLSL, not the
placeholder filenames previously passed by the source.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1f0bc0` | 774 | `1b709dd6e304dbf2460997b2bd6afa248a01dc3bee6802de9322780aabee59e0` |
| Fragment | `0x1f0ec7` | 803 | `c74ac42b249bf78f084c7826bfecc314deaa766efeaced2253e81d626b396541` |

Both C++ arrays were decoded and compared byte-for-byte against those
ELF ranges. All 14 input query names and their order match the
original PC-relative literals. The fifth input is the attribute
`a_bitangent`: the old source queried it as a uniform. The old field
names also called the texcoord location `aNormalLoc`, the normal
location `aTangentLoc`, and the tangent location `aTexCoordLoc`.
Correct names now express the actual ARM query and mesh-buffer pairing
without changing the original class offsets.

The original `Init` iterates over two adjacent sampler-location fields.
The old local array copied them onto the stack and caused different
code and stack protection. The source now iterates over the adjacent
fields. The original `UpdateMeshData` converts display width and height
through signed 32-bit integers before floating point division; the
source now follows that conversion.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. Focused ARM verification improves from **4/6** to
**6/6 linked-exact**, with **100% source-shape and strict fuzzy** for
the whole unit. `Init` and `UpdateMeshData` are the two new exact
matches. Their previous source-shape scores were **88.3%** and
**99.6%**, and previous strict fuzzy scores **75.3%** and **98.9%**.
Focused report:
`D:\gof2-arm64-crypto\simple-refraction-focused-20261030.json`.

The strict ARM64 shared library links without undefined symbols and
retains all **53 JNI exports**. A signed, zip-aligned parallel-install
APK at
`D:\gof2-arm64-crypto\gof2-arm64-simple-refraction-poco-clone-debug-20261030.apk`
contains that library byte-for-byte. It has not been installed or run
because Play Protect blocked earlier clone installations on Poco X3.
Runtime shader compilation, rendering, and gameplay remain unverified.

The full ARM corpus compares **4,323/4,524** original functions across
**4,745** pairs: **94.87%** mean source-shape, **84.46%** strict fuzzy,
**2,650 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Against
`D:\gof2-arm64-crypto\energy-shield-full-20261030.json`, the exact
`(unit, symbol)` sets gained precisely `Init` and `UpdateMeshData` in
this unit and lost no linked-exact or byte-exact members. Full report:
`D:\gof2-arm64-crypto\simple-refraction-full-20261030.json`.
