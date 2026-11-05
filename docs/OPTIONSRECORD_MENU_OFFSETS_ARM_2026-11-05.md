# Menu option-record offsets (2026-11-05)

The original Android ARMv7 `MenuTouchWindow::startGOF2` at `0x12cf84`
writes the menu fade value at `Globals::options + 0x2c`
(`0x12cfa6–0x12cfa8`). The ELF identifies `Globals::options` through
its `R_ARM_GLOB_DAT` relocation at GOT `0x21016c`. The prior ARMv7
object instead emitted a store at `+0x30`. The reconstructed
`OptionsRecord` inserted C++ alignment after the unaligned `0x0d`
flag, shifting later members; its preview flag was likewise compiled
at `+0x4e` rather than the original `+0x48`.

`OptionsRecord` is now packed, with compile-time assertions for
offsets `0x0d`, `0x2c`, `0x32`, and `0x48`. The ARM32 `startGOF2`
path now uses its `fadeValue` field, matching the already typed ARM64
path. The rebuilt ARMv7 object stores at `+0x2c`. The two DLC startup
presets, `startValkyrie` (`0x12ca80`) and `startSupernova`
(`0x12cce4`), also use the corrected option and hint offsets.

Focused ARMv7 verification compiles **209/209** translation units.
`startGOF2` source-shape rises **84.2% → 84.8%** and strict fuzzy
**58.0% → 60.9%**; `startValkyrie` rises **97.5% → 98.7%** and
**70.8% → 71.6%**; `startSupernova` rises **96.4% → 97.6%** and
**64.7% → 65.1%**. None is exact. Windows native and ARM64 static
builds pass. A current device run is still needed to verify the
menu-to-game transition and DLC startup behavior.

Strict ARM64 shared linking passes with zero unresolved symbols and
53 JNI exports. Full ARMv7 verification compares **4,323/4,524**
original functions over **4,770** pairs: mean source-shape **96.78%**,
strict fuzzy **86.78%** (previously 86.77%), **2,782 linked-exact**,
**1,108 byte-exact**, 201 missing, zero skipped and zero wrong-type
matches. No comparison pair or exact-match member was lost.
