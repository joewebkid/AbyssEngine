# Campaign transitions, steps 46–48 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends steps 46–48 to `0xab4b0`, `0xa9da4`,
and `0xab548`. Step 46 constructs campaign `Mission(11,0,74)`.
Step 47 stores the current ship at Status offset `0x8c`, clears the
ship pointer at `0x190`, then calls `Globals::ships[9]->makeShip(-1)`
and `Status::setShip`. It sets race `1`, equips catalog items `58`
and `83` in slots `0` and `1` through `Globals::status->ship`, clears
Status field `0x30` through that global pointer, and constructs
campaign `Mission(11,0,58)` (`0xa9da4`–`0xa9e3a`).

Step 48 makes ship template `41` with argument `-1`, sets race `1`,
equips three new copies of item `177` in slots `0`–`2`, then equips
items `58`, `83`, and `52` in those respective slots. It constructs
campaign `Mission(156,0,58)` (`0xab548`–`0xab610`). The original
relocations identify the accessed globals: `Globals::ships` at GOT
`0x210210`, `Globals::items` at `0x2101fc`, and `Globals::status` at
`0x21015c`. The previous source omitted both ship transitions and
classified all three missions as ordinary; step 46 also used station
`58` instead of `74`.

## Recovery and verification

The source now follows these paths. The production-source smoke runs
steps 0–48 at `-O0` and `-O2`, checking the mission tuples and slot,
preserved previous ship, template indices and creation arguments,
race, equipment call order, and Status field `0x30`.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **15.0% -> 19.6%** and strict
  fuzzy **5.8% -> 6.6%**, with **443** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved
  names, with no name added or removed.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect steps 49–50.
