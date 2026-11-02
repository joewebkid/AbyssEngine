# Ships 001–003 material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The three ships' main meshes and two LODs each previously used zero
material links in `BuildResourceList`.

| Ship | Main / LOD resource IDs | Original material ID | ARM evidence |
| --- | --- | ---: | --- |
| 001 | 17001 / 17103 / 17104 | 34201 | `0xf17a2` forms `0x84fa + 0x9f`; `0xf18ba` saves it at `[sp, #0x1d8]`; `0x102744` reloads it for the main constructor and `0x102784`/`0x1027c0` reuse it in `r4` for the LODs. |
| 002 | 17002 / 17106 / 17107 | 34202 | `0xf17dc` forms `0x84fa + 0xa0`; `0xf17e2` saves it at `[sp, #0x14]`; `0x102870` reloads it for the main constructor and `0x1028b0`/`0x1028ec` reuse it for the LODs. |
| 003 | 17003 / 17109 / 17110 | 34203 | `0xf1820` forms `0x84fa + 0xa1`; `0xf1968` saves it at `[sp, #0x1d0]`; `0x1029a0` reloads it for the main constructor and `0x1029e0`/`0x102a1a` reuse it for the LODs. |

The adjacent registration instructions identify the main resource IDs
and both LOD IDs. Each of 34201–34203 is registered in the original
ARM as a kind-6 material resource, and the C++ table has matching
material entries with diffuse and normal/specular texture links.
These nine constructor arguments now match the original. This data
restoration does not demonstrate live rendering or material pointer
validity on the current ARM64 build; those remain to be observed.

Windows native, ARM64 static and strict ARM64 shared builds pass; ARMv7
compiles 209/209 translation units. Seven resource/ship table tests
pass. The source-extracted full resource table smoke passes at `-O0`
and `-O2`. A signed ARM64 test APK with the ship 000–003 material links
is `D:/gof2-arm64-crypto/gof2-arm64-ship001003-redmi-debug-20261102.apk`,
SHA-256 `626d34bbb0ebf3db0e5278f0001edde4c21c6796d5b705e3836f06cc3e9edfe5`.
It has been copied to the Redmi as
`/sdcard/Download/gof2-arm64-current-test.apk`; a live result remains
pending manual installation of the separate test package.

Focused `BuildResourceList` ARM source-shape remains **77.0%** and
strict fuzzy remains **23.9%** after this batch. Generated instruction
count rises from 51,461 to 51,472 versus 60,817 in the original; the
function is still nonexact. Focused report:
`D:/gof2-arm64-crypto/ship001-003-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals across **4,762**
pairs at **96.20%** mean source-shape and **85.96%** strict fuzzy,
with **2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201
missing originals. Normalized comparison-pair and both exact-match
sets are identical to the preceding ship-000 report: zero losses.
Full report: `D:/gof2-arm64-crypto/ship001-003-material-full-20261102.json`.

Next: observe the signed APK on the Redmi after manual installation,
then verify the still-zero material links for ships 004–009 against
their own ARM constructor instructions.
