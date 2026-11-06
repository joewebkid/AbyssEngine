# Campaign transitions, steps 30–32 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends steps 30, 31, and 32 to `0xa9f62`,
`0xa9f28`, and `0xa9f80`. They construct `Mission(11, 30000, 98)`,
`Mission(11, 0, 10)`, and `Mission(8, 0, 10)`. The first two paths
join the campaign-mission setter at `0xa9c30`; step 32 calls that
setter directly at `0xa9f9e`. It then obtains the new mission from
the missions array and tail-calls `Mission::setProductionGoods(164,
50)` through the `0x19bf48` veneer, which resolves to the typed
method's PLT entry at `0x61ca4`.

The previous source added steps 30 and 31 as ordinary missions and
omitted the production-goods fields on step 32.

## Recovery and verification

The source now assigns all three to the campaign slot and sets the
step-32 production goods. The production-source smoke exercises
steps 0–32 at `-O0` and `-O2`, checking the mission tuples and both
production-goods values.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **12.3% -> 12.6%** and strict
  fuzzy **5.3% -> 5.5%**; generated instructions **260 -> 268**
  against **2,884** original. The `Status` unit retains 103
  linked-exact and 66 byte-exact functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. Raw ARM32 unresolved names remain **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression remains unverified. Next inspect step 33
separately: its original branch removes cargo and scans blueprints
before creating the next mission.
