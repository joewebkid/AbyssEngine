# SpaceLounge mission-difficulty button

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)` at `0x171f10`–`0x171f98`.

In mode 2, a consumed button at index **4** sets the selected agent's
byte at offset `0x24` to 1 (`wasAskedForDifficulty` in the recovered
class). It gets the agent mission's difficulty, converts it to float,
divides by **10.0f**, multiplies by **5.0f**, truncates to an integer,
then adds **808** for the game-text ID. The ARM instructions at
`0x171f3a`–`0x171f5a` show that order and float precision. The scroll
window receives the agent's name and a copy of that text. The branch
sets `singleOffer = 1`, `mode = 2`, and `chatScroll = 0` before the
common consumed-button event path. The former source had no action
for this button.

Focused ARM comparison for the whole `OnTouchEnd()` function rose from
**18.9% to 21.7% source-shape** and **14.7% to 15.5% strict fuzzy**.
The rebuilt function has 462 instructions versus 2,630 in the
original. It is not linked-exact or byte-exact. The focused unit
retains 14 linked-exact and 2 byte-exact matches across 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.

Live behavior remains unverified while device testing is deferred.
The mode-2 actions at indices 0, 2, and 3 remain incomplete, as do
the large `onKeyPress` and `startChat` paths.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.10%** mean source-shape and **85.86%** strict
fuzzy (unchanged at reported precision). It retains **2,733
linked-exact** and **1,091 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair and exact-match
sets are unchanged from the previous full report. Reports:
`D:/gof2-arm64-crypto/space-touch-end-button4-20261102-focused.json`
and `D:/gof2-arm64-crypto/space-touch-end-button4-20261102-full.json`.
