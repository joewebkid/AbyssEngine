# Campaign transition, step 66 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 66 to `0xaa5ec`. The original
constructs campaign `Mission(4,0,104)` and sets its status value to
zero (`0xaa5ec`–`0xaa618`). It then takes catalog items at indexes
`60`, `61`, and `62`, calls `Item::makeItem(5,0)` on each, and passes
the three copies in order to `Globals::status->station->addItem`
(`0xaa61c`–`0xaa68c`). The GOT relocations identify `Globals::items`,
`Globals::status`, `Item::makeItem(int,int)`, and `Station::addItem`.
The previous source created the mission but omitted its status value
and the station stock additions.

## Recovery and verification

The source now follows the original call order. A smoke test extracts
the production implementation and runs it at `-O0` and `-O2` through
step 66. It checks the campaign mission, status value, three item
indexes, quantity five, zero price, and station insertion order.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **27.3% -> 28.9%**; strict fuzzy
  **7.5% -> 7.8%**, with **723** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved names,
  with no name added or removed.

- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect step 67's production-goods setup.
