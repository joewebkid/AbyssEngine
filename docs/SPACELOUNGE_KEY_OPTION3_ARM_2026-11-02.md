# SpaceLounge keyboard option 3 preview and mission location

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

For keyboard option **3**, the original checks offers **2** and **3**
first (`0x16ea92`–`0x16eabe`). It creates `ListItemWindow` if needed.
Offer 2 previews the item at `getSellItemIndex()` (`0x16eac2`–
`0x16eafa`); offer 3 searches owned blueprints by
`getSellBlueprintIndex()` and previews the matching item
(`0x16eafe`–`0x16f05c`). Both set `listVisible` at offset `0x1c`
(`0x16f060`).

The other observed branch marks `wasAskedForLocation`, compares the
mission's target station with the current station, and displays game
text **805** when equal (`0x16edfc`–`0x16ee52`). If the station is
in the current system it displays text **806** (`0x16f360`–
`0x16f3ae`). These text paths clear chat scroll and remain in mode
**2** (`0x16e9b0`–`0x16e9bc`). For a target outside the system,
the original reuses or creates the application-module-5 StarMap with
`(true, mission, false, -1)` and sets `mapVisible`
(`0x16f54e`–`0x16fcae`). All recovered paths reach the common
generic-agent `setEvent(1)` tail (`0x16fdc6`–`0x16fddc`).

There is a source-backed ambiguity for offers **8–10**: the ARM
option-3 branch does not treat them as previews and proceeds to
dereference `Agent::getMission()`, while the recovered Agent
constructor initializes `mission` to null and can assign those
offers. The current source checks for a null mission before that
location branch, leaving these variants without a keyboard action.
This deliberate safety guard diverges from the observed ARM path;
the actual game-state invariant and iOS/PC behavior need further
investigation before claiming those variants restored.

Focused ARM comparison for `onKeyPress()` rose from **20.4% to 27.0%
source-shape** and **7.8% to 9.2% strict fuzzy**. The rebuilt
function has 682 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report: `D:/gof2-arm64-crypto/space-key-option3-20261102-focused.json`.

Runtime preview/location behavior remains unverified while device
testing is deferred. Next offline priority: keyboard option 0 and
the mission-text implementation.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.13%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-option3-20261102-full.json`.
