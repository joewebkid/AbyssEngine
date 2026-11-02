# Ships 029–031 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Nine zero-valued main/LOD `ResourceMesh` arguments in `BuildResourceList`
are restored from the original function's value flow.

| Ship | Main / LOD mesh IDs | Material ID | ARM evidence |
| --- | --- | ---: | --- |
| 029 | 17029 / 17187 / 17188 | 34229 | `0xf1d9e`: `r9 = 0x84fa + 0xbb`; `0xf1ec4`: save to `[sp, #0x134]`; `0x1044dc–0x1044e6`: load, narrow and pass to the main constructor; `0x10451c` and `0x104558`: reuse in `r4` for both LOD constructors. |
| 030 | 17030 / 17190 / 17191 | 34230 | `0xf1dde`: `r10 = 0x84fa + 0xbc`; `0xf1f06`: save to `[sp, #0x1ec]`; `0x10460c–0x104616`: load, narrow and pass to the main constructor; `0x10464c` and `0x104686`: reuse in `r4` for both LOD constructors. |
| 031 | 17031 / 17193 / 17194 | 34231 | `0xf1e1e`: `r11 = 0x84fa + 0xbd`; `0xf1f44`: save to `[sp, #0x1e4]`; `0x104738–0x104742`: load, narrow and pass to the main constructor; `0x104778` and `0x1047b4`: reuse in `r4` for both LOD constructors. |

The following registration instructions write the listed mesh IDs.
All three material IDs are kind-6 `ResourceMaterial` entries in the
original registration sequence and in the recovered resource table.
The stack slots for ships 030 and 031 held earlier texture IDs before
the material writes above; the last stores before the mesh calls are
the material IDs. The engine and light meshes have separate arguments
and were not changed. This is source-backed registration data. Live
creation, material lookup and rendering remain unverified.

Windows native and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. The strict ARM64 shared library links without
undefined symbols and preserves 52 Java JNI exports plus
`JNI_OnLoad`. Seven resource/ship table tests pass.
The full resource-table smoke passes at `-O0` and `-O2`.

Focused `BuildResourceList` source-shape remains **77.1%** and strict
fuzzy remains **21.6%**. Generated instructions change **51,502 →
51,521** versus 60,817 in the original. The function remains
nonexact. Focused report:
`D:/gof2-arm64-crypto/ship029-031-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the preceding ships-026–028 report; no exact matches are
lost. Full report:
`D:/gof2-arm64-crypto/ship029-031-material-full-20261102.json`.

The ARM64 shared library for this source revision is built locally.
The APK staged on the Redmi contains the earlier ships-008–009
revision, and its separate test package is not installed. The former
player-ship crash and frame rendering remain open runtime checks.
Next: trace ships 032–034. Resource ID 34234 has more than one
registration in the recovered table, so verify the effective lookup
before inferring ship 034's material behavior.
