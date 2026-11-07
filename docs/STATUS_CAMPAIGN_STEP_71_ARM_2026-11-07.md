# Campaign transition, step 71 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 71 to `0xaa726`. The original
iterates all blueprints, unlocking every one with index `183`
(`0xaa732`–`0xaa754`). It makes one copy of catalog item `175` and
passes it to `Globals::status->ship->addCargo` (`0xaa756`–`0xaa77a`).
It then constructs campaign `Mission(164,150000,101)` and clears
`Status::field_10c` (`0xaa77e`–`0xaa79a`). The signed integer literal
at `0xaaa18` is `150000`. PLT relocations identify
`BluePrint::unlock`, `Item::makeItem(int)`, and `Ship::addCargo`.
The previous source only constructed `Mission(164,0,101)`.

## Recovery and verification

The source now follows the original order. A smoke test extracts
the production implementation and runs it at `-O0` and `-O2`
through step 71. It checks all matching blueprints, the copied
item, mission tuple, and reset Status field. Steps 72–73 already
match their original ordinary mission constructor paths.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **29.5% -> 30.7%**; strict fuzzy
  **11.9% -> 9.4%**, with **782** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions. The fuzzy reduction is recorded;
  the recovered behavior follows the original call sequence.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved names,
  with no name added or removed.

- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect step 74's station ship removal.
