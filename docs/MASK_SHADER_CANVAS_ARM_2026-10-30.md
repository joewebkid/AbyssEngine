# MaskShader and canvas mask state (2026-10-30)

Primary source: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`MaskShader::Init` starts at ELF `0x9156c`; `UpdateMeshData` at
`0x9165a`. The PC-relative `ES2LoadProgram` arguments are embedded GLSL:

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1f8f80` | 400 | `9b3b0cb4dfbe63b0be5194387fd597c6b1ee784ef8beb0d14287e284a0f96f46` |
| Fragment | `0x1f9111` | 577 | `99fcd78d6cd3d2b9e27f6fb04bf5c08ac4fcc5021d8877bdd03a7155ca4e19ee` |

The C++ arrays were decoded and compared byte-for-byte with these ELF
ranges. All seven queried names match the original order:
`a_position`, `a_texCoord_0`, `a_texCoord_1`, `u_WorldMatrix`,
`s_texture[0]`, `s_texture[1]`, `glColor`. The third attribute is the
mask texture coordinate, not a vertex color. The fragment multiplies
the base color by `glColor` and masks alpha with the second texture.

The original `UpdateMeshData` loads `Engine::appManager` from ARM
`+0x30`, then the application's `PaintCanvas*` from `+0x0`, and then
the active mask image from canvas `+0x20`. The old source skipped the
canvas dereference and read a different object. Original
`PaintCanvas::MeshSet2DMask` at `0x78b30` stores the image pointer at
canvas `+0x20`; `MeshClear2DMask` at `0x78b60` clears that slot. The
former `PaintCanvas` header put `mask2dImage` at `+0x2c`, following a
Ghidra-inferred `int` at `+0x20`; direct ARM stores disprove that field
typing. The 32-bit mask field is now at `+0x20` with an assertion;
the 64-bit host field order was retained. Its measured Windows
64-bit layout remains size 688, mask offset 72, mesh count 56,
meshes 64, orientation 80, and engine 88.

`MaskShader` now reaches the named canvas mask and uses typed
`Image2D::textureId`, `Image2D::mesh`, and `Mesh::texCoords` /
`texCoordVBO`. This removes 32-bit pointer truncation in the mask
path on ARM64 while preserving the original ARMv7 load sequence.

Focused ARM verification: the six `MaskShader` functions rise from
**5/6** to **6/6 linked-exact**, with **100% source-shape and strict
fuzzy**. `PaintCanvas::MeshClear2DMask` rises from **66.7% / 93.3%**
strict fuzzy/source-shape to **byte-exact**. `MeshSet2DMask` rises from
**83.3% / 97.8%** to **88.9% / 98.9%** but remains nonexact; the
remaining diff is instruction scheduling of an index register.
Native `gof2` and ARM64 static builds pass, and ARMv7 compiles
**209/209** units. Focused reports are
`D:\gof2-arm64-crypto\mask-shader-before-20261030.json`,
`D:\gof2-arm64-crypto\mask-shader-after-20261030.json`,
`D:\gof2-arm64-crypto\mask-canvas-before-20261030.json`, and
`D:\gof2-arm64-crypto\mask-canvas-after-20261030.json`.

The full ARM corpus compares **4,323/4,524** originals across
**4,745** pairs: **94.87%** mean source-shape, **84.47%** strict
fuzzy, **2,654 linked-exact**, **1,087 byte-exact**, zero skipped,
and **201 missing** (all absent). Relative to
`D:\gof2-arm64-crypto\green-shader-full-20261030.json`, the
only linked-exact gains are `MaskShader::UpdateMeshData` and
`PaintCanvas::MeshClear2DMask`; the latter is the only byte-exact
gain. No exact symbols were lost. Full report:
`D:\gof2-arm64-crypto\mask-shader-full-20261030.json`.

The current ARM64 game shared library/APK has not been rebuilt with this packet, and the
mask rendering path has not been run on Poco. Runtime texture
ownership and visual behavior remain unverified. Next priority:
inspect `BumpRimCubeShader` source and behavior, then an allowed
phone run.
