# Campaign transition, step 59 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 59 to `0xaa18a`. The original
loads `missions[0]`, calls `Mission::getStatusValue()`, and computes
`(previousStatus + 1) * 50000` with ARM `mla` (`0xaa192`–`0xaa1ac`).
It constructs campaign `Mission(11,amount,101)`, then sets the new
mission's status value to `1` if the previous value was positive,
otherwise `0` (`0xaa1b0`–`0xaa1d2`, tail thunk `0x19bf58`). The PLT
relocations resolve the getter and setter to `Mission` methods. The
previous source always constructed `Mission(11,50000,101)` and did
not set the new status value.

## Recovery and verification

The source now computes the amount with 32-bit wraparound, matching
the ARM multiply-add, and sets the derived status flag. A smoke test
extracts the production implementation and runs it at `-O0` and
`-O2` through step 59. It checks previous status values `0`, `2`,
and `-1`, producing amounts `50000`, `150000`, and `0` and flags
`0`, `1`, and `0`, respectively.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **26.0% -> 27.1%**; strict fuzzy
  **9.2% -> 9.6%**, with **658** generated instructions against
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
unverified. Step 60's ordinary `Mission(164,0,101)` already matches
the original constructor path at `0xaa50c`–`0xaa526`. Next inspect
step 61.
