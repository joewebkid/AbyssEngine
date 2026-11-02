# Ships 051–055 material records (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`BuildResourceList` had zero material links for the main meshes of
051, 052, 054 and 055, and for the emissive mesh of 054. The original
function passes these 16-bit material IDs:

| Ship | Mesh ID | Material ID | ARM constructor / registration |
| --- | ---: | ---: | --- |
| 051 main | 17293 | 34251 | `0xec0a0–0xec0b8`: `0x84fa + 0xd1`, then `uxth` and `ResourceMesh` construction. |
| 052 main | 17299 | 34252 | `0xec150–0xec168`: `0x84fa + 0xd2`. |
| 054 main | 17311 | 34254 | `0xec200–0xec218`: `0x84fa + 0xd4`. |
| 054 emissive | 17316 | 34354 | `0xec276–0xec292`: literal `0xffff86c5 - 0x93`, narrowed to `0x8632`, then registration. |
| 055 main | 17317 | 34255 | `0xec2b8–0xec2d0`: `0x84fa + 0xd5`. |

The original also registers the following kind-6 material records.
The recovered table had placeholders for most of their IDs or texture
arguments:

| Ship | Main material → diffuse / normal-specular | Secondary materials → diffuse |
| --- | --- | --- |
| 051 | 34251 → 34051 / 34151 | 34351 (blend 0), 32551 (blend 2) → 34051 |
| 052 | 34252 → 34052 / 34152 | 34352 (blend 0), 32552 (blend 2) → 34052 |
| 053 | 34253 → 34053 / 34153 | 34353 (blend 0), 32553 (blend 2) → 34053 |
| 054 | 34254 → 34054 / 34154 | 34354 (blend 0), 32554 (blend 2) → 34054 |
| 055 | 34255 → 34055 / 34155 | 34355 (blend 0), 32555 (blend 2) → 34055 |

Main material constructors and registrations are at `0xf22bc–0xf23ee`.
The 053 pair is computed directly from `0x84fa + 0x0b` and `+ 0x6f`
at `0xf2328–0xf2350`. The other pairs load diffuse and normal/specular
IDs previously registered in the texture runs at `0xedf84–0xee036`
and `0xeed28–0xeedfe`. Secondary materials at `0xf3986–0xf3b28`
reuse the corresponding diffuse ID; blend arguments are `0` and `2`.
The ID arithmetic uses `0x86c5 - 0x96..0x92` for 34351–34355 and
explicit `0x7f27..0x7f2b` for 32551–32555.

The original Android OBB contains one diffuse and one normal/specular
texture for each of 051, 052, 054 and 055. It contains no 053 texture
files, and the original resource sequence has no 053 texture or mesh
registration. The 053 material records nevertheless exist in the ARM
function; this correction reproduces that dangling original table
state without adding fictitious assets. Live use of 053 and rendering
of the corrected ships remain unverified.

Windows native and ARM64 static builds pass. Strict ARM64 shared link
passes with no unresolved symbols and retains all 52 Java JNI exports
plus `JNI_OnLoad`. Five resource-material table tests pass. The full
resource-table smoke passes at `-O0` and `-O2`; its original-OBB check
finds all 91 selected platform texture paths.

ARMv7 compiles **209/209** translation units. Focused
`BuildResourceList` source-shape stays **77.1%** while strict fuzzy
rises **19.6% → 27.0%**. Generated instructions change **51,637 →
51,731** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship051-055-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-046–049 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship051-055-material-full-20261102.json`.

The APK staged on the Redmi predates this change. The separate test
package was not installed at the last device check following Play
Protect's earlier ADB rejection; the Redmi was disconnected at this
checkpoint. Next: trace ships 056–060 and test a freshly packaged APK
after device installation is accepted.
