# SpaceLounge item-offer confirmation

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

When the mode-2 acceptance button is consumed for an affordable offer
**2 or 9**, the original starts with game text **868**. It replaces
`#Q` with `Agent::getSellItemQuantity()` converted to a string
(`0x171da4`–`0x171e00`), then `#P` with game text at
`Agent::getSellItemIndex() + 1274` (`0x171e34`–`0x171e7a`). Finally,
it formats the integer conversion of the floating-point price,
including the previously verified multiplier for offer 9, and
replaces `#C` (`0x171e9c`–`0x171ef4`). The common tail calls
`ChoiceWindow::set(message,true)` and marks `chatActive` at offset
`0x19` (`0x1729ba`–`0x1729d0`). The recovered source follows this
order and retains the common generic-agent event path.

Focused ARM comparison for `OnTouchEnd()` rose from **39.4% to 47.6%
source-shape** and **18.7% to 21.0% strict fuzzy**. The rebuilt
function has 1,078 instructions versus 2,630 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches across 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass.

Affordable confirmation paths for offers 3, 4, 8, and 10, as well as
the other offer actions, remain incomplete. Runtime purchase behavior
remains unverified while device testing is deferred. Focused report:
`D:/gof2-arm64-crypto/space-touch-trade-item-confirm-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-trade-item-confirm-20261102-full.json`.
