# Ships 008–009 mesh resource links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Six zero-valued `ResourceMesh` constructor arguments in `BuildResourceList`
are restored from the original function's register and stack value flow.

| Ship | Main / LOD IDs | Constructor argument | ARM evidence |
| --- | --- | ---: | --- |
| 008 | 17008 / 17124 / 17125 | 34208 | `0xf18d2`: `r6 = 0x84fa`; `0xf193e`: `r1 = r6 + 0xa6`; `0xf1944`: save to `[sp, #0x1f0]`; `0xfe1d8–0xfe1dc`: load and narrow to `r6`; `0xfe8a2`: save to `[sp, #0x304]`; `0x102f1c`: reload; `0x102f8c`, `0x102fc6`, `0x103000`: pass `r6` to the three mesh constructors. |
| 009 | 17009 / 17127 / 17128 | 34109 | `0xec392`: `r8 = 0x84fa`; `0xee418`: `r6 = r8 + 0x43`; `0xee432`: save to `[sp, #0x1b0]`; `0x1030b8–0x1030c0`: load, narrow and pass in `r2`; `0x1030f8` and `0x103134`: reuse the value in `r4` for the LOD constructors. |

The mesh registration instructions after these calls assign the IDs in
the table. Literal-pool words in the flat disassembly are not treated
as executable writes. The only apparent write to `r6` between
`0xfe1dc` and `0xfe8a2`, at `0xfe314`, is a literal-pool word skipped
by the branch at `0xfe312`.

Ship 008's 34208 is a kind-6 `ResourceMaterial` entry. Ship 009's
**34109 is a kind-2 `ResourceTexture` entry**, although a separate
kind-6 material 34209 also exists. The original ARM passes 34109; the
source change records that fact rather than substituting 34209 from
the numbering pattern. `PaintCanvas::MaterialCreate` does not check
the resource kind before reading the payload as material fields. The
live lookup result and rendering behavior for ship 009 remain
unverified. The engine and light mesh entries were not changed.

The current-source ARM64 APK's runtime result, including the previous
`PlayerEgo::setShip` null-material crash and black frame, remains
unverified until the separate test package is installed and observed
on the Redmi.

Windows native and ARM64 static builds pass. The ARMv7 verifier compiles
209/209 translation units, and the strict ARM64 shared library links
without undefined symbols with the same 52 Java exports plus
`JNI_OnLoad` as the preceding test library. Seven resource/ship table
tests and the source-extracted full resource table smoke at `-O0` and
`-O2` pass. Focused `BuildResourceList` source-shape stays **77.0%**;
strict fuzzy falls **23.9% → 21.6%**, and generated instructions
change **51,497 → 51,453** against 60,817 original instructions. The
function is not linked-exact or byte-exact. Focused report:
`D:/gof2-arm64-crypto/ship008-009-material-focused-20261102.json`.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs: **96.20%** mean source-shape, **85.96%** strict fuzzy,
**2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201 missing.
Normalized comparison-pair, linked-exact and byte-exact sets are
identical to the ships-004–007 baseline; there are no exact losses.
Full report: `D:/gof2-arm64-crypto/ship008-009-material-full-20261102.json`.

The signed ARM64 test APK is
`D:/gof2-arm64-crypto/gof2-arm64-ship008009-redmi-debug-20261102.apk`,
SHA-256 `fe243580c1e65182bc78faab6f9392005814e90c9482c1d4a54405fcaccc330e`.
Its copy at `/sdcard/Download/gof2-arm64-current-test.apk` on the
Redmi has the same checksum. The clone OBB under
`/sdcard/Android/obb/net.fishlabs.gof2hdallandroiddevx/` is readable
and matches the original OBB checksum. At the last ADB check, Android
still listed only the original GOF2 package, so the current test APK
had not been installed or observed. The user's reported resource-check
screen could not be attributed to this APK from the available device
state. Its launch and material behavior remain open runtime checks.
