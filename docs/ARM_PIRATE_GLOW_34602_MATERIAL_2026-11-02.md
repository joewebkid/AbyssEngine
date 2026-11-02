# Pirate glow material 34602 (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA's bound binary session reports this exact original path and digest.

The recovered resource table had a placeholder at slot 630, between
materials 34601 and 34603. The original `BuildResourceList` constructs
**kind-6 material 34602 with texture 34502 and blend 2**:

- At `0xec568`, `r11 = 0x8728 + 2 = 34602`; `0xef324` preserves it at
  `[sp, #0x100]` until the material registration.
- At `0xecdfc–0xece00`, the registered glow texture ID `34502` is
  saved at `[sp, #0x2ac]`.
- At `0xf168a–0xf16a2`, the constructor receives the saved texture ID
  and blend 2; the resulting resource is registered with the saved
  material ID. The adjacent records 34600/34601/34603 keep their
  existing values.

The same original material ID is passed to these `ResourceMesh`
constructors where recovered entries incorrectly used 34811:

| Asset | Engine/add mesh | Lights/add mesh | ARM constructor evidence |
| --- | ---: | ---: | --- |
| Pirate ship 002 | 18002 | 18702 | `0x102928`, `0x102964` |
| Pirate ship 011 | 18011 | 18711 | `0x1033d0`, `0x10340c` |
| Pirate ship 023 | 18023 | 18723 | `0x103e78`, `0x103eb4` |
| Pirate ship 024 | 18024 | 18724 | `0x103fa8`, `0x103fe4` |
| Pirate ship 025 | 18025 | 18725 | `0x1040d8`, `0x104114` |
| Pirate ship 029 | 18029 | 18729 | `0x104594`, `0x1045d0` |
| Pirate ship 032 | 18032 | 18732 | `0x104920`, `0x10495c` |
| Pirate station | — | 14245 | `0x1055aa` |

Each call passes `r11` as the material operand; `0x10255a` reloads
34602 from the earlier `[sp, #0x50]` save and no intervening write to
`r11` was found before these calls. The previously restored Supernova
ship 060 lights mesh (18823) also passes 34602 at `0xec564–0xec580`.
Other meshes that still use 34811 are outside this confirmed group.

The original Android OBB contains the registered
`glow_pirates.aei` texture once and all **15/15** listed pirate mesh
files. Windows native and ARM64 static builds pass. The strict ARM64
shared link has no unresolved symbols and retains all 52 Java JNI
exports plus `JNI_OnLoad`. Five resource-material table tests and the
`-O0`/`-O2` full resource-table smoke pass; its original-OBB check
finds all 91 selected platform texture paths. Live material creation
and rendering on the Redmi are still unverified while that device is
absent from ADB.

ARMv7 compiles **209/209** translation units. Focused
`BuildResourceList` source-shape remains **77.2%** and strict fuzzy
remains **18.7%**. Generated instructions change **51,802 → 51,826**
versus 60,817 in the original; the function remains nonexact. Focused
report: `D:/gof2-arm64-crypto/pirate-glow-34602-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-061–063 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/pirate-glow-34602-full-20261102.json`.

Next: trace the adjacent material 34605 placeholder and remaining
material links individually against ARM. The Redmi resource-download
screen needs its installed package, OBB permissions and process log
checked over ADB when the device reconnects.
