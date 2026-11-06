# Campaign transitions, steps 39–42 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends steps 39–42 to `0xa9f44`, `0xab42e`,
`0xab75e`, and `0xa9ec8`. They construct, respectively,
`Mission(161,0,-1)`, `Mission(4,0,-1)`, `Mission(160,0,-1)`, and
`Mission(11,0,10)`, then join `Status::setCampaignMission` at
`0xa9c30`. Before construction, step 41 stores `-10` to the two
Status fields at offsets `0x7c` and `0x80` (`0xab75e`–`0xab76a`).
The previous source had the mission tuples but assigned them as
ordinary missions and omitted the field updates.

## Recovery and verification

The four transitions now assign the mission to the campaign slot,
and step 41 resets both fields. The production-source smoke runs
steps 0–42 at `-O0` and `-O2`, checking mission tuples, the campaign
slot, visibility, progression, and the step 41 field values.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **14.2% -> 14.4%** and strict
  fuzzy **6.1% -> 6.2%**, with **310** generated instructions against
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
unverified. Next inspect step 43 and the larger behavior gaps at
steps 44–45.
