# SpaceLounge system-information offer acceptance

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

In mode **2**, with an active confirmation, left/right keys call
`ChoiceWindow::left/right` (`0x16e4d6`–`0x16e504`). Confirm key
`0x10000` starts the accepted-offer path. The original first chooses
one of game texts **850–852** using `AERandom::nextInt(3)`
(`0x16e61c`–`0x16e638`). For offer **4**, it subtracts
`Agent::getSellItemPrice()` from credits and calls
`Status::setSystemVisibility(getSellSystemIndex(), true)`
(`0x16ec6a`–`0x16ec8a`). It sets lounge bytes `0x18` (show map
after final reply) and `0x35` (`hangarUpdate`) to one (`0x16ec8e`–
`0x16ec92`). If the agent's unsigned index is within
`Status::getAgents()`, it also marks that global agent's offer
accepted (`0x16ec96`–`0x16ecba`). The common tail marks the selected
agent accepted, displays the random reply under its name, clears
`chatScroll`, changes mode to **3**, and clears `chatActive`
(`0x16ff9a`–`0x16ffe2`). The next confirm key in mode 3 opens the
system map via the previously recovered branch.

The source now implements this confirmed transaction. For active
confirmations of other offer types, the remaining acceptance paths
have not yet been reconstructed. Their confirm key is left without
side effects rather than taking the old incorrect path that dismissed
the dialogue as though the purchase had completed.

Focused ARM comparison for `onKeyPress()` rose from **8.8% to 14.6%
source-shape** and **4.3% to 5.0% strict fuzzy**. The rebuilt
function has 370 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report: `D:/gof2-arm64-crypto/space-key-offer4-20261102-focused.json`.

Runtime purchase, dialogue and map behavior remains unverified while
device testing is deferred. Next offline priority: the remaining
mode-2 offer transactions and keyboard option selection.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.13%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-offer4-20261102-full.json`.
