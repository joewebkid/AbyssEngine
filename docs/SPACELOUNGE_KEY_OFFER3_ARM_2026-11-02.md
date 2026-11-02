# SpaceLounge keyboard blueprint offer 3

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

Keyboard option **0** routes offer **3** through the shared
single-precision price/credits gate (`0x16e7de`–`0x16e838`). On
insufficient credits, the existing text **203** shortfall path opens
a no-buttons window and sets `choiceVisible` (`0x16e83c`–
`0x16e8ec`). When affordable, the original starts from game text
**869**, uses the item indexed by `getSellBlueprintIndex()` to
substitute its game text for `#P`, then formats the seller's price
for `#C` (`0x16f86c`–`0x16f952`). It opens confirmation with
buttons and sets `chatActive` (`0x16fda4`–`0x16fdb0`).

On acceptance, the original chooses a reply from texts **850–852**,
charges the seller's price, searches `Status::getBluePrints()` for
an equal blueprint index, and calls `BluePrint::unlock()` on the
first match (`0x16ec20`–`0x16f7b0`). It marks the indexed global
agent accepted when its unsigned index is within the array, sets
`hangarUpdate`, and follows the common tail: mark the selected agent
accepted, display the reply under its name, clear `chatScroll`,
enter mode **3**, and clear `chatActive` (`0x16f7ba`–`0x16ffe2`).
The `BluePrint::unlock()` function itself is byte-exact with the
Android original in the corpus report.

Focused ARM comparison for `onKeyPress()` rose from **35.5% to 39.6%
source-shape** and **7.2% to 10.3% strict fuzzy**. The rebuilt
function has 1,046 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report: `D:/gof2-arm64-crypto/space-key-offer3-20261102-focused.json`.

The purchase has not been exercised in the game while device testing
is deferred. Other offer transactions and mission-text semantics
remain incomplete. Next offline priority: offer-2 item purchase or
mission-text recovery.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.14%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-offer3-20261102-full.json`.
