# Campaign transition, step 77 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 77 to `0xaa1f2`. The original
looks for ship equipment of sort `18`. If found, it calls
`Ship::removeEquipment` on that item (`0xaa1f2`–`0xaa21e`). If
absent, it looks for cargo item `85` and removes the returned item
when present (`0xab970`–`0xab980`). It then iterates every blueprint:
each nonempty blueprint with station index `101` is reset
(`0xab984`–`0xab9b8`). Finally it creates ordinary
`Mission(4,0,101)` (`0xab9ba`–`0xab9cc`). PLT relocations identify
all ship and blueprint methods. The previous source had only the
mission tuple.

## Recovery and verification

The source now follows these branches and resets every matching
blueprint. A smoke test extracts the production implementation and
runs it at `-O0` and `-O2` through step 77. It checks equipment
present, cargo fallback, and both absent, plus two matching
blueprints and nonmatching/empty blueprints.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **31.6% -> 33.2%**; strict fuzzy
  **10.8% -> 12.4%**, with **844** generated instructions against
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
unverified. Steps 78–82 already match their original mission
constructor paths. Next inspect step 83's cargo handling.
