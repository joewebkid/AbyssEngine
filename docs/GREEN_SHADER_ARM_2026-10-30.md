# GreenShader ARM source recovery (2026-10-30)

Primary source: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`GreenShader::Init` starts at ELF `0x882a0`. Its PC-relative
`ES2LoadProgram` arguments are embedded GLSL, rather than the old
placeholder filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1e9070` | 201 | `872de6bce52990e1da7a47e017095852cd13c4ffa864c2bc6c4ce2f1db05dc5f` |
| Fragment | `0x1e913a` | 283 | `e017c8e01069f979abd6b28e7a3a13f6e60c11c5df5c50447055e6f72c5662d6` |

The reconstructed C++ arrays were decoded and compared byte-for-byte
with those ELF ranges. The fragment is byte-identical to the separately
embedded `PulseShader` fragment at `0x1fa7a8`. Fourteen queried names
follow the original call order: five attributes `a_position`, `a_texCoord`,
`a_normal`, `a_tangent`, `a_bitangent`; then nine uniforms
`u_ModelViewProjectionMatrix`, `ModelMatrix`, `lightdirmodel`,
`u_eyeposmodel`, `s_texture`, `glColor`, `u_AmbientColor`,
`u_DiffuseColor`, `u_SpecularColor`. The apparently truncated
`ModelMatrix` and `lightdirmodel` names are the exact PC-relative
string addresses used by the original ARM instructions, so they were
not expanded speculatively.

The fragment declares `u_timevalue`, but `Init` does not query it. Its
`v_texCoord` input is also not assigned by this vertex source. These
are observed mismatches in the original embedded sources; their
runtime shader-link and rendering behavior needs device confirmation.
No guessed fix was made.

Native `gof2` and ARM64 static builds pass, and ARMv7 compiled
**209/209** translation units. Focused verification remains
**6/6 linked-exact**, **100% mean source-shape**, and **100% strict
fuzzy** before and after. Because the verifier compares code but not
referenced GLSL bytes, those unchanged code metrics alone cannot
validate the shader contents. Focused reports:
`D:\gof2-arm64-crypto\green-shader-before-20261030.json` and
`D:\gof2-arm64-crypto\green-shader-after-20261030.json`.

The full ARM corpus compares **4,323/4,524** originals across
**4,745** pairs: **94.87%** mean source-shape, **84.47%** strict
fuzzy, **2,652 linked-exact**, **1,086 byte-exact**, zero skipped,
and **201 missing** (all absent). Compared with
`D:\gof2-arm64-crypto\texture-conference-full-20261030.json`, the
complete `(unit, symbol)` linked-exact and byte-exact sets are
unchanged. Full report:
`D:\gof2-arm64-crypto\green-shader-full-20261030.json`.

Poco runtime shader compilation, rendering, and gameplay remain
unverified. Next priority:
`MaskShader` placeholder sources, then an allowed phone run.
