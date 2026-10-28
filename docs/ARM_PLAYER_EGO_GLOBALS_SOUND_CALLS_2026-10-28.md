# PlayerEgo sound-call global pointer recovery (2026-10-28)

## Original ARM evidence

The original Android ARM library exports `Globals::globals` at `0x2181d0`
and has a `R_ARM_GLOB_DAT` relocation for it at GOT slot `0x210218`.
Its dynamic symbol table has no `Globals::gGlobals` entry. In `PlayerEgo::PlayerEgo(Player*)`,
the PC-relative load at `0x99c58` reaches that GOT slot; `0x99c5c` stores
the GOT address in `fp`, and `0x99c60` loads the current `Globals::globals`
pointer before `Globals::addSoundResourceToList(int)`. The same `fp`-based
load appears before later equipment-gated sound calls, including
`0x99de8–0x99e2e`. The source now uses `Globals::globals` for the ten
sound registrations in this constructor. Other `gGlobals` sites are left for
their own ARM review.

The prior ARM64 APK crashed in `Globals::addSoundResourceToList(int)+12`,
called from `PlayerEgo.cpp:1780`. The crash register `x0` was null. Source
initialization sets `Globals::globals = this`, while `Globals::gGlobals`
has only a null definition and no assignment. The corrected constructor
calls use the initialized pointer and match the original symbol choice.

## Verification

Native and ARM64 static libraries build. ARMv7 compiles all 209 translation
units. Focused `PlayerEgo` compares 173 pairs with no skips, 97.33%
source-shape, 88.14% strict fuzzy, 99 linked-exact and 61 byte-exact;
these values and both exact sets are unchanged from the preceding corpus.
The constructor itself remains 83.9% source-shape and 53.2% strict fuzzy.
The full ARM corpus compares 4,323/4,524 original functions across 4,745
pairs with zero skips, 94.86% source-shape, 84.43% strict fuzzy, 2,638
linked-exact and 1,086 byte-exact. Relative to the preceding full report,
no compared pair or exact match was lost. Foreground device testing remains
pending.

A signed ARM64 APK is prepared at
`D:\gof2-arm64-crypto\gof2-arm64-playerglobals-debug-20261028.apk`
(SHA-256 `f1cf81c2d0e4b21ce8a8187419362ec354e5f5a39d8563b811261f6d12f9530f`).
It contains the linked library with 53 JNI exports and passes APK v1/v2/v3
signature and embedded-library checks. It installed on the `25053RT47C`
phone, but the device has remained in phone calls or another foreground app;
the new build has not been launched yet.

Next, verify the runtime path through player creation, then review the next
reached `gGlobals` use against original ARM before changing it.
