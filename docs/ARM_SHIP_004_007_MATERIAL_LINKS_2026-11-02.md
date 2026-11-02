# Ships 004–007 material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass restores twelve zero material links in `BuildResourceList`.

| Ship | Main / LOD resource IDs | Material ID | ARM value flow |
| --- | --- | ---: | --- |
| 004 | 17004 / 17112 / 17113 | 34204 | `0xf185c`: `r11 = 0x84fa + 0xa2`; saved to `[sp, #0x1c8]` at `0xf19a4`, loaded for the main constructor at `0x102acc`, then retained in `r4` for the LOD calls at `0x102b0c` and `0x102b48`. |
| 005 | 17005 / 17115 / 17116 | 34205 | `0xf1894`: `r9 = 0x84fa + 0xa3`; saved to `[sp, #0x1c0]` at `0xf19e6`, loaded at `0x102bfa`, then reused in `r4` at `0x102c3c` and `0x102c78`. |
| 006 | 17006 / 17118 / 17119 | 34206 | `0xf18d6`: `r10 = 0x84fa + 0xa4`; saved to `[sp, #0x1b8]` at `0xf1a22`, loaded at `0x102d2c`, then reused in `r4` at `0x102d6c` and `0x102da8`. |
| 007 | 17007 / 17121 / 17122 | 34207 | `0xf1906`: `r1 = 0x84fa + 0xa5`; saved to `[sp, #0x10]` at `0xf190c`, loaded at `0x102e5c`, then reused in `r4` at `0x102e9c` and `0x102ed8`. |

The registration instructions immediately after each constructor call
assign the listed mesh IDs. IDs 34204–34207 are kind-6 ship material
resources in the original ARM and have matching entries in the C++
resource table. The changed arguments are therefore backed by the
original binary; they are not inferred from the numerical sequence.

The nearby ship 008 and 009 links remain zero. Their ARM constructor
values take different register/stack paths, and this pass does not
assert their IDs. Runtime material pointers, rendering and the former
player-ship crash remain unverified on the current ARM64 APK.

Windows native, ARM64 static and strict ARM64 shared builds pass, and
the shared library retains 52 Java JNI exports plus `JNI_OnLoad`.
ARMv7 compiles 209/209 translation units. Seven resource/ship table
tests pass; the source-extracted full resource table smoke passes at
`-O0` and `-O2`.

The signed ARM64 test APK with restored ship 000–007 links is
`D:/gof2-arm64-crypto/gof2-arm64-ship004007-redmi-debug-20261102.apk`,
SHA-256 `15884ffe44483d4f7a5e17af94f30ca70e453b0d2f9ae116c42fdfe1e6d8a3be`.
It is on the Redmi at `/sdcard/Download/gof2-arm64-current-test.apk`
with a matching checksum. The separate test package is not installed
yet; runtime results remain pending.

Focused `BuildResourceList` ARM source-shape remains **77.0%** and
strict fuzzy remains **23.9%**. Generated instruction count rises
from 51,472 to 51,497 versus 60,817 in the original; the function
is still nonexact. Focused report:
`D:/gof2-arm64-crypto/ship004-007-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals across **4,762**
pairs at **96.20%** mean source-shape and **85.96%** strict fuzzy,
with **2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201
missing originals. The normalized comparison-pair, linked-exact and
byte-exact sets are identical to the preceding ships-001–003 report:
zero losses. Full report:
`D:/gof2-arm64-crypto/ship004-007-material-full-20261102.json`.

Next: inspect the ship 008/009 constructor arguments independently,
then check the current test APK's material pointers and frame once the
separate package is installed on the Redmi.
