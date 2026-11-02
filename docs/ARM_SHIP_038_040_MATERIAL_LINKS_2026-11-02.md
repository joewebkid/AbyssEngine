# Ships 038–040 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Five `ResourceMesh` calls in `BuildResourceList` passed zero or 65535
where the original passes registered ship materials:

| Ship | Mesh IDs | Original material | ARM value flow |
| --- | --- | ---: | --- |
| 038 | 17216 | 34238 | `0xf1fde`: `r11 = 0x84fa + 0xc4`; `0xf20ea`: save to `[sp, #0x1bc]`; `0x104f08–0x104f10`: load, narrow and pass to the main constructor. |
| 039 | 17221 / 17222 / 17223 | 34239 | `0xf201e`: `r1 = 0x84fa + 0xc5`; `0xf2020`: save to `[sp, #0x1c4]`; `0x104fc0–0x104fca`: load, narrow and pass to the main constructor; `0x105000` and `0x10503c`: reuse for both LODs. |
| 040 | 17227 | 34240 | `0xf205a`: `r1 = 0x84fa + 0xc6`; `0xf205c`: save to `[sp, #0x118]`; `0x1050f0–0x1050f8`: load, narrow and pass to the main constructor. |

The following instructions register these mesh IDs. Kind-6 material
records 34238–34240 are present in the original registration sequence
and the recovered table. The six matching diffuse/normal-specular
texture files each occur once in the original Android OBB. Lights and
engine meshes use separate values 32538–32540 and were not changed.
These are source-backed registration corrections. Live material loading
and rendering remain unverified.

Windows native and ARM64 static builds pass. The strict ARM64 shared
library links without undefined symbols and retains all 52 Java JNI
exports plus `JNI_OnLoad`. ARMv7 compiles **209/209** translation
units. Seven resource/ship table tests pass. The full resource-table
smoke passes at `-O0` and `-O2`; its original-OBB check finds all 91
selected platform texture paths.

Focused `BuildResourceList` source-shape stays **77.0%** and strict
fuzzy changes **21.0% → 20.2%**. Generated instructions change
**51,571 → 51,583** versus 60,817 in the original. The function
remains nonexact. The metric decrease does not override the direct
ARM evidence for these five IDs. Focused report:
`D:/gof2-arm64-crypto/ship038-040-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-035–037 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship038-040-material-full-20261102.json`.

The APK staged on the Redmi contains the earlier ships-032–034
revision, and its separate test package was still not installed at
this checkpoint. The former player-ship crash and frame rendering
remain open runtime checks. Next: trace ships 041–043, then test an
APK built from current source.
