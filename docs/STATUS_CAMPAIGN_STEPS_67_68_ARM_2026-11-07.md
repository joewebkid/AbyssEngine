# Campaign transitions, steps 67–68 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 67 to `0xaa68e`. It constructs
campaign `Mission(8,0,66)` and calls
`Mission::setProductionGoods(175,1)` through the tail thunk at
`0x19bf48` (`0xaa68e`–`0xaa6b6`). The previous source created the
mission but omitted the production-goods setup.

Step 68 starts at `0xaa6ba`. It calls `Ship::removeCargo(175)` on
`Globals::status->ship`, then creates ordinary `Mission(6,0,66)`
(`0xaa6ba`–`0xaa6e6`). The PLT relocation at `0x63894` identifies
the one-argument cargo removal. The previous source only created
the mission. Steps 69–70 already follow the original ordinary
mission constructor paths.

## Recovery and verification

The source now sets the production goods and removes cargo before
constructing step 68's mission. A smoke test extracts the production
implementation and runs it at `-O0` and `-O2` through step 68. It
checks both mission tuples and the goods/cargo calls.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **28.9% -> 29.5%**; strict fuzzy
  **7.8% -> 11.9%**, with **742** generated instructions against
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
unverified. Next inspect step 71's blueprint and mission effects.
