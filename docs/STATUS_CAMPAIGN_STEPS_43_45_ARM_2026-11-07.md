# Campaign transitions, steps 43–45 (Android ARM32)

Date: 2026-11-07

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 43 to `0xab44e`, where the original
constructs `Mission(11,0,10)` and joins the campaign setter at
`0xa9c30`. Step 44 branches to `0xa9c40`: it stores `-10` into Status
fields `0x7c/0x80`, constructs a default `Mission`, assigns it to the
campaign slot, hides the first mission-array entry, then adds 40,000
to credits at Status offset `0x1b0` and masks a negative 32-bit
result to zero (`0xa9c72`–`0xa9c80`). Step 45 branches to `0xa9ee4`
and returns through `0xab96a` without constructing a mission. The
previous source made step 43 ordinary, omitted step 44's field,
visibility, and credit effects, and created an ordinary mission at
step 45.

## Recovery and verification

The source follows these paths. The production-source smoke runs
steps 0–44 at `-O0` and `-O2`, checks step 44's normal credit award,
visibility, fields, and campaign slot, then verifies that step 45
creates no mission. A separate step 44 invocation checks the
original 32-bit overflow-and-clamp behavior.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **14.4% -> 15.0%**; strict fuzzy
  **6.2% -> 5.8%**, with **327** generated instructions against
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
unverified. Next inspect step 46 and the ship changes at steps 47–48.
