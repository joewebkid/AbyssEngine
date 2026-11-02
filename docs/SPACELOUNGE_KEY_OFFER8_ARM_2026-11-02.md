# SpaceLounge keyboard ship-mod offer 8

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

Keyboard option **0** routes offer **8** through the same
single-precision price/credits gate as offers 9 and 10
(`0x16e7c2`–`0x16e838`). The price factor is **2** when the options
byte at offset `0x38` is zero, otherwise **1**. Insufficient credits
show text **203** with the factor-adjusted shortfall. When affordable,
the confirmation uses game text **871** and the formatted price at
`#C` (`0x16fa52`–`0x16fabe`).

After confirmation, the original forms a negative float charge from
`getSellItemPrice()`, doubles it under the same options-byte
condition, converts it to integer, and calls `Status::changeCredits`
(`0x16ed7e`–`0x16ed96`, `0x16f1b8`–`0x16f1d4`). It then calls
`Status::getShip()`, obtains `Agent::getSellModIndex()`, and calls
`Ship::addMod()` (`0x16f1de`–`0x16f1ea`). The path sets
`hangarUpdate` and shows a random reply from texts **850–852** in
mode 3. At the common tail (`0x16ff9a`), offer **8** explicitly skips
`Agent::setOfferAccepted`; the selected agent can offer the mod
again. The separately indexed global agent is not marked accepted.

Focused ARM comparison for `onKeyPress()` rose from **49.1% to 49.8%
source-shape**. Strict fuzzy fell from **18.5% to 13.7%**; the added
branch changes alignment in this large, still incomplete function.
The rebuilt function has 1,384 instructions versus 3,187 in the
original. It is neither linked-exact nor byte-exact. The focused
unit retains 22 linked-exact and 7 byte-exact matches over 42
comparisons. ARMv7 compiles 209/209 units; Windows and ARM64 static
builds pass. Focused report:
`D:/gof2-arm64-crypto/space-key-offer8-20261102-focused.json`.

Runtime credit charge, mod application, and repeat purchase remain
unverified while device testing is deferred. Next offline priority:
mission-text recovery or other incomplete `SpaceLounge` branches.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.14%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-offer8-20261102-full.json`.
