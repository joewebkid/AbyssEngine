# Main-menu global-pointer calls (2026-10-28)

## ARM evidence

The original Android ARM library has a `R_ARM_GLOB_DAT` entry for
`Globals::globals` at GOT address `0x210218`. `ModMainMenu::OnRelease`
loads that entry at `0x175b1c–0x175b22`, then calls
`Globals::loadFont(int)` at `0x175b2c`. The PC-relative literal at
`0x175b68` resolves to the same GOT address. `ModMainMenu::OnInitialize`
loads `Globals::globals` at `0x1759d2–0x1759d8` before calling
`Globals::playMusicAndFadeOutCurrent(int)` at `0x1759da`; its literal at
`0x175a78` also resolves to `0x210218`.

The source had called both functions through `Globals::gGlobals`, which has
only a null definition and no initializer. Both calls now load the original
`Globals::globals` pointer. No other menu behavior was changed.

## Verification and runtime limit

Native and ARM64 static libraries build; ARMv7 compiles 209/209 translation
units. Focused `ModMainMenu` compares 21 pairs with zero skips at 88.37%
source-shape and 82.23% strict fuzzy, including 10 linked-exact and 4
byte-exact matches. The two edited functions retain their prior individual
scores and the focused exact sets have no losses. The full ARM corpus
compares 4,323/4,524 original functions across 4,745 pairs, with zero skips,
94.86% source-shape, 84.43% strict fuzzy, 2,638 linked-exact and 1,086
byte-exact. Relative to the preceding full report, no compared pair or
exact match was lost.

The signed ARM64 test APK is
`D:\gof2-arm64-crypto\gof2-arm64-menuglobals-debug-20261028.apk`
(SHA-256 `6f4b4c1e2a8a4d905e4d3ab8b371dc005b1401697aeb8a906d9ee1d89425278e`).
It contains the linked library with 53 JNI exports, passes APK v1/v2/v3
signature and embedded-library checks, and installed on the `25053RT47C`
test phone. It reached the game foreground and passed the prior
`Globals::addSoundResourceToList` crash. The first captured screen was
mostly black. Roughly four minutes after launch it crashed twice in
`PlayerEgo::setShip(int, int)+88`, with fault address `0x20`. ARM64
disassembly shows that `AEGeometry` and `Mesh` exist but `mesh->materialBlock`
is null; the failing instruction reads `[materialBlock+0x20]`. The original
Android ARM `setShip` also reads the mesh material block without a null
guard (`0x9a29a–0x9a2a4`). The actual creation path for this material
remains unverified. The two edited menu calls were not independently
observed executing. This APK also contains the preceding `PlayerEgo`
pointer fix.

Next, trace the ship mesh resource and material creation path against
original ARM. Other `gGlobals` sites require separate ARM validation.
