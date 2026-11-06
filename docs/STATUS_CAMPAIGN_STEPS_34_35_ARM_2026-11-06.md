# Campaign transitions, steps 34–35 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 34 to `0xaa06a`, which constructs
`Mission(11, 0, 29)` and joins `Status::setCampaignMission` at
`0xa9c30`. Step 35 branches to `0xab410`, constructs
`Mission(12, 0, 27)`, and joins the same campaign setter. The previous
source had both mission tuples but assigned them as ordinary missions.

## Recovery and verification

Both transitions now assign the mission to the campaign slot. The
production-source smoke runs steps 0–35 at `-O0` and `-O2`, checking
the mission types, station indices, and campaign progression.

- Focused ARM32 build: **211/211** translation units. The large
  `nextCampaignMission` function remains **14.0%** source-shape and
  **5.8%** strict fuzzy, with **300** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. Raw ARM32 unresolved-name count remains **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect step 36 and its side effects.
