# Campaign transitions, steps 61–62 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 61 to `0xaa52a` and step 62 to
`0xaa550`. Step 61 writes `1` to system visibility index `22`, then
constructs ordinary `Mission(11,0,100)` (`0xaa52a`–`0xaa54c`).
Step 62 writes `1` to visibility index `24`, then constructs ordinary
`Mission(4,0,103)` (`0xaa550`–`0xaa572`). Both paths join the ordinary
mission insertion at `0xa9c30`. The previous source had the correct
mission tuples but omitted both visibility updates. Step 60's
ordinary `Mission(164,0,101)` already matches its original path at
`0xaa50c`–`0xaa526`.

## Recovery and verification

The source now sets both visibility flags before constructing the
missions. A smoke test extracts the production implementation and
runs it at `-O0` and `-O2` through step 62. It checks the mission
tuples, ordinary insertion, and that each visibility flag changes at
its respective step.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **27.1% -> 27.3%**; strict fuzzy
  **9.6% -> 7.5%**, with **666** generated instructions against
  **2,884** original. The `Status` unit retains **103 linked-exact**
  and **66 byte-exact** functions. The fuzzy reduction is recorded;
  the restored visibility writes match the original behavior.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved names,
  with no name added or removed.

- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Steps 63–65 already have the original ordinary mission
tuples and insertion path. Next inspect step 66's equipment changes.
