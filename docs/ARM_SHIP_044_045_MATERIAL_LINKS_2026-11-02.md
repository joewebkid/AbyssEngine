# Ships 044–045 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Four `ResourceMesh` registrations in `BuildResourceList` passed zero as
their material ID. The original function supplies:

| Ship | Mesh IDs | Material ID | ARM value flow |
| --- | --- | ---: | --- |
| 044 | 17251 / 17252 / 17253 | 34244 | `0xebbfe–0xebc06`: `0x84fa + 0xca`, narrowed and passed to the main mesh constructor; `0xebcae–0xebcc0`: the same value for LOD 1; `0xebcf4–0xebcf8`: reuse for LOD 2. |
| 045 | 17257 | 34245 | `0xebd2c–0xebd3c`: `0x84fa + 0xcb`, narrowed and passed to the main mesh constructor. |

The corresponding mesh IDs are registered at `0xebc14`, `0xebccc`,
`0xebd04` and `0xebd48`. The original kind-6 material records for
34244 and 34245 occur later at `0xf2154` and `0xf218a`; they were
already present in the recovered table with textures 34044/34144 and
34045/34145. All four diffuse and normal/specular texture files occur
once in the original Android OBB. Lights and engine meshes use the
separate material IDs 32544 and 32545 and remain unchanged.

Windows native and ARM64 static builds pass. The strict ARM64 shared
library links without undefined symbols and retains all 52 Java JNI
exports plus `JNI_OnLoad`. Seven resource/ship table tests pass. The
full resource-table smoke passes at `-O0` and `-O2`; its original-OBB
check finds all 91 selected platform texture paths.

ARMv7 compiles **209/209** translation units. Focused
`BuildResourceList` source-shape stays **77.0%**, while strict fuzzy
changes **20.2% → 18.8%**. Generated instructions change **51,589 →
51,595** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship044-045-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-041–043 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship044-045-material-full-20261102.json`.

A separately packaged ARM64 APK with this source revision is staged at
`/sdcard/Download/gof2-arm64-current-test.apk` on the Redmi. Its
signature and embedded library hash were verified. An earlier ADB
installation attempt was rejected with
`INSTALL_FAILED_VERIFICATION_FAILURE` after Play Protect verification;
the separate test package was not installed at this checkpoint. The
user's resource-download screen was from the installed original GOF2
package. Live loading and rendering of the newly linked meshes remain
unverified. Next: trace Supernova ships 046–049 and test the current
APK after manual installation is accepted on the device.
