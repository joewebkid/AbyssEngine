# FMOD language and initialization recovery (ARM32)

Date: 2026-11-06

## Original-binary evidence

The original APK's `FModSound::init()` starts at `0x92c30`. It calls
`FMOD_EventSystem_Create`, then `FMOD::EventSystem::init(32, 0x82,
nullptr, 0)` at `0x92c84`. After `GameText::getLanguage()` it calls
`FModSound::setAudioLanguage(int)`. The language function at `0x92d80`
selects pointers to the original `.rodata` strings `english` and
`deutsch` at `0x1cafc4`/`0x1cafcc`, and calls FMOD's
`EventSystem::setLanguage(const char *)`.

The original `init()` copies a suffix to the application root path.
When `lowMemory` is nonzero, the suffix is `FMOD_GOF2.fev` at
`0x1fb510`; otherwise it is `data/audio/FMOD_GOF2.fev` at `0x1fb4e8`.
It calls `EventSystem::load(path, nullptr, nullptr)` at `0x92cf6`,
gets categories named `master`, `music`, `sfx`, and `voice` at
`0x92d16`, and stores `getProjectByIndex(0, ...)` into the member at
offset `0x2404` at `0x92d30`.

## Source recovery

The source now uses those two paths and four category names, invokes
the typed FMOD methods, and models the member at `0x2404` as an
`FMOD::EventProject *`. Before this change the source appended
`.fev`/`_low.fev`, passed a null category argument, failed to store
the project pointer, and called undefined helper names. The new
ARM32 object uses offset `0x2404` for the project pointer, matching
the original. Its four new FMOD method imports are all exported by
the extracted original `libfmodevent.so`. The original APK also
declares `libfmodex.so` and `libfmodevent.so` as dependencies.

The separately authorized ARM64 first-build configuration remains
silent. Its `setAudioLanguage()` path is inactive so it does not
require a nonexistent ARM64 FMOD library. This does not implement
ARM64 audio playback.

## Verification

- Focused ARMv7 build: 211/211 translation units. `init` improves
  from 93.3% to 98.9% source-shape and 77.3% to 98.2% strict fuzzy;
  it remains non-exact. `setAudioLanguage` remains 100% on both
  measures and linked-exact. Final focused rerun gives identical
  results for all 46 `FModSound` comparisons.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved names remain 186: four undefined stand-in
  names are replaced by four actual FMOD method imports. All four
  imports are present in the original ARM32 `libfmodevent.so`.
- Full ARM corpus: 4,323/4,524 original functions, 4,772 pairs,
  96.83% mean source-shape, 86.93% strict fuzzy, 2,798 linked-exact,
  1,113 byte-exact, 201 missing, zero skipped, zero wrong-type, 152
  extra symbols. Both exact membership sets match the preceding
  result.

This verifies static call targets and source structure. Loading the
original FEV, live playback, and a complete ARM32 shared-library
link are not yet verified. Next investigate the remaining event and
listener paths in `FModSound` and the ARM32 external dependency link.
