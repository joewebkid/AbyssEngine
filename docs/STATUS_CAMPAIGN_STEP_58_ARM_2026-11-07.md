# Campaign transition, step 58 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 58 to `0xaa0c0`. At
`0xaa0cc`–`0xaa118`, the original deletes the old
`Globals::status->field_90` array, creates a new `Array<int>`, sets
its length to three and writes `[56,45,22]`. At
`0xaa130`–`0xaa152`, it checks every blueprint for index `179` and
calls `BluePrint::lock()` for each match. It then constructs campaign
`Mission(163,0,101)`, sets that mission's status value to zero, and
clears `Globals::status->field_10c` (`0xaa154`–`0xaa182`). The PLT
relocations identify `ArraySetLength<int>`, `BluePrint::getIndex`,
`BluePrint::lock`, `Mission` construction, campaign assignment, and
`Mission::setStatusValue`. The previous source only created the
mission.

## Recovery and verification

The source now follows the original sequence. A smoke test extracts
the production implementation and runs it at `-O0` and `-O2` through
step 58. It checks array replacement, all matching blueprint locks,
the mission tuple and status value, and the cleared Status field.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **23.9% -> 26.0%**; strict fuzzy
  **8.9% -> 9.2%**, with **632** generated instructions against
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
unverified. Next inspect step 59.
