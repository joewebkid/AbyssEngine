# Campaign transitions, steps 27–29 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends steps 27, 28, and 29 to `0xab48a`,
`0xaa36e`, and `0xa9fe8`. Step 27 writes `18` and `91` to Status
offsets `0x7c/0x80`, then constructs `Mission(4, 0, 91)`.
Step 28 constructs `Mission(4, 0, -1)` and step 29 constructs
`Mission(156, 0, 91)`. All three paths join the campaign-mission
setter at `0xa9c30`. The previous source had the constructor tuples
but added the missions outside the campaign slot and omitted the
step-27 field writes.

## Recovery and verification

The source now preserves the paired fields and classifies all three
new missions as campaign missions. The production-source smoke runs
steps 0–29 at `-O0` and `-O2`, including the step-27 fields.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **12.1% -> 12.3%** and strict
  fuzzy **4.6% -> 5.3%**; generated instructions **256 -> 260**
  against **2,884** original. The `Status` unit retains 103
  linked-exact and 66 byte-exact functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. Raw ARM32 unresolved names remain **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression remains unverified. Next inspect steps
30–32, including the post-construction path on step 32.
