# FMOD music and category calls (ARM32)

Date: 2026-11-06

## Original-binary evidence

The original APK's `FModSound::promptMusicCue(int)` at `0x93ad8`
loads `music` from offset `0x2400` and tail-dispatches virtual slot
`+0x28`, `MusicSystem::promptCue`. `setMusicParamValue(int, float)`
at `0x92fa8` uses virtual slot `+0x34`,
`MusicSystem::setParameterValue`. `setVolume(int, float)` at `0x92fb8`
loads a category pointer and dispatches slot `+0x20`,
`EventCategory::setVolume`.

`enableCategory(int, bool)` at `0x93288` stores the category flag,
calls category virtual slot `+0x1c` (`stopAllEvents`) when disabling,
and calls `FModSound::play(currentMusicEvent, nullptr, nullptr, 0)` at
`0x932c2` when enabling category one with an existing music event.
`stopAllSoundFXEvents` at `0x92dac` uses the same category virtual
slot for sound-effect categories. `pauseAllPlayingSoundFXEvents` uses
category virtual slot zero (`getInfo`) at `0x93c54` and checks index
one before pausing the event.

## Recovery and verification

The source now calls the typed FMOD methods and the real
`FModSound::play` overload. Six undefined stand-in helper names no
longer appear in the ARM32 object inventory. The source retains the
original gating, category iteration, and null checks.

- Focused ARMv7 build: **211/211** translation units. Three functions
  become 100% source-shape, 100% strict fuzzy, linked-exact, and
  byte-exact: `promptMusicCue`, `setMusicParamValue`, `setVolume`.
  `enableCategory` improves **79.0% -> 94.7%** source-shape and
  **40.0% -> 53.1%** strict fuzzy;
  `stopAllSoundFXEvents` **69.1% -> 99.0%** and **80.0% -> 90.5%**;
  `pauseAllPlayingSoundFXEvents` **89.7% -> 97.1%** and **65.4% ->
  69.7%**.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The ARM64 build remains intentionally silent.
- Raw ARM32 unresolved symbol names: **185 -> 179**. Removed names
  correspond exactly to the six stand-in music/category/play helpers;
  no unresolved name was added.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, **96.87%** mean source-shape, **86.97%** strict fuzzy,
  **2,801 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Exact membership
  comparison shows zero losses and precisely the three named gains
  for both exact measures.

The machine-code comparison validates these call targets and shapes.
It does not prove live audio behavior or a full ARM32 shared-library
link. Next investigate reverb, fading, and event-stop helper paths.
