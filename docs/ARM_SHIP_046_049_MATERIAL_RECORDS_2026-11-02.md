# Ships 046–049 mesh and material records (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Four main `ResourceMesh` calls passed zero as the material ID. Four
corresponding kind-6 `ResourceMaterial` records were absent or
incorrect in the recovered table. The original `BuildResourceList`
value flow supplies:

| Ship | Main mesh ID | Material ID | Diffuse / normal-specular texture IDs | ARM anchors |
| --- | ---: | ---: | ---: | --- |
| 046 | 17263 | 34246 | 34046 / 34146 | `0xebde0–0xebdf8` constructs and registers the main mesh; `0xf21ac–0xf21ca` constructs and registers the material. Texture IDs come from `0xede44–0xede5e` and `0xeec0c–0xeec26`. |
| 047 | 17269 | 34247 | 34047 / 34147 | `0xebe90–0xebea8` main mesh; `0xf21e0–0xf21fe` material. Texture IDs come from `0xede80–0xeded2` and `0xeec48–0xeec9c`. |
| 048 | 17275 | 34248 | 34048 / 34148 | `0xebf40–0xebf58` main mesh; `0xf2214–0xf2230` material. Diffuse is loaded directly as `0x8500`, normal/specular from `0xeec78–0xeecd6`. |
| 049 | 17281 | 34249 | 34049 / 34149 | `0xebff0–0xec008` main mesh; `0xf2246–0xf2268` material. Texture IDs come from `0xedf30–0xedf4a` and `0xeecbc–0xeed4c`. |

The texture arithmetic uses `r8 = 0x84fa` at `0xec392`, while the mesh
material arithmetic uses `r6 = 0x84fa` at `0xebd2c`; both are narrowed
to 16 bits before the relevant constructors or registrations. The
kind-6 records occupy recovered table slots 677–680. Each of the eight
diffuse and normal/specular texture files occurs once in the original
Android OBB. Engine and light meshes retain their separate materials
32546–32549. These are source-backed table corrections; live loading
and rendering of the four ships remain unverified.

Windows native and ARM64 static builds pass. Strict ARM64 shared link
passes with no unresolved symbols and retains all 52 Java JNI exports
plus `JNI_OnLoad`. Five resource-material table tests pass. The full
resource-table smoke passes at `-O0` and `-O2`; its original-OBB check
finds all 91 selected platform texture paths.

ARMv7 compiles **209/209** translation units. Focused
`BuildResourceList` source-shape changes **77.0% → 77.1%** and strict
fuzzy **18.8% → 19.6%**. Generated instructions change **51,595 →
51,637** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship046-049-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-044–045 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship046-049-material-full-20261102.json`.

The current-source APK staged on the Redmi predates this package. Its
separate test package is not yet installed because Play Protect
rejected the earlier ADB attempt. Next: trace the ship 051–055 table
records and test a freshly packaged APK after device installation is
accepted.
