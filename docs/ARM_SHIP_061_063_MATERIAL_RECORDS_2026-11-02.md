# Ships 061–063 mesh and material records (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `BuildResourceList` supplies these material operands to
the `ResourceMesh` constructors, where recovered entries had zero:

| Mesh | Resource ID | Material ID | ARM evidence |
| --- | ---: | ---: | --- |
| 061 main | 17353 | 34261 | `0xec5a8–0xec5c0`: `0x86c5 - 0xf0`, narrowed and passed in `r2`. |
| 062 main | 17359 | 34262 | `0xec620–0xec638`: `0x86c5 - 0xef`. |
| 063 main | 17365 | 34263 | `0xec698–0xec6b0`: `0x86c5 - 0xee`. |
| 063 lights emissive | 17370 | 34363 | `0xec710–0xec728`: `0x86c5 - 0x8a`. |

The kind-6 main material records 34261 and 34262 at slots 692–693
had placeholder IDs and textures. The original constructors at
`0xf24d4–0xf2528` pair 34061/34161 and 34062/34162 with blend 28.
The diffuse and normal/specular texture values were saved when those
textures were registered at `0xee162–0xee19e` and `0xeef2a–0xeef66`.
The corresponding four secondary materials at slots 831–834 reuse
34061 or 34062 with blend 0 and 2, as shown at `0xf3d06–0xf3dbc`.
The 063 main and secondary material records were already present; only
its two mesh operands were missing.

The original Android OBB contains the 061, 062 and 063 diffuse and
normal/specular texture files. Windows native and ARM64 static builds
pass. The strict ARM64 shared link has no unresolved symbols and
retains all 52 Java JNI exports plus `JNI_OnLoad`. Five
resource-material table tests and the `-O0`/`-O2` full resource-table
smoke pass. The original OBB contains all 91 selected platform
texture paths checked by that smoke.

ARMv7 compiles **209/209** translation units. Focused
`BuildResourceList` source-shape remains **77.2%**, while strict fuzzy
declines **20.5% → 18.7%**. Generated instructions change
**51,805 → 51,802** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship061-063-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-056–060 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship061-063-material-full-20261102.json`.

Live rendering of these ships is not yet observed. The unusual
original 34602 light link for ship 060 remains under investigation;
this package does not alter it.
The Redmi package and OBB state also remain unverified because the
device is not currently visible over ADB. Next: resolve 34602 and
inspect the remaining resource-table placeholders or the Redmi runtime
when that device reconnects.

Follow-up on 2026-11-02: [pirate glow material 34602](ARM_PIRATE_GLOW_34602_MATERIAL_2026-11-02.md)
recovers the previously unresolved resource record and its confirmed
pirate mesh links. Live rendering remains unverified.
