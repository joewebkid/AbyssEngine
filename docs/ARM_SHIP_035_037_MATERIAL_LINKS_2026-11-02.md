# Ships 035–037 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Seven main/LOD `ResourceMesh` calls in `BuildResourceList` had zero or
65535 where the original passes a registered material ID:

| Ship | Mesh IDs | Original material | ARM value flow |
| --- | --- | ---: | --- |
| 035 | 17035 / 17205 / 17206 | 34235 | `0xf1f1c`: `r6 = 0x84fa + 0xc1`; `0xf1f88`: save to `[sp, #0x11c]`; `0x104bf4–0x104bfe`: load, narrow and pass to the main constructor; `0x104c34` and `0x104c70`: reuse for both LODs. |
| 036 | 17036 / 17208 / 17209 | 34236 | `0xf1f5a`: `r10 = 0x84fa + 0xc2`; `0xf2084`: save to `[sp, #0x1d4]`; `0x104d24–0x104d2e`: load, narrow and pass to the main constructor; `0x104d64` and `0x104da0`: reuse for both LODs. |
| 037 | 17211 | 34237 | `0xf1fa2`: `r6 = 0x84fa + 0xc3`; `0xf2008`: save to `[sp, #0x1cc]`; `0x104e50–0x104e58`: load, narrow and pass to the main constructor. |

The following instructions register the listed mesh IDs. Kind-6
material records 34235, 34236 and 34237 occur in the original list
and the recovered table. Ship 037's lights and engine meshes pass
32537 (`0x7f19` at `0x104e90` and `0x104ecc`), which was already
represented in the source. The other engine and light meshes also use
separate arguments; none were changed. This corrects registration
data. The six diffuse/normal-specular files for these materials each
occur once in the original Android OBB. Live material loading and
rendering remain unverified.

Windows native and ARM64 static builds pass. The strict ARM64 shared
library links without undefined symbols and preserves all 52 Java
JNI exports plus `JNI_OnLoad`. ARMv7 compiles **209/209** translation
units. Seven resource/ship table tests pass. The full resource-table
smoke passes at `-O0` and `-O2`; its original-OBB check finds all 91
selected platform texture paths.

Focused `BuildResourceList` source-shape stays **77.0%** and strict
fuzzy rises **19.3% → 21.0%**. Generated instructions change **51,539 →
51,571** versus 60,817 in the original. The function remains nonexact. Focused
report: `D:/gof2-arm64-crypto/ship035-037-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-032–034 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship035-037-material-full-20261102.json`.

The APK staged on the Redmi contains the preceding ships-032–034
revision, and its separate test package was still not installed at
this checkpoint. The former player-ship crash and frame rendering
remain open runtime checks. Next: trace ships 038–040, then test an APK
built from current source.
