# Campaign transitions, steps 49–53 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends steps 49–51 to `0xa9ef0`, `0xab722`,
and `0xaa086`. They construct campaign `Mission(156,0,62)`,
`Mission(156,0,25)`, and `Mission(160,0,25)`, respectively. Step 51
joins the path at `0xab51a`, which assigns the mission to the
campaign slot and calls `Mission::setVisible(false)` on its first
array entry.

Step 52 branches back to `0xa9c0e`, the shared achievement-check
loop. It advances from step 52 to 53 and then to 54, checking the
achievement table for both values before dispatching the step 53
branch. That branch at `0xaa0a4` constructs campaign
`Mission(11,200000,74)`; the second constructor argument comes from
the literal at `0xaa128`. The previous source made steps 49–51
ordinary, left step 51 visible, created a default ordinary mission
at step 52, and omitted the 200,000 parameter at step 53.

## Recovery and verification

The source now uses a shared loop for the step 52 redispatch and
restores the mission slots, visibility, and constructor argument.
The production-source smoke covers steps 0–53 at `-O0` and `-O2`;
it separately checks achievement matches on both passes through
step 52 and the final counter value 54.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape remains **19.6%** and strict
  fuzzy changes **6.6% -> 6.5%**, with **449** generated instructions
  against **2,884** original. The `Status` unit retains **103
  linked-exact** and **66 byte-exact** functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. The 211 ARM32 objects retain **174** raw unresolved
  names, with no name added or removed.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect step 54's restored-ship path.
