# Campaign transition, step 55 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 55 to `0xaa3ca`. The original
stores the current ship at Status offset `0x8c`, clears the ship
pointer at `0x190`, makes ship template `39` with argument `-1`,
calls `Status::setShip`, and sets race `1` (`0xaa3d6`–`0xaa404`).
It then equips new catalog items `181`, `52`, `58`, and `83` in
slots `0`, `0`, `1`, and `2`, respectively, through
`Globals::status->ship` (`0xaa408`–`0xaa482`). Finally it constructs
campaign `Mission(4,0,102)` (`0xaa484`–`0xaa496`). The original
relocations resolve the accessed globals to `Globals::ships` at
GOT `0x210210`, `Globals::items` at `0x2101fc`, and
`Globals::status` at `0x21015c`.

The previous source had the mission tuple but classified it as
ordinary and omitted the ship and equipment transition.

## Recovery and verification

The source now follows this sequence. The production-source smoke
runs the recovered campaign through step 55 at `-O0` and `-O2` and
checks the saved old ship, template index and creation argument,
race, equipment call order, and campaign mission tuple.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **20.4% -> 22.8%** while strict
  fuzzy **7.7% -> 7.1%**, with **547** generated instructions
  against **2,884** original. The `Status` unit retains **103
  linked-exact** and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved
  names, with no name added or removed.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect step 56.
