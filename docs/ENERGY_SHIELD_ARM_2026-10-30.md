# EnergyShield ARM recovery (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`EnergyShield::Init` starts at ELF `0x8a28c`. Its two PC-relative
arguments to `ES2LoadProgram` resolve to embedded GLSL; the previous
source passed placeholder shader filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1ebe40` | 755 | `b38dcdbb611ea480dc8a31addeab2da1b0cd4c56232a0800680f8f501f25fd0a` |
| Fragment | `0x1ec134` | 791 | `74b283703e3b4b418bec04e0d4f184f44f311a999a197743668be56600ec1145` |

The two C++ GLSL arrays were decoded and compared byte-for-byte with
those ELF ranges. All 14 input query names and their order match the
original PC-relative literals. The third input, `a_normal`, is an
attribute: the original calls `glGetAttribLocation`, while the old
source called `glGetUniformLocation`. The corrected location is used
by vertex attribute enable, pointer setup, and disable calls.

`Init` stores uniform locations at offsets `0x44`, `0x48`, `0x50`,
`0x54`, `0x58`, and `0x5c`. The old C++ class omitted four-byte slots
at `0x40` and `0x4c`, so `UpdateMeshData` could read the wrong fields.
The two slots restore the original layout. The original `UpdateMeshData`
converts display dimensions as signed 32-bit integers before floating
point division; the source now follows that conversion.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. Focused ARM verification improves from **4/6** to
**6/6 linked-exact**, with **100% source-shape and strict fuzzy** for
the whole unit. `Init` and `UpdateMeshData` are the two new exact
matches. Their previous source-shape scores were **90.4%** and
**99.2%**, and previous strict fuzzy scores **86.9%** and **95.6%**.
Focused report:
`D:\gof2-arm64-crypto\energy-shield-focused-20261030.json`.

The strict ARM64 shared library links without undefined symbols and
retains all **53 JNI exports**. A signed, zip-aligned parallel-install
APK at
`D:\gof2-arm64-crypto\gof2-arm64-energyshield-poco-clone-debug-20261030.apk`
contains that library byte-for-byte. It has not been installed or run
because Play Protect blocked earlier clone installations on Poco X3.
Runtime shader compilation, rendering, and gameplay remain unverified.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.87%** mean source-shape, **84.46%** strict fuzzy,
**2,648 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Compared with
`D:\gof2-arm64-crypto\bump-particle-full-20261030.json`, the exact
`(unit, symbol)` sets gained precisely `Init` and `UpdateMeshData` in
this unit and lost no linked-exact or byte-exact members. Full report:
`D:\gof2-arm64-crypto\energy-shield-full-20261030.json`.
