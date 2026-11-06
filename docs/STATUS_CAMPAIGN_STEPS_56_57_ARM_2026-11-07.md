# Campaign transitions, steps 56–57 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 56 to `0xaa49a` and step 57 to
`0xaa4b8`. Step 56 constructs campaign `Mission(11,150000,101)`;
the signed integer literal at `0xaa5e8` is `150000`. The previous
source constructed `Mission(11,0,101)` as an ordinary mission.

Step 57 constructs campaign `Mission(166,0,101)`, then calls
`Mission::setProductionGoods(179,10)` on the installed mission. It
searches `Status::bluePrints` for the first blueprint with index `179`.
If found, it unlocks that blueprint and calls
`BluePrint::addItem(Globals::items[127],5,101)` using the catalog item
directly. It then restores `previousShip` through `Status::setShip` and
clears the saved pointer, whether or not a matching blueprint exists.
The original code for these effects spans `0xaa4b8`–`0xaa508` and
`0xab930`–`0xab966`. The previous source only created the mission.

## Recovery and verification

The source now follows those transitions. A smoke test extracts the
production implementation and runs it at `-O0` and `-O2` through step
57. It checks both mission tuples, production goods, the catalog-item
pointer and blueprint update, and restoration of the original ship
both with and without blueprint `179` in the list.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **22.8% -> 23.9%**; strict fuzzy
  **7.1% -> 8.9%**, with **580** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved names,
  with no name added or removed.

- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect step 58's blueprint and route-state changes.
