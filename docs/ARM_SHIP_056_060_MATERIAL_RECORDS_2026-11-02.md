# Ships 056–060 mesh and material records (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`BuildResourceList` previously gave zero material IDs to five main ship
meshes and the 060 lights mesh. The original passes the following IDs
to the `ResourceMesh` constructors:

| Mesh | Resource ID | Material ID | ARM evidence |
| --- | ---: | ---: | --- |
| 056 main | 17323 | 34256 | `0xec330–0xec348`: `0x84fa + 0xd6`, narrowed and passed in `r2`. |
| 057 main | 17329 | 34257 | `0xec3ae–0xec3cc`: `0x86c5 - 0xf4`. |
| 058 main | 17335 | 34258 | `0xec42c–0xec444`: `0x86c5 - 0xf3`. |
| 059 main | 17341 | 34259 | `0xec4a4–0xec4bc`: `0x86c5 - 0xf2`. |
| 060 main | 17347 | 34260 | `0xec51c–0xec534`: `0x86c5 - 0xf1`. |
| 060 lights | 18823 | 34602 | `0xec564–0xec580`: `0x8728 + 2`, narrowed and passed in `r2`. |

The five kind-6 main materials at resource slots 687–691 are 34256–34260.
Their constructors use texture pairs 34056–34060 (diffuse) and
34156–34160 (normal/specular), with blend value 28. The ARM records are
at `0xf23e2–0xf24c0`; the corresponding texture IDs are retained from
`0xee01c–0xee162` and `0xeede4–0xeef2a`. The ten secondary kind-6
materials at slots 821–830 use each diffuse ID twice, with blend values
0 and 2. Their ARM registrations are at `0xf3b3e–0xf3cf8`.

The original Android OBB contains all ten referenced 056–060 texture
files. No kind-6 resource with ID 34602 has yet been identified in the
recovered source table. The 060 lights link is the original operand,
but whether and where that material resolves at runtime remains open.
Rendering of these ships and the newly installed Redmi APK also remains
unverified until the Redmi is visible over ADB and its process log can
be inspected.

Windows native and ARM64 static builds pass. The strict ARM64 shared
link passes with no unresolved symbols and retains the previous 52 Java
JNI exports plus `JNI_OnLoad`. Five resource-material table tests pass.
The full resource-table smoke passes at `-O0` and `-O2`, with all 91
selected platform texture paths present in the original OBB.

ARMv7 compiles **209/209** translation units. Focused
`BuildResourceList` source-shape improves **77.1% → 77.2%**, while
strict fuzzy declines **27.0% → 20.5%**. Generated instructions change
**51,731 → 51,805** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship056-060-material-focused-final-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-051–055 baseline; no exact matches are lost.
Full report: `D:/gof2-arm64-crypto/ship056-060-material-full-20261102.json`.

Next: inspect the unresolved 34602 light material and the remaining
ships 061–063 placeholders. The Redmi resource-download screen needs
its installed package, OBB permissions and process log checked over
ADB before attributing it to the new build.
