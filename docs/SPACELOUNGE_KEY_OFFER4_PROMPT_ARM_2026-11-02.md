# SpaceLounge keyboard offer-4 prompt and credit gate

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

When keyboard option **0** is confirmed, offer **1** returns to mode
0 (`0x16f3d2`–`0x16f3d6`). Offers **2–4** and **8–10** enter the
ARM price gate. The multiplier only applies to offers 8–10, so
offer **4** compares its price with credits at factor 1.0 using
single-precision conversion (`0x16e7de`–`0x16e838`). When the
price exceeds credits, it formats the shortfall and substitutes it
for `#C` in game text **203**, opens a no-buttons choice window,
sets `choiceVisible`, and returns without the generic-agent event
(`0x16e83c`–`0x16e8ec`). The shortfall conversion follows the
original float arithmetic at `0x16e876`–`0x16e896`.

For an affordable offer **4**, the original starts from game text
**870**, substitutes the name of `Galaxy::getSystem` at the seller's
system index for `#S`, then the formatted price for `#C`
(`0x16f960`–`0x16fa44`). It opens the choice window with buttons,
marks `chatActive`, and reaches the generic-agent `setEvent(1)`
tail (`0x16fda4`–`0x16fddc`). The existing confirmed acceptance
path then charges the price, reveals the system and enables the map.
Keyboard option 0 for other offers remains incomplete.

Focused ARM comparison for `onKeyPress()` rose from **27.0% to 35.5%
source-shape**; strict fuzzy moved **9.2% to 7.2%**. The rebuilt
function has 929 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass.
Focused report:
`D:/gof2-arm64-crypto/space-key-offer4-prompt-20261102-focused.json`.

Runtime purchase and map behavior remains unverified while device
testing is deferred. Next offline priority: the remaining option-0
offer families or the mission-text implementation.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs. Mean source-shape rose **96.13% to 96.14%**;
strict fuzzy remains **85.93%**. It retains **2,741 linked-exact**
and **1,096 byte-exact** matches, zero skipped, and 201 missing
originals. The normalized comparison-pair, linked-exact, and
byte-exact sets are unchanged from the previous full report. Full
report:
`D:/gof2-arm64-crypto/space-key-offer4-prompt-20261102-full.json`.
