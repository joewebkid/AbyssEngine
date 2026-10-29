# Ship 017–019 material links (2026-10-30)

The original Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
forms material IDs from `0x84fa` (34042), then passes each value to the
main and two LOD `ResourceMesh` constructors. The C++ list previously
used zero for all nine links.

| Ship | ARM value flow | Mesh IDs and ARM constructor calls |
| --- | --- | --- |
| 017 | `r8 = 34042 + 0xaf = 34217` at `0xf1aa8–0xf1aac`; stored at `[sp,#0x18c]` at `0xf1bd0` | 17017 `0x1036a8–0x1036b2`; 17151 `0x1036e8–0x1036ec`; 17152 `0x103724–0x103728` |
| 018 | `r6 = 34042 + 0xb0 = 34218` at `0xf1ae4–0xf1ae8`; stored at `[sp,#0x188]` at `0xf1c10` | 17018 `0x1037d4–0x1037de`; 17154 `0x103814–0x103818`; 17155 `0x103850–0x103854` |
| 019 | `r9 = 34042 + 0xb1 = 34219` at `0xf1b26–0xf1b2a`; stored at `[sp,#0x180]` at `0xf1c4c` | 17019 `0x103904–0x10390e`; 17157 `0x103942–0x103946`; 17158 `0x10397c–0x103980` |

The LOD calls pass the material value retained in callee-saved `r4`
after the main-mesh call. Source registrations at slots 648–650 declare
34217, 34218 and 34219 as kind-6 `ResourceMaterial` entries with the
corresponding diffuse and normal texture IDs. Their live rendering
behavior remains unverified.

ARMv7 compiles 209/209 translation units; native and ARM64 static
libraries build. Focused `BuildResourceList` remains **77.0%
source-shape / 20.2% strict fuzzy** and nonexact. The full ARM corpus
compares **4,323/4,524** original functions over **4,745** pairs at
**94.86% source-shape / 84.43% strict fuzzy**, with **2,638 linked-exact**,
**1,086 byte-exact** and zero skips. Comparison against the preceding
`ship12-16-full-20261030.json` shows no lost linked- or byte-exact pair.
Full output is `D:\gof2-arm64-crypto\ship17-19-full-20261030.json`.
Five resource-table regression tests pass.
