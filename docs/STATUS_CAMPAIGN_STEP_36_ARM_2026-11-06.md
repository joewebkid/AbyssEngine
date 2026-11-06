# Campaign transition, step 36 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 36 to `0xab500`. It constructs
`Mission(160, 0, 27)`, calls `Status::setCampaignMission`, then reads
the first mission from the array at Status offset `0x198` and calls
`Mission::setVisible(false)` through the veneer at `0x19bf38` to
PLT `0x63858`. The previous source had the same mission tuple but
assigned it as an ordinary visible mission.

## Recovery and verification

Step 36 now assigns the mission to the campaign slot and hides its
first array entry. The production-source smoke runs steps 0–36 at
`-O0` and `-O2`, checking the tuple, slot, visibility, and progression.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **14.0% -> 14.2%** and strict
  fuzzy **5.8% -> 6.1%**, with **306** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. Raw ARM32 unresolved names remain **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect steps 37–38.
