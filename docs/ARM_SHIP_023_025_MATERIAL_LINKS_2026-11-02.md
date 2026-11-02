# Ships 023–025 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Nine zero-valued main/LOD `ResourceMesh` arguments in `BuildResourceList`
are restored from the original function's value flow.

| Ship | Main / LOD mesh IDs | Material ID | ARM evidence |
| --- | --- | ---: | --- |
| 023 | 17023 / 17169 / 17170 | 34223 | `0xf1c24`: `r6 = 0x84fa + 0xb5`; `0xf1d4c`: save to `[sp, #0x164]`; `0x103dc0–0x103dca`: load, narrow and pass to the main constructor; `0x103e00` and `0x103e3c`: reuse in `r4` for both LOD constructors. |
| 024 | 17024 / 17172 / 17173 | 34224 | `0xf1c62`: `r9 = 0x84fa + 0xb6`; `0xf1d88`: save to `[sp, #0x15c]`; `0x103eee–0x103ef8`: load, narrow and pass to the main constructor; `0x103f30` and `0x103f6c`: reuse in `r4` for both LOD constructors. |
| 025 | 17025 / 17175 / 17176 | 34225 | `0xf1ca2`: `r10 = 0x84fa + 0xb7`; `0xf1dc8`: save to `[sp, #0x154]`; `0x104020–0x10402a`: load, narrow and pass to the main constructor; `0x104060` and `0x10409c`: reuse in `r4` for both LOD constructors. |

The following registration instructions write the listed mesh IDs.
All three material IDs are kind-6 `ResourceMaterial` entries in the
original registration sequence and in the recovered resource table.
The engine and light meshes have separate nonzero arguments and were
not changed. This is source-backed registration data. Live creation,
material lookup and rendering for these ships remain unverified.

Windows native and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. The strict ARM64 shared library links without
undefined symbols and preserves 52 Java JNI exports plus
`JNI_OnLoad`. Seven resource/ship table tests pass, as does the full
resource-table smoke at `-O0` and `-O2`.

Focused `BuildResourceList` source-shape rises **77.0% → 77.1%**;
strict fuzzy falls **21.7% → 21.6%**, and generated instructions
change **51,470 → 51,486** versus 60,817 in the original. The
function remains nonexact. Focused report:
`D:/gof2-arm64-crypto/ship023-025-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the preceding ships-020–022 report; no exact matches are
lost. Full report:
`D:/gof2-arm64-crypto/ship023-025-material-full-20261102.json`.

The ARM64 shared library for this source revision is built locally.
The APK currently staged on the Redmi contains the earlier
ships-008–009 revision, and its separate test package is not installed.
The former player-ship crash and frame rendering remain open runtime
checks. Next: trace the zero main/LOD mesh arguments of ships 026–028
individually and observe the staged APK when installed.
