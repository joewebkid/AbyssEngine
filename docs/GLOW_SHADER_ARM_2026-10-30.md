# GlowShader ARM shader sources (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`GlowShader::Init` starts at ELF `0x91fc4`. Its PC-relative arguments
to `ES2LoadProgram` are embedded GLSL; the previous source passed
placeholder shader filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1fa040` | 273 | `da2e96b86576a0f5d1687d0190e2b0ec747da2019333567f5fb1de5051b7ddd4` |
| Fragment | `0x1fa152` | 177 | `d369725fe849765f068b4362d3d6a3bffa6e44ffb7fd99884d31d88c00a7dbcd` |

Both C++ GLSL arrays were decoded and compared byte-for-byte against
those ELF ranges. All five queried input names and their order match
the original ARM literals: `a_position`, `a_texCoord`,
`u_ModelViewProjectionMatrix`, `u_ModelMatrix`, and `s_texture`.
`Init` was already linked-exact with incorrect referenced strings;
the code metric did not establish their byte contents.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. Focused verification remains **6/6 linked-exact**,
with **100% source-shape and strict fuzzy**. The strict ARM64 shared
library links without undefined symbols and retains all **53 JNI
exports**. This packet was not installed or run on Poco X3; runtime
shader compilation, rendering, and gameplay remain unverified.
Focused report:
`D:\gof2-arm64-crypto\glow-shader-focused-20261030.json`.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.87%** mean source-shape, **84.46%** strict fuzzy,
**2,650 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Compared with
`D:\gof2-arm64-crypto\pulse-shader-full-20261030.json`, the complete
`(unit, symbol)` sets have zero linked-exact and byte-exact gains or
losses. Full report:
`D:\gof2-arm64-crypto\glow-shader-full-20261030.json`.
