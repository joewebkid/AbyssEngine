# SpaceLounge offers 8 and 10: confirmation text

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

For an affordable offer **8**, the original starts with game text
**871** (`0x172840`–`0x17284c`). For offer **10**, it starts with
text **873** (`0x17291c`–`0x172928`). Each branch converts the
agent's item price to float, multiplies by the earlier `S24` price
factor, converts the result to an integer, formats credits, and
substitutes `#C` (`0x172868`–`0x1728bc` and
`0x172944`–`0x172998`). Both reach the common
`ChoiceWindow::set(message,true)` tail and mark `chatActive` at
offset `0x19` (`0x1729ba`–`0x1729d2`). The recovered source uses
separate branches, following the two original blocks.

A shared C++ branch with a conditional text ID produced **60.8%**
source-shape and **23.3%** strict fuzzy for `OnTouchEnd()`. Separate
branches produced **63.4%** and **27.0%**, respectively, and preserve
the original control-flow distinction. Relative to the previous
package, focused `OnTouchEnd()` rose from **58.1% to 63.4%
source-shape** and **24.3% to 27.0% strict fuzzy**. The rebuilt
function has 1,503 instructions versus 2,630 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches across 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass.

Offer actions outside the trade family remain incomplete. Runtime
purchase behavior remains unverified while device testing is
deferred. Focused report:
`D:/gof2-arm64-crypto/space-touch-offer-8-10-separate-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-offer-8-10-20261102-full.json`.
