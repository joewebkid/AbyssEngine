# Ships 041–043 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Nine main/LOD `ResourceMesh` calls in `BuildResourceList` previously
passed zero as their material ID. The original function supplies:

| Ship | Main / LOD mesh IDs | Material ID | ARM value flow |
| --- | --- | ---: | --- |
| 041 | 17233 / 17234 / 17235 | 34241 | `0xf209c`: `r6 = 0x84fa + 0xc7`; `0xf20c8`: save to `[sp, #0x1b4]`; `0x1051a8–0x1051b2`: load, narrow and pass to main constructor; `0x1051e6` and `0x105220`: reuse for LODs. |
| 042 | 17239 / 17240 / 17241 | 34242 | `0xf20e6`: `r8 = 0x84fa + 0xc8`; `0xf224a`: save to `[sp, #0x1a0]`; `0x1052d4–0x1052de`: load, narrow and pass to main constructor; `0x105314` and `0x105350`: reuse for LODs. |
| 043 | 17245 / 17246 / 17247 | 34243 | `0xf211a`: `r1 = 0x84fa + 0xc9`; `0xf2120`: save to `[sp, #0x1a8]`; `0x1053c8–0x1053d2`: load, narrow and pass to main constructor; `0x105408` and `0x105444`: reuse for LODs. |

The following instructions register the listed mesh IDs. Kind-6
material records 34241–34243 occur in the original registration
sequence and the recovered table. Their six diffuse/normal-specular
texture files each occur once in the original Android OBB. Lights
meshes use separate values 32541–32543 and were not changed. These
are source-backed registration corrections. Live material loading and
rendering remain unverified.

Windows native and ARM64 static builds pass. The strict ARM64 shared
library links without undefined symbols and retains all 52 Java JNI
exports plus `JNI_OnLoad`. ARMv7 compiles **209/209** translation
units. Seven resource/ship table tests pass. The full resource-table
smoke passes at `-O0` and `-O2`; its original-OBB check finds all 91
selected platform texture paths.

Focused `BuildResourceList` source-shape stays **77.0%** and strict
fuzzy stays **20.2%**. Generated instructions change **51,583 →
51,589** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship041-043-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-038–040 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship041-043-material-full-20261102.json`.

The APK staged on the Redmi contains the earlier ships-032–034
revision, and its separate test package was still not installed at
this checkpoint. The former player-ship crash and frame rendering
remain open runtime checks. Next: trace Supernova ships 044–045,
including their newly recovered material records, then test an APK
built from current source.
