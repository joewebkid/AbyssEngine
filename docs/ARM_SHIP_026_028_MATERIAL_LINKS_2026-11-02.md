# Ships 026–028 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Nine zero-valued main/LOD `ResourceMesh` arguments in `BuildResourceList`
are restored from the original function's value flow.

| Ship | Main / LOD mesh IDs | Material ID | ARM evidence |
| --- | --- | ---: | --- |
| 026 | 17026 / 17178 / 17179 | 34226 | `0xf1ce2`: `r11 = 0x84fa + 0xb8`; `0xf1e08`: save to `[sp, #0x14c]`; `0x104150–0x10415a`: load, narrow and pass to the main constructor; `0x104190` and `0x1041cc`: reuse in `r4` for both LOD constructors. |
| 027 | 17027 / 17181 / 17182 | 34227 | `0xf1d22`: `r8 = 0x84fa + 0xb9`; `0xf1e48`: save to `[sp, #0x144]`; `0x104280–0x10428a`: load, narrow and pass to the main constructor; `0x1042be` and `0x1042f8`: reuse in `r4` for both LOD constructors. |
| 028 | 17028 / 17184 / 17185 | 34228 | `0xf1d60`: `r6 = 0x84fa + 0xba`; `0xf1e88`: save to `[sp, #0x13c]`; `0x1043ac–0x1043b6`: load, narrow and pass to the main constructor; `0x1043ec` and `0x104428`: reuse in `r4` for both LOD constructors. |

The following registration instructions write the listed mesh IDs.
All three material IDs are kind-6 `ResourceMaterial` entries in the
original registration sequence and in the recovered resource table.
The engine and light meshes have separate arguments and were not
changed. This is source-backed registration data. Live creation,
material lookup and rendering for these ships remain unverified.

Windows native and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. The strict ARM64 shared library links without
undefined symbols and preserves 52 Java JNI exports plus
`JNI_OnLoad`. Seven resource/ship table tests pass.
The full resource-table smoke passes at `-O0` and `-O2`.

Focused `BuildResourceList` source-shape remains **77.1%** and strict
fuzzy remains **21.6%**. Generated instructions change **51,486 →
51,502** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship026-028-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the preceding ships-023–025 report; no exact matches are
lost. Full report:
`D:/gof2-arm64-crypto/ship026-028-material-full-20261102.json`.

The ARM64 shared library for this source revision is built locally.
The APK staged on the Redmi contains the earlier ships-008–009
revision, and its separate test package is not installed. The former
player-ship crash and frame rendering remain open runtime checks.
Next: trace the zero main/LOD mesh arguments of ships 029–031
individually, and observe the staged APK when installed.
