# Layout pulse sine call target (ARM32)

Date: 2026-11-06

## Evidence and change

The original APK's `Layout::getPulseValue(float)` at `0xd4bf0` calls
`AbyssEngine::AEMath::Sinf(float)@plt` twice, at `0xd4c18` and
`0xd4c34`. It also obtains `Status::getPlayingTime()` separately for
each call. The previous ARM32 source instead called an undefined
file-local `Sinf(float)` declaration, while ARM64 redirected the same
name through an inline helper. Both source calls now directly name
`AbyssEngine::AEMath::Sinf`; the two time reads and sine evaluations
are preserved. ARM32 `Layout.o` has two relocations to the real
`AEMath::Sinf` symbol and none to `_Z4Sinff`.

## Verification

- Focused ARMv7 build: 211/211 translation units; target source-shape
  97.0%, strict fuzzy 83.6%, non-exact, unchanged.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved symbol names: 188 -> 187. The only removed name
  is `_Z4Sinff`; no name was added.
- Full ARM corpus: 4,323/4,524 original functions, 4,772 pairs,
  96.83% mean source-shape, 86.93% strict fuzzy, 2,798 linked-exact,
  1,113 byte-exact, 201 missing, zero skipped, zero wrong-type, 152
  extra symbols. Both exact membership sets match the preceding
  corpus result exactly.

The original-binary call target and static compilation are verified.
Live UI behavior and a complete ARM32 shared-library link remain
unverified. The 201 missing original symbols are currently identified
as bundled ZIP/zlib/OpenSSL/C++ runtime functions, not gameplay
functions. Remaining gameplay-specific ARM32 placeholders are the
next bounded priority.
