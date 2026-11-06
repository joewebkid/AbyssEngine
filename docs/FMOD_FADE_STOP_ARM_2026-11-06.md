# FMOD music fade and event stop (ARM32)

Date: 2026-11-06

## Original-binary evidence

In the original Android 2.0.16 `libgof2hdaa.so`,
`FModSound::fadeOutNow()` at `0x93550` returns when the current music
event is `-1` or already the fade target. Otherwise it tail-calls
`FModSound::setParamValue(0, currentMusicEvent, 0.1f)`; the sibling
`playMusicFadeOutCurrent(int)` calls that same method at `0x93542`.
The previous `fadeOutNow` used an undefined `FMOD_fade` stand-in.

`FModSound::stop(FMOD::Event*)` at `0x9357c` returns the `this` value
for a null event. For a live event, it tail-calls
`FMOD::Event::stop(false)`, whose method is exported by the original
`libfmodevent.so`. The resulting register holds either an object
address or an FMOD result code; `uintptr_t` describes this raw ABI
without pretending that both cases return an object pointer. No
source caller uses this return value. The previous call used an
undefined `FMOD_Event_stop_p` stand-in.

## Recovery and verification

The ARM32 paths now call the confirmed methods directly. The silent
ARM64 path still performs no audio operation.

- Focused ARMv7 build: **211/211** translation units. Both functions
  remain **100%** source-shape and strict fuzzy; `stop(FMOD::Event*)`
  retains linked-exact. Neither is byte-exact.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved names fall **180 -> 178**. Exactly the two
  fictitious fade/stop helper names disappear; no name is added.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.97%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Exact membership
  comparison shows no losses or gains.

These comparisons establish call shape and ABI at the ARM32 boundary;
live FMOD behavior and full ARM32 shared linking remain unverified.
