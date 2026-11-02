# Ships 032–034 mesh materials and late material records (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The nine main/LOD `ResourceMesh` calls for ships 032–034 previously passed
zero as their material ID. The original function supplies these values:

| Ship | Main / LOD mesh IDs | Material ID | ARM value flow |
| --- | --- | ---: | --- |
| 032 | 17032 / 17196 / 17197 | 34232 | `0xf1e5e`: `r8 = 0x84fa + 0xbe`; `0xf1f8a`: save to `[sp, #0x1dc]`; `0x104868–0x104872`: load and pass to main mesh constructor; `0x1048a8`, `0x1048e4`: reuse for LODs. |
| 033 | 17033 / 17199 / 17200 | 34233 | `0xf1e9c`: `r6 = 0x84fa + 0xbf`; `0xf1f04`: save to `[sp, #0x12c]`; `0x104998–0x1049a2`: load and pass to main constructor; `0x1049d8`, `0x104a14`: reuse for LODs. |
| 034 | 17034 / 17202 / 17203 | 34234 | `0xf1eda`: `r9 = 0x84fa + 0xc0`; `0xf1f66`: save to `[sp, #0x124]`; `0x104ac4–0x104ace`: load and pass to main constructor; `0x104b04`, `0x104b40`: reuse for LODs. |

The following registrations write the listed mesh IDs. Kind-6 material
records 34232, 34233 and 34234 occur earlier in the same original
resource list. Engine and light meshes have separate arguments and were
not changed.

Two later material records in the recovered table were also wrong.
The earlier texture-registration value flow has `r8 = 0x84fa` at
`0xec392`; `0xeddcc` and `0xede08` calculate texture IDs 34044 and
34045, saved at `0xedde6` and `0xede22` to `[sp, #0x1a0]` and
`[sp, #0x198]`. At `0xeeb94` and `0xeebd4`, the same base plus
`0x66` and `0x67` calculates 34144 and 34145, saved at `0xeebae`
and `0xeebea` to `[sp, #0xe4]` and `[sp, #0xe0]`. The material constructors at
`0xf2150` and `0xf2186` consume those pairs with blend value 28.
The instructions at `0xebcb4` and `0xebd30` calculate IDs
`0x84fa + 0xca = 34244` and `0x84fa + 0xcb = 34245`, saved at
`0xebcb6` and `0xebd34`; `0xf2154–0xf2160`
and `0xf218a–0xf2196` register them as kind-6 records. The old source
registered duplicate IDs 34236 and 34234 with incorrect texture pairs.
The corrected records are 34244 → (34044, 34144) and
34245 → (34045, 34145). The earlier 34234 material remains the one
used by ship 034. This is a source-backed table correction; live
material loading and rendering still require a current APK test.

Windows native and ARM64 static builds pass. The strict ARM64 shared
library links without undefined symbols and retains all 52 Java JNI
exports plus `JNI_OnLoad`. ARMv7 compiles **209/209** translation units.
Seven resource/ship table tests pass. The full resource-table smoke
passes at `-O0` and `-O2`; its original-OBB check finds all 91 selected
platform texture paths.

Focused `BuildResourceList` source-shape changes **77.1% → 77.0%** and
strict fuzzy **21.6% → 19.3%**, with generated instructions **51,521 →
51,539** versus 60,817 original. This very large function remains
nonexact. The decrease does not override the direct ARM evidence for
the corrected IDs and constructor arguments. Focused report:
`D:/gof2-arm64-crypto/ship032-034-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-029–031 baseline. Full report:
`D:/gof2-arm64-crypto/ship032-034-material-full-20261102.json`.

The APK staged on the Redmi still contains the earlier ships-008–009
revision. Its separate package was not installed at this checkpoint,
so live material loading, the old player-ship crash and frame rendering
remain unverified. Next: trace ships 035–037 and the Supernova 044–045
mesh links, then test an APK built from current source.
