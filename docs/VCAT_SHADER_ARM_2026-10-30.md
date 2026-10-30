# VertexColorAlphaTextureShader (VCAT) ARM sources (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`VertexColorAlphaTextureShader::Init` starts at ELF `0x91b6c`. Its
PC-relative `ES2LoadProgram` arguments are embedded GLSL; the old
source passed placeholder VCAT filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1f9ad0` | 725 | `037c06d5f6e41fc451f4bf3ac18fde234dba612d72e3f405e3c25fca4870185a` |
| Fragment | `0x1f9da6` | 615 | `b98ad00d260dd96d27741c2bae34ab5603b48092daaadd1e7a39f8868edb90e4` |

Both C++ GLSL arrays were decoded and compared byte-for-byte against
those ELF ranges. The fragment contains UTF-8 bytes `c3 b6` inside a
comment; fixed-width octal escapes preserve them without relying on
the C++ source-file encoding. All 15 queried input names and their
order match the original ARM literals, including `a_vertexColor` for
the color attribute. `Init` was already linked-exact with incorrect
referenced strings, so code matching did not verify those contents.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. Focused verification remains **6/6 linked-exact**,
with **100% source-shape and strict fuzzy**. The strict ARM64 shared
library links without undefined symbols and retains all **53 JNI
exports**. A signed, zip-aligned parallel-install APK at
`D:\gof2-arm64-crypto\gof2-arm64-vcat-shader-poco-clone-debug-20261030.apk`
contains that library byte-for-byte. It has not been installed or run
on Poco X3; runtime shader compilation, rendering, and gameplay remain
unverified. Focused report:
`D:\gof2-arm64-crypto\vcat-shader-focused-20261030.json`.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.87%** mean source-shape, **84.46%** strict fuzzy,
**2,650 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Compared with
`D:\gof2-arm64-crypto\glow-shader-full-20261030.json`, the complete
`(unit, symbol)` sets have zero linked-exact and byte-exact gains or
losses. Full report:
`D:\gof2-arm64-crypto\vcat-shader-full-20261030.json`.
