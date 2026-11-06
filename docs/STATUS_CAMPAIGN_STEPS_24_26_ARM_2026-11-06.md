# Campaign transitions, steps 24–26 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends campaign steps 24, 25, and 26 to
`0xa9e58`, `0xab786`, and `0xab6c8`. Step 24 gets ship cargo, scans
items until the first `Item::getIndex() == 0x83`, and calls
`setUnsaleable(true)` on that item (`0xa9e60`–`0xa9e8c`,
`0xab9d0`–`0xab9dc`). It then creates `Mission(156, 0, -1)`.
Step 25 writes `-1` to Status offsets `0x7c` and `0x80`, then creates
`Mission(4, 0, 48)`. Step 26 creates `Mission(11, 0, 10)`.
All three constructor paths join the campaign-mission setter at
`0xa9c30`. The previous source classified them as ordinary missions,
had incorrect step-24/25 tuples, and omitted the cargo and field
changes.

## Recovery and verification

The source now implements those side effects and campaign tuples.
The production-source smoke runs steps 0–26 at `-O0` and `-O2`,
including the first matching cargo item and the step-25 field reset.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **11.2% -> 12.1%** and strict
  fuzzy **4.8% -> 4.6%**; generated instructions **234 -> 256**
  against **2,884** original. The `Status` unit retains 103
  linked-exact and 66 byte-exact functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. Raw ARM32 unresolved names remain **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression remains unverified. Next inspect steps
27–29, including the paired Status fields at step 27.
