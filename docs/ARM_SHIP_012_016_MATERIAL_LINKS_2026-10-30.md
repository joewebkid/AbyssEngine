# Ship 012 and 016 material links (2026-10-30)

The original Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
forms the material IDs from `r11 = 0x84fa` (34042) at `0xf19cc`.

| Ship | ARM material value | Mesh resource IDs | Constructor evidence |
| --- | ---: | --- | --- |
| 012 | 34212 = 34042 + `0xaa` at `0xf1a3c`; saved at `[sp,#0x19c]` at `0xf1b14` | 17012, 17136, 17137 | main load/call `0x103448–0x103452`; retained `r4` at `0x103488` and `0x1034c4` |
| 016 | 34216 = 34042 + `0xae` at `0xf1a74`; saved at `[sp,#0x194]` at `0xf1b90` | 17016, 17148, 17149 | main load/call `0x103576–0x103580`; retained `r4` at `0x1035b8` and `0x1035f4` |

The registrations store those mesh IDs immediately after the listed
constructor calls: `0x103460`, `0x10349c`, `0x1034d8` for ship 012 and
`0x103590`, `0x1035cc`, `0x103608` for ship 016. The C++ table had zero
for all six material links. It now passes 34212 or 34216. Each ID has
one kind-6 `ResourceMaterial` registration, with texture pairs
34012/34112 and 34016/34116 respectively.

ARMv7 compiles 209/209 translation units; native and ARM64 static
libraries build. Focused `BuildResourceList` remains **77.0%
source-shape** and improves **17.8% → 20.2% strict fuzzy**; the function
remains nonexact. The full ARM corpus compares **4,323/4,524** originals
over **4,745** pairs at **94.86% source-shape / 84.43% strict fuzzy**,
with **2,638 linked-exact**, **1,086 byte-exact** and zero skips. The
linked- and byte-exact sets have no lost pair relative to
`ship11-full-20261030.json`. Full output is
`D:\gof2-arm64-crypto\ship12-16-full-20261030.json`. Five resource-table
regression tests pass. Live rendering of these ships is not yet verified.
