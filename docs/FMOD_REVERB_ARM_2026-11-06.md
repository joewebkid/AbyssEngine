# FMOD reverb recovery (ARM32)

Date: 2026-11-06

## Original-binary evidence

In the original Android 2.0.16 `libgof2hdaa.so`,
`FModSound::enableReverb(int)` at `0x938b8` calls
`EventSystem::getNumReverbPresets`, checks the requested index and the
current preset, stores the new index, then calls
`getReverbPresetByIndex(index, &props, nullptr)` and, on success,
`setReverbProperties(&props)`. The local properties structure occupies
80 bytes. `FModSound::disableReverb()` at `0x9392c` copies an 80-byte
constant from `0x1fb540` onto the stack, passes it to
`setReverbProperties`, and sets `reverbPreset` to `-1`. The constant
matches the bundled FMOD header's `FMOD_PRESET_OFF`, including
`Environment = -1`, `Room = -10000`, and `Flags = 0x33f`; the previous
source incorrectly passed 80 zero bytes. Both typed methods are
exported by the original `libfmodevent.so`.

## Recovery and verification

The source now uses `FMOD_REVERB_PROPERTIES` and the typed FMOD calls.
The silent ARM64 path retains its existing no-audio behavior. No
original binaries or game assets are committed.

- Focused ARMv7 build: **211/211** translation units. `enableReverb`
  source-shape rises **98.2% -> 99.1%**; strict fuzzy changes
  **94.5% -> 88.9%**. `disableReverb` rises from **96.5% / 92.5%**
  source-shape / strict fuzzy to **100% / 100%** and becomes
  **linked-exact**. It remains non-byte-exact.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The ARM64 build remains intentionally silent.
- Raw ARM32 unresolved names: **179 -> 180**. Two fictitious reverb
  helper names disappear; two real FMOD imports and compiler runtime
  `__aeabi_memcpy8` appear. This is an object-level inventory, not a
  failed full ARM32 shared link.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.97%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Exact membership
  comparison shows zero losses and one linked-exact gain:
  `FModSound::disableReverb`.

Live FMOD behavior and full ARM32 shared linking remain unverified.
The next narrow boundary is the remaining fade helper call, followed
by the event-stop wrapper.
