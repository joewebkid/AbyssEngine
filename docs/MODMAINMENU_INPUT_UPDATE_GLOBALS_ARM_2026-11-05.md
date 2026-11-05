# Main-menu input and update globals (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Its Thumb code for `ModMainMenu::OnTouchEnd(int, int, void*)` starts at
`0x175ab4`. The logo-dismissal branch loads GOT entry `0x210170`,
relocated to `Globals::logoIsShown`, and writes zero to that global.
The prior source dereferenced an unwritten pointer holder instead.

`ModMainMenu::OnUpdate()` starts at `0x175b78`. Its update calls load
GOT entry `0x210224`, relocated to `Globals::layout`, before calling
`Layout::update`. The audio call loads GOT entry `0x210168`, relocated
to `Globals::sound`, and calls `FModSound::updateAll`. The prior source
read two unwritten pointer holders. The recovered method uses the
identified global slots and retains the observed two layout updates.

Focused ARMv7 verification compiles **209/209** translation units.
`OnTouchEnd` source-shape rises **96.3% -> 98.1%**, and strict fuzzy
rises **95.2% -> 97.6%**. `OnUpdate` source-shape rises **96.4% ->
97.1%**, and strict fuzzy rises **85.7% -> 90.9%**. Neither function is
exact. Windows native and ARM64 static builds pass; strict ARM64 shared
linking succeeds. Runtime behavior remains unverified on a device.

Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.78%**, **2,784 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. Compared with the preceding
full corpus, there is no lost comparison pair or exact-match member.
