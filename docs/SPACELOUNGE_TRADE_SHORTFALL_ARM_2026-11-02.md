# SpaceLounge trade-offer shortfall

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

For accepted offers **2, 3, 4, 8, 9, and 10**, the original loads
`Globals::options+0x38` (`0x171d18`), doubles the price only for
offers 8–10 when that byte is zero (`0x171d1e`–`0x171d2c`), and
compares the resulting floating-point price to the player's credits
(`0x171d34`–`0x171d5c`). The byte is also used as the DLC-4
ownership state in the existing `ndk_getDLC_4_BOUGHT()` recovery. The
pricing branch itself is confirmed by the original instructions;
the reason for this pricing policy remains unverified.

If the price exceeds the balance, the original computes the
floating-point shortfall and converts it to an integer
(`0x172f1a`–`0x172f32`). It formats that number, substitutes it for
`#C` in game text **203**, opens `ChoiceWindow` without buttons, sets
`choiceVisible` at offset `0x1b`, and returns
(`0x172f3c`–`0x172f9e`). The recovered source now follows this
path. The affordable offer actions remain incomplete and are the
next offline priority.

Focused ARM comparison for `OnTouchEnd()` rose from **35.7% to 39.4%
source-shape** and **17.4% to 18.7% strict fuzzy**. The rebuilt
function has 873 instructions versus 2,630 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches across 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass.

Runtime purchase behavior remains unverified while device testing
is deferred. Focused report:
`D:/gof2-arm64-crypto/space-touch-trade-shortfall-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-trade-shortfall-20261102-full.json`.
