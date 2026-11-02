# FMOD event progress recovery (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`FModSound::getPlayingProgress(int)` begins at `0x93988` in Thumb mode.

The original checks the initialized flag, event system, first category
enable flag and selected event pointer. It zero-initializes an
`FMOD_EVENT_INFO`, calls `FMOD::Event::getInfo`, converts the 32-bit
integers at info offsets 8 and 4 to float, and divides the second by
the first (`0x939b8`–`0x939f2`). The FMOD header identifies those fields
as `lengthms` and `positionms`, respectively. Thus the returned value
is `positionms / lengthms`. The old source made two calls to an
undefined integer-to-float helper, discarded both results and always
returned zero. The fix uses the declared FMOD method and returns the
original ratio. It deliberately follows the original's lack of a
zero-length guard; the behavior of a zero-length event remains
unverified.

The silent Android FMOD adapter now implements the actual `getInfo`
signature. It reports no event and clears output fields. The old
wrapper with the wrong signature was removed. The ARM64 shared library
links with `--no-undefined`, and its 52 Java JNI exports match the
previous test library. Windows native and ARM64 static builds pass;
the ARMv7 build compiles 209/209 translation units.

Focused ARM verification compares 46 symbols with 23 linked-exact and
3 byte-exact, unchanged from the preceding focused baseline. For
`getPlayingProgress`, source-shape improves **72.6% → 100%**, strict
fuzzy **52.3% → 89.8%**, and generated instructions **39 → 49** versus
49 in the original; the function is still not exact. Focused report:
`D:/gof2-arm64-crypto/fmod-progress-20261102-focused.json`.

The current-source ARM64 test APK is
`D:/gof2-arm64-crypto/gof2-arm64-current-redmi-debug-20261102.apk`,
SHA-256 `7b2d367a846ed1d1abb42ad7f67e2f3143fbdd4d2bb6e5dc23c0d0d003c05cf63`.
It is signed for the separate `net.fishlabs.gof2hdallandroiddevx`
package. The original OBB was copied to that package's directory on
the Redmi with matching SHA-256 and readable file permissions. Android
still listed only the original `net.fishlabs.gof2hdallandroid2012`
package at the time of this report, so the current APK's runtime and
shader behavior have not yet been observed. Manual installation of the
test package is pending after Play Protect blocked ADB installation.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.20%** mean source-shape and **85.96%** strict fuzzy, with
**2,741 linked-exact / 1,096 byte-exact**. There are zero skipped and
201 missing originals. Normalized comparison-pair and both exact sets
are unchanged from the preceding corpus; there are no exact losses.
Full report: `D:/gof2-arm64-crypto/fmod-progress-20261102-full.json`.

Next: install and observe the current APK, then trace the remaining
black frame and ship-material creation using the actual runtime log.
