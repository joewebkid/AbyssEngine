# Campaign transition, step 76 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 76 to `0xaa80c`. The original
calls `Ship::getFirstEquipmentOfSort(18)` on
`Globals::status->ship`. If the returned item is non-null, it calls
`Item::setUnsaleable(true)` (`0xaa80c`–`0xaa82a`). It then creates
ordinary `Mission(11,0,101)` (`0xaa82e`–`0xaa840`). PLT
relocations identify both methods. The previous source had the
mission tuple but omitted the equipment update. Step 75 already
matches its original ordinary mission constructor path.

## Recovery and verification

The source now sets the unsaleable flag when equipment of sort `18`
exists. A smoke test extracts the production implementation and
runs it at `-O0` and `-O2` through step 76. It checks both the
present and absent equipment paths and the mission tuple.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **31.1% -> 31.6%**; strict fuzzy
  **9.1% -> 10.8%**, with **802** generated instructions against
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
unverified. Next inspect step 77's equipment-dependent branch.
