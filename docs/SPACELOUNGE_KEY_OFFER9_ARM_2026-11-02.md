# SpaceLounge keyboard item offer 9

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

Keyboard option **0** routes offer **9** through the shared
single-precision price/credits gate. The price factor is **2** when
the observed options byte at offset `0x38` is zero, otherwise **1**
(`0x16e7de`–`0x16e838`). Insufficient credits show game text **203**
with the factor-adjusted shortfall and a no-buttons choice window.
When affordable, game text **868** receives the item quantity at
`#Q`, item text `getSellItemIndex() + 1274` at `#P`, and the
factor-adjusted formatted price at `#C` (`0x16eed4`–`0x16f014`).
The ARM instructions at `0x16efba`–`0x16efca` confirm the
single-precision multiplication before integer formatting. The
choice window opens with buttons and sets `chatActive`.

On acceptance, the original creates a negative float charge from
`getSellItemPrice()`; it doubles that value when the options byte
is zero, converts it to integer, and calls `Status::changeCredits`
(`0x16eda4`–`0x16edc6`, `0x16f1f2`–`0x16f210`). It makes the
item from the indexed prototype with the offered quantity. Type-1
items merge their amount into every same-index equipped item;
otherwise the new item is added to cargo (`0x16f21a`–`0x16f2d8`).
The path then sets `hangarUpdate`, marks the selected agent accepted,
and shows the random reply from game texts **850–852** in mode 3.
Unlike offer 2, it does not set the status flags for item indices
132–153; unlike offers 3 and 4, it does not mark an indexed agent
in the global agent array.

Focused ARM comparison for `onKeyPress()` rose from **46.4% to 47.5%
source-shape** and **10.5% to 13.8% strict fuzzy**. The rebuilt
function has 1,301 instructions versus 3,187 in the original. It
is neither linked-exact nor byte-exact. The focused unit retains
22 linked-exact and 7 byte-exact matches over 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.
Focused report: `D:/gof2-arm64-crypto/space-key-offer9-20261102-focused.json`.

Runtime purchase and ownership behavior remain unverified while
device testing is deferred. The merged-item ownership question
recorded for offer 2 applies here as well. Next offline priority:
remaining offers 8/10 or mission-text recovery.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.14%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-offer9-20261102-full.json`.
