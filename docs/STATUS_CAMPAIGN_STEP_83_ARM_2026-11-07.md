# Campaign transition, step 83 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 83 to `0xaa8be`. The original
looks for ship equipment of sort `18`. If found, it calls
`Item::setUnsaleable(false)` on that item. Otherwise it looks for
cargo item `85` and clears the same flag when that item exists
(`0xaa8be`–`0xaa8ea`). It then copies catalog item `85` with
`Item::makeItem(1)`, adds the copy to the ship's cargo, and creates
an ordinary empty `Mission()` (`0xaa8ee`–`0xaa91c`). PLT
relocations identify the item, ship, and mission methods. The
previous source created the empty mission but omitted the item and
cargo effects.

## Recovery and verification

The source now follows all three branches and then adds the item
copy. A smoke test extracts the production implementation and runs
it at `-O0` and `-O2` through step 83. It checks equipment present,
cargo fallback, and neither present, plus the new catalog copy and
ordinary mission. Steps 78–82 already match their original mission
constructor paths.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **33.2% -> 34.5%**; strict fuzzy
  **12.4% -> 12.9%**, with **889** generated instructions against
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
unverified. Steps 84–86 match their original mission constructor
paths. Next inspect step 87.
