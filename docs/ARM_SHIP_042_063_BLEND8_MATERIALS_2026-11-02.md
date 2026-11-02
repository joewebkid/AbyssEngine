# Ships 042–063 blend-8 material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Seventeen kind-6 materials in the noncontiguous **34442–34463** block
had texture 65535 in the reconstruction. The original single-texture
constructors pass these arguments with blend value 8:

| Materials | Texture arguments | ARM chain |
| --- | --- | --- |
| 34443 | 34043 | `0xeddaa → 0xf2110 → 0xf2b78` |
| 34446–34447 | 34046–34047 | `0xede5e/0xeded2 → 0xf21b4/0xf21e8 → 0xf2bfc/0xf2c28` |
| 34449 | 34049 | `0xedf0e → 0xf2252 → 0xf2c82` |
| 34450 | 34050 | `0xf227e/0xf228a → 0xf2cae` |
| 34451–34462 | 34051–34062 | `0xedf84–0xee19e → 0xf22c4–0xf2510 → 0xf2cda–0xf2ebe` |

The texture registration run derives its IDs from `r8 = 0x84fa`
(34042). The later material constructors narrow or directly form the
corresponding IDs; the blend-8 registration run derives material IDs
from `r8 = 0x86c5` (34501). The stack values were traced to their
last writes before the consumer. Materials 34442, 34444, 34445,
34448 and 34463 already had their observed argument forms and are
unchanged.

The original OBB contains the files for **15/17** changed texture
links. IDs **34050 and 34053** have no texture registrations in the
original resource run or the reconstructed table, and the OBB has no
`ship_050_` or `ship_053_` paths. Their material arguments nevertheless
appear explicitly in the original constructor sequence. This records
an original dangling reference; it does not establish that those
materials can render.

Windows native and ARM64 static builds pass. Five resource-material
table tests and the full resource-table smoke at `-O0`/`-O2` pass;
its original-OBB check finds all 91 selected platform texture paths.
Focused ARMv7 verification compiles **209/209** translation units.
`BuildResourceList` remains nonexact at **77.3%** source-shape;
strict fuzzy changes **19.0% → 18.0%** and generated instructions
change **51,883 → 51,853** against 60,817 original instructions.
The links are retained for direct ARM behavior evidence, not because
they improve the fuzzy score.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.20%** mean source-shape and **85.96%** mean
strict fuzzy, with **2,741 linked-exact**, **1,096 byte-exact**, zero
skipped and 201 missing. The pair set and both exact sets are
identical to the preceding ships-016–036 baseline: zero lost pairs
or exact matches.
Live rendering and the resource lookup behavior for 34050/34053
remain unverified.
