# Campaign transition, step 74 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 74 to `0xaa7c0`. The original
loads `Globals::status->station` and calls `Station::removeShips()`
through PLT `0x638ac` (`0xaa7c0`–`0xaa7d4`). It then constructs
ordinary `Mission(11,0,100)` and joins the ordinary insertion path
(`0xaa7d8`–`0xaa7ea`). The previous source had the mission tuple
but omitted station ship removal. Steps 72–73 and 75 already match
their original ordinary mission constructor paths.

## Recovery and verification

The source now removes station ships before constructing the
mission. A smoke test extracts the production implementation and
runs it at `-O0` and `-O2` through step 74. It checks that this
removal occurs only at step 74 and that the surrounding missions
retain their expected tuples.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **30.7% -> 31.1%**; strict fuzzy
  **9.4% -> 9.1%**, with **789** generated instructions against
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
unverified. Next inspect step 76.
