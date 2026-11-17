# Campaign transition, step 90 (Android ARM32)

Date: 2026-11-16

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb-2 branch table (`0xa9ac6: tbh [pc, r1, lsl #1]`, base
`0xa9aca`) sends step 90 (`0x5a`) to `0xaaa38`:

- `0xaaa38`–`0xaaa48`: stores `next` (`91`) to
  `this->currentCampaignMission` (`+0x1e8`) and `prevTimeLo`/`prevTimeHi`
  to `+0x100`/`+0x104`, loads `this->systemVisibilities` (`[r4, #0x38]`),
  and writes `0x0101` (`strh.w r1, [r0, #27]`) to mark systems `27` and
  `28` visible.
- `0xaaa4c`–`0xaaa64`: allocates `0x78` bytes, sets `r6 = 0`, calls
  `Mission::Mission(184, 0, 110)` (`0xb8, 0, 0x6e`) through the PLT at
  `0x63834`, and calls `Status::setCampaignMission(Mission *)` through
  the PLT at `0x63840`.
- `0xaaa68`–`0xaaa78`: loads `(*this->missions)[0]` (`[r4, #0x198]`),
  calls `Mission::setProductionGoods(0, 10)` through the PLT at
  `0x61ca4`, and branches to `0xab0e2`.
- `0xab0e2`–`0xab0f4`: loads `(*this->missions)[0]`, calls
  `Mission::setStatusValue(10)` through the PLT at `0x62898`, stores
  `r6` (`0`) into `this->field_178` (`[r4, #0x178]`), and branches to
  the return epilogue at `0xab96a`.

The prior source fell through the end of `kSteps` into the fallback
ordinary `Mission(11, 0, 100)`, omitting visibility updates, campaign
`Mission(184, 0, 110)`, production goods `(0, 10)`, status value `10`,
and clearing `field_178`.

## Recovery and checks

`kSteps[0x5a]` now holds `{0xb8, 0, 0x6e, 1}`, the pre-mission branch
for `step == 90` marks `systemVisibilities[27]` and `[28]` true, and
the campaign post-mission branch calls `setProductionGoods(0, 10)`,
`setStatusValue(10)`, and sets `field_178 = 0`. The smoke runner
extracts the production implementation and checks all of these effects
at `-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Both exact membership
  sets have zero losses and zero gains relative to the preceding
  steps 87–89 alignment run. The `Status` unit retains **103
  linked-exact** and **66 byte-exact** functions.
- Focused `nextCampaignMission`: source-shape **35.2% -> 35.9%**,
  strict fuzzy **11.3% -> 12.2%**; generated instructions **915 ->
  933** against 2,884 original. This function remains non-exact.

Live campaign progression and full ARM32 shared linking remain
unverified. An ARM64 shared link does not establish a playable APK.
Next campaign transition to inspect: step 91 (`0x5b` -> `0xaaa7a`).
