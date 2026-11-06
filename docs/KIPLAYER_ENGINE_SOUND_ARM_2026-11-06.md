# KIPlayer engine-sound calls (ARM32)

Date: 2026-11-06

## Original-binary evidence

In the original Android 2.0.16 `libgof2hdaa.so`, the four
`KIPlayer` engine-sound methods at `0xa617e`-`0xa61be` load the
`Player*` at offset `+0x04`, check it and the sound-event ID at
`+0xf8`, then dispatch to `Player` methods. The branch veneers
at `0x19ba08`, `0x19ba18`, `0x19ba28`, and `0x19ba38` lead to the
PLT entries for `Player::PlayEngineSound(int, Vector*)`,
`PauseEngineSound()`, `ResumeEngineSound(bool)`, and
`StopEngineSound()`. Play passes the event ID and a null vector;
resume passes `false`. These methods already exist in the project.

## Recovery and verification

The ARM32 source now uses those typed `Player` methods, just as the
64-bit source did. Four undefined `FModSound_*Event` stand-ins and
the pointer-size split are removed.

- Focused ARMv7 build: **211/211** translation units. All four
  functions retain **100%** source-shape and strict fuzzy and remain
  linked-exact. None is byte-exact.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The ARM64 build remains intentionally silent.
- Raw ARM32 unresolved names fall **178 -> 174**; exactly the four
  stand-ins disappear, and no name is added.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.97%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Neither exact
  membership set gains or loses a function.

The remaining unresolved raw ARM32 names are external FMOD,
platform, C/C++ runtime, graphics, ZIP, and crypto dependencies.
This object-level inventory does not establish a successful complete
ARM32 shared-library link. Live audio also remains unverified.
