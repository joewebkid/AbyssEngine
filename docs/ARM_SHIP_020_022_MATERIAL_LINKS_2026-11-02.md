# Ships 020–022 mesh material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Nine zero-valued main/LOD `ResourceMesh` arguments in `BuildResourceList`
are restored from the original function's value flow.

| Ship | Main / LOD mesh IDs | Material ID | ARM evidence |
| --- | --- | ---: | --- |
| 020 | 17020 / 17160 / 17161 | 34220 | `0xf1b66`: `r10 = 0x84fa + 0xb2`; `0xf1c8c`: save to `[sp, #0x178]`; `0x103a30–0x103a3a`: load, narrow and pass to the main constructor; `0x103a70` and `0x103aac`: reuse in `r4` for both LOD constructors. |
| 021 | 17021 / 17163 / 17164 | 34221 | `0xf1ba6`: `r11 = 0x84fa + 0xb3`; `0xf1ccc`: save to `[sp, #0x174]`; `0x103b60–0x103b6a`: load, narrow and pass to the main constructor; `0x103ba0` and `0x103bdc`: reuse in `r4` for both LOD constructors. |
| 022 | 17022 / 17166 / 17167 | 34222 | `0xf1be6`: `r8 = 0x84fa + 0xb4`; `0xf1d0c`: save to `[sp, #0x168]`; `0x103c90–0x103c9a`: load, narrow and pass to the main constructor; `0x103cd0` and `0x103d0c`: reuse in `r4` for both LOD constructors. |

The following registration instructions write the listed mesh IDs.
These three material IDs are kind-6 `ResourceMaterial` entries in the
original registration sequence and in the recovered resource table.
Nearby engine and light meshes have separate, nonzero arguments and
were not changed. This is source-backed registration data; live mesh
creation and rendering remain unverified on the current ARM64 APK.

Windows native and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. The strict ARM64 shared library links without
undefined symbols and preserves all 52 Java JNI exports plus
`JNI_OnLoad`. Seven resource/ship table tests and the full resource
table smoke at `-O0` and `-O2` pass. Focused `BuildResourceList`
source-shape stays **77.0%**, strict fuzzy rises **21.6% → 21.7%**, and
generated instructions change **51,453 → 51,470** versus 60,817 in
the original. The function remains nonexact. Focused report:
`D:/gof2-arm64-crypto/ship020-022-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
The normalized comparison-pair, linked-exact and byte-exact sets are
identical to the preceding ships-008–009 report; no exact matches are
lost. Full report:
`D:/gof2-arm64-crypto/ship020-022-material-full-20261102.json`.

The ARM64 shared library for this source revision is built locally.
The APK currently on the Redmi contains the earlier ships-008–009
revision, and the separate test package is still not installed. Its
first launch, rendering and previous player-ship crash remain open
runtime checks. The next resource-list priority is to trace ships
023–025 individually; their main and LOD mesh arguments are also zero
in the recovered source.
