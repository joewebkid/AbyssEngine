# SpaceLounge blueprint and system confirmations

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

When the mode-2 acceptance button is consumed with enough credits,
offer **3** starts with game text **869**. The original indexes
`Globals::items` with `Agent::getSellBlueprintIndex()`, calls
`Item::getIndex()` on that item, and uses game text at that index
plus **1274** for `#P` (`0x172626`–`0x17269e`). It then replaces
`#C` with `Layout::formatCredits(Agent::getSellItemPrice())`
(`0x1726bc`–`0x172712`).

Offer **4** starts with game text **870**, gets the system at
`Agent::getSellSystemIndex()` from `Globals::galaxy`, and replaces
`#S` with `SolarSystem::getName()` (`0x172734`–`0x1727a6`). It then
replaces `#C` with the formatted agent price (`0x1727ca`–`0x172820`).
Both offers reach the common `ChoiceWindow::set(message,true)` tail
and mark `chatActive` at offset `0x19` (`0x1729ba`–`0x1729d0`).
The recovered source follows these paths and retains the common
generic-agent event handling.

Focused ARM comparison for `OnTouchEnd()` rose from **47.6% to 58.1%
source-shape** and **21.0% to 24.3% strict fuzzy**. The rebuilt
function has 1,351 instructions versus 2,630 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches across 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass.

Affordable confirmation paths for offers 8 and 10, along with other
offer actions, remain incomplete. Runtime purchase behavior remains
unverified while device testing is deferred. Focused report:
`D:/gof2-arm64-crypto/space-touch-blueprint-system-confirm-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-blueprint-system-confirm-20261102-full.json`.
