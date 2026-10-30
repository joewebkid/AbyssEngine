# BumpRimCubeShader ARM recovery (2026-10-30)

Primary source: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The old engine shader's `Init` starts at ELF `0x8fb60`; `UpdateMeshData`
starts at `0x8fe0c`. This is distinct from the game shader
`BumpRimCubeShader_new`.

| Embedded source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1f5020` | 2,145 | `a258f34aef5c7c07fd59d3568b03331c2a703712c716ed964dbe8c3660345f76` |
| Fragment | `0x1f5882` | 2,811 | `61e21fe7ddecad878bef02742a5a3511d1da13225c42407fd5cb9a4c9c55bd3f` |

The embedded GLSL replaces filenames previously passed to
`ES2LoadProgram`; the file-backed path is now the original
`data/shader/BumpRimCubeShader.vs` / `.fs`. Query strings and their order
follow ARM PC-relative references: four attributes, then `a_bitangent`
queried as a uniform in the original, followed by 25 more uniforms.
The names and field assignments cover both texture samplers, cubemap,
model matrices, two lights, material, rim, glow, and fog. The two texture
samplers are assigned units 0 and 1 in the original two-iteration loop.

The ARM field stores show that the locations at `+0x60`, `+0x68`, and
`+0x70` are the second ambient, diffuse, and specular colors. Locations
`+0x78` and `+0x84` are rim and glow. The final location at `+0x94` is
the fog enable flag. The header now names those offsets accordingly.
Original `UpdateMeshData` reads the engine's global diffuse and normal
LOD biases, and its fog enable byte; the old code read three constant
zero placeholders. ARM calls at `0x8fec0` and `0x8fed0` respectively
upload `Engine::field_0x2fc` and `lightDiffuseShaded`; the former upload
was absent. The fog color call converts `Engine::fogColor` through
`Vector::operator float*`, as in the original.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles 209/209
translation units. Focused ARM verification improves from 4/6 to
**5/6 linked-exact**, with **100% mean source-shape** and **99.5% strict
fuzzy**. `UpdateMeshData` is now linked-exact; `Init` has the same 258
instructions and 100% source-shape but differs in a small instruction
scheduling span around a location query. Focused report:
`D:\gof2-arm64-crypto\bump-rimcube-after-20261030.json`.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.88%** mean source-shape, **84.48%** strict fuzzy,
**2,655 linked-exact**, **1,087 byte-exact**, zero skipped, and
**201 missing** (all absent). Against
`D:\gof2-arm64-crypto\mask-shader-full-20261030.json`, the only
linked-exact gain is `BumpRimCubeShader::UpdateMeshData`; no linked- or
byte-exact symbols were lost. Full report:
`D:\gof2-arm64-crypto\bump-rimcube-full-20261030.json`.

The ARM64 game shared library/APK has not been rebuilt with this packet.
Shader compilation and visual behavior on Poco remain untested until
the phone is available again.
