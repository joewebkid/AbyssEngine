# SpaceLounge keyboard item offer 2

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

Keyboard option **0** routes offer **2** through the same
single-precision price/credits gate as offers 3 and 4
(`0x16e7de`–`0x16e838`). Insufficient credits show game text
**203** with the formatted shortfall and a no-buttons choice window
(`0x16e83c`–`0x16e8ec`). When affordable, game text **868** receives
`getSellItemQuantity()` at `#Q`, game text
`getSellItemIndex() + 1274` at `#P`, and formatted price at `#C`,
in that order (`0x16eed4`–`0x16f014`). Confirmation opens with
buttons and sets `chatActive` (`0x16fda4`–`0x16fdb0`).

On acceptance, the original selects a reply from texts **850–852**,
subtracts `getSellItemPrice()` from credits, and creates an item
from the indexed prototype with `getSellItemQuantity()`
(`0x16e61c`–`0x16e638`, `0x16eb44`–`0x16eb76`). If the new item
has type **1**, it scans ship equipment and adds the new amount to
every non-null item with the same index (`0x16eb80`–`0x16ebd2`).
If no matching equipment was found, it adds the created item to
ship cargo (`0x16ebd8`–`0x16ebe4`). For seller item indices
**132–153**, it sets the corresponding status flag at offset
`0xac` (`0x16ebfc`–`0x16ec12`). It sets `hangarUpdate` and takes
the common reply tail, marking only the selected agent accepted,
displaying the reply, clearing chat scroll, entering mode 3, and
clearing `chatActive` (`0x16ec16`, `0x16ff8c`–`0x16ffe2`). Unlike
offers 3 and 4, offer 2 does not mark an indexed agent in the
global agent array.

Focused ARM comparison for `onKeyPress()` rose from **39.6% to 46.4%
source-shape** and **10.3% to 10.5% strict fuzzy**. The rebuilt
function has 1,252 instructions versus 3,187 in the original. It
is neither linked-exact nor byte-exact. The focused unit retains
22 linked-exact and 7 byte-exact matches over 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.
Focused report: `D:/gof2-arm64-crypto/space-key-offer2-20261102-focused.json`.

Runtime item ownership and purchase behavior remain unverified while
device testing is deferred. The Android path does not visibly add or
delete the created item after it merges into equipment; that
ownership detail needs separate investigation. Next offline
priority: remaining offer transactions or mission-text recovery.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.14%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-offer2-20261102-full.json`.
