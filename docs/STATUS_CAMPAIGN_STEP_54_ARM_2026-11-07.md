# Campaign transition, step 54 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 54 to `0xaa38c`. The original
sets element `23` of the visibility array at Status offset `0x38`
to true (`0xaa396`–`0xaa39a`), constructs and assigns campaign
`Mission(11,0,101)`, passes the saved ship from Status offset `0x8c`
to `Status::setShip`, clears that saved pointer, and returns
(`0xaa39c`–`0xaa3c6`). The previous source already had the mission
tuple and campaign slot but omitted the visibility and ship effects.

## Recovery and verification

The source now follows that order. The production-source smoke runs
the restored sequence through step 54 at `-O0` and `-O2`, checking
the mission tuple, visibility element, restored ship identity, and
cleared saved pointer. The temporary ship from step 48 is replaced
through the existing `Status::setShip` contract.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **19.6% -> 20.4%** and strict
  fuzzy **6.5% -> 7.7%**, with **473** generated instructions against
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
unverified. Next inspect step 55.
