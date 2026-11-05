# Main-menu resume volume (2026-11-05)

The original Android ARMv7 `ModMainMenu::OnResume` starts at `0x1757f8`
in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
Its Thumb literal loads resolve to `Globals::sound` (GOT
`0x210168`) and `Globals::options` (`0x21016c`). It checks the sound
pointer, calls `FModSound::tryToStopMusicForBGMusic()` on that object,
then loads the first 32-bit word of options into integer argument
register `r2` before tail-calling `FModSound::setVolume(int, float)`
with category 1. The word is passed as floating-point bits under the
original soft-float calling convention, not converted from an integer.

The reconstructed method read unwritten holders, called a separate
wrapper for the music check, and numerically converted the raw word to
`float`. It now uses the observed sound slot, preserves the raw float
bits with `memcpy`, and calls the two typed sound methods. The sound
slot is read again before the final call, matching the original load
pattern.

Focused ARMv7 verification compiles **209/209** translation units.
Source-shape rises **92.1% → 99.0%**. Strict fuzzy changes
**86.4% → 80.0%** because the rebuilt compiler schedules the option
and sound loads in a different order; the function is not exact.
The argument values and call targets match the ARM evidence. Native
runtime behavior remains unverified on a device.

Windows native and ARM64 static builds pass. Strict ARM64 shared
linking succeeds with zero unresolved symbols and 53 JNI exports.
Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.78%**, **2,784 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. No comparison pair or
exact-match member was lost.
