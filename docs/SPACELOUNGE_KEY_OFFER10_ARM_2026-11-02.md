# SpaceLounge keyboard ship offer 10

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

Keyboard option **0** routes offer **10** through the single-precision
price/credits gate shared with offers 8 and 9. The displayed price
doubles when the options byte at offset `0x38` is zero. Insufficient
credits show text **203** with the factor-adjusted shortfall. When
affordable, the confirmation uses game text **873** and substitutes
the formatted price at `#C`.

After confirmation, the original forms a negative float charge from
`getSellItemPrice()`, doubles it under the same options-byte condition,
converts it to integer, and changes credits. The ARM path at
`0x16f30e` loads the station pointer from `Status + 0x14c`, which
corresponds to `voidStation`. The first `Ship::makeShip(-1)` call at
`0x16f322` creates a ship for that station. The path then calls
`Status::getStation()` at `0x16f332` and makes a **second** ship at
`0x16f34a` for the current station. The two `Station::addShip` calls
are at `0x16f32a` and `0x16f352`. Both ship instances come from the
prototype indexed by `getSellItemIndex()`. The path sets
`hangarUpdate`, accepts the selected agent, and shows a random reply
from texts **850–852** in mode 3. It does not mark the separately
indexed global agent accepted.

Focused ARM comparison for `onKeyPress()` rose from **47.5% to 49.1%
source-shape** and **13.8% to 18.5% strict fuzzy**. The rebuilt
function has 1,360 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report: `D:/gof2-arm64-crypto/space-key-offer10-20261102-focused.json`.

Runtime ship purchase, station persistence, and ownership remain
unverified while device testing is deferred. Next offline priority:
keyboard offer 8 or mission-text recovery.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.14%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-offer10-20261102-full.json`.
