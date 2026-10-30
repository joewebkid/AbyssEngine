# ColorMixAdd ARM shader sources (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ColorMixAdd::Init` starts at ELF `0x921c8`. Its PC-relative arguments
to `ES2LoadProgram` are embedded GLSL; the previous source passed
placeholder filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1fa230` | 496 | `c9ba168f2bb17f9ab2e2b886bbac820f41b7da91fc48709ac5a064d215cea4b8` |
| Fragment | `0x1fa421` | 523 | `ecd0c2e6b641c5ee9decbc7c9b2760104326db6e65b1d49ffdd3a5076df73f9c` |

Both C++ GLSL arrays were decoded and compared byte-for-byte against
those ELF ranges. All eight queried input names and their order match
the original PC-relative literals: `s_texture`, `a_position`,
`a_texCoord`, `u_WorldMatrix`, `glColor`, `u_DarkenValue`, `u_UvMatrix`,
and `u_UVAnimation`. `Init` was already linked-exact before this edit,
despite passing incorrect strings: the code verifier does not compare
bytes at referenced string addresses.

Original `SetInActive` disables both attributes twice with a positive
counter descending from two. The current source has the same calls
and behavior but Clang emits a negative counter ascending to zero.
Changing the counter type to unsigned did not alter that code shape,
so the source retains the existing simple loop. Focused verification
remains **5/6 linked-exact**, with **98.9% source-shape** and **97.4%
strict fuzzy** on average. `SetInActive` is **93.3% source-shape** and
**84.6% strict fuzzy**; the other five functions remain linked-exact.
Focused report:
`D:\gof2-arm64-crypto\color-mix-focused-20261030.json`.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. The strict ARM64 shared library links without
undefined symbols and retains all **53 JNI exports**. This packet was
not installed or run on Poco X3. Runtime shader compilation, rendering,
and gameplay remain unverified.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.87%** mean source-shape, **84.46%** strict fuzzy,
**2,650 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Compared with
`D:\gof2-arm64-crypto\simple-refraction-full-20261030.json`, the
complete `(unit, symbol)` sets have zero linked-exact and byte-exact
gains or losses. Full report:
`D:\gof2-arm64-crypto\color-mix-full-20261030.json`.
