# TextureConference ARM recovery (2026-10-30)

Primary source: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`TextureConference::Init` starts at ELF `0x8cb98`; `UpdateMeshData` at
`0x8cc4a`. The PC-relative arguments to `ES2LoadProgram` point to these
embedded GLSL sources:

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1f1220` | 505 | `27a3d6e4b6f5d4c2169d8236514417d1b754180ec9de4eed20d87c5359f0eedd` |
| Fragment | `0x1f141a` | 449 | `9de74d6a3557f81e2a91da385261b9c44960fac0cf4f4869fcb74ed2916fdda4` |

The C++ arrays were decoded and compared byte-for-byte with the ELF. The
original six queried names, in call order, are `s_texture`, `a_position`,
`a_texCoord`, `u_WorldMatrix`, `u_time`, and `glColor`. The original stores
their locations at ARM object offsets `+0x34`, `+0x20`, `+0x24`, `+0x2c`,
`+0x28`, and `+0x30` respectively. The old placeholders and field order
would leave the GLSL uniforms unresolved and associate the time and color
locations with the wrong fields. The recovered header and uses follow the original
layout and GLSL types.

The original `UpdateMeshData` compares the 64-bit accumulated time with
`0xe10` and subtracts it only when the value is strictly greater. The
previous source also subtracted at equality. Rewriting that condition as
`(t > 0xe10) ? (t - 0xe10) : t` restores the boundary behavior and the
original ARM function shape. The boundary cases `3599`, `3600`, `3601`
were checked as `3599`, `3600`, `1`.

Focused ARM verification of six functions changed from **2/6** to
**4/6 linked-exact**. `Init` and `UpdateMeshData` both became
linked-exact; the focused mean source-shape changed from **99.5%** to
**100.0%** and strict fuzzy from **96.1%** to **98.8%**. Native `gof2`
and ARM64 static builds pass, and ARMv7 compiled **209/209** units.
Focused reports: `D:\gof2-arm64-crypto\texture-conference-before-20261030.json`
and `D:\gof2-arm64-crypto\texture-conference-final-20261030.json`.

The full ARM corpus compares **4,323/4,524** originals across
**4,745** pairs: **94.87%** mean source-shape, **84.47%** strict
fuzzy, **2,652 linked-exact**, **1,086 byte-exact**, zero skipped, and
**201 missing** (all absent). Relative to
`D:\gof2-arm64-crypto\vcat-shader-full-20261030.json`, the only
linked-exact gains are `Init` and `UpdateMeshData`; there are no exact
losses. Full report:
`D:\gof2-arm64-crypto\texture-conference-full-20261030.json`.

The shader has not been run on Poco; runtime GLSL compilation, visual
output, and gameplay remain unverified. Next priority: another shader with placeholder sources.