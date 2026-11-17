# Campaign transitions, steps 91–101 (Android ARM32)

Date: 2026-11-16

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb-2 branch table (`0xa9ac6: tbh [pc, r1, lsl #1]`, base
`0xa9aca`) dispatches steps 91–101 (`0x5b`–`0x65`) as follows:

- **Step 91 (`0x5b`) -> `0xaaa7a`:** constructs `Mission(184, 0, 113)`
  (`0xb8, 0, 0x71`), assigns it via `Status::setCampaignMission`
  (`0xaaa98`), calls `Mission::setProductionGoods(0, 10)` (`0xaaaa8`),
  and branches to `0xaba20` for `Mission::setStatusValue(10)`.
- **Step 92 (`0x5c`) -> `0xaaab6`:** constructs ordinary
  `Mission(11, 0, 114)` (`0xb, 0, 0x72`) and branches to `0xa9c30`.
- **Step 93 (`0x5d`) -> `0xaaad4`:** calls
  `(*Globals::items)[205]->makeItem(1)` (`offset 0x334` at
  `0xaaae8`–`0xaaaec`) and `Globals::status->ship->addCargo(...)`
  (`0xaaafa`–`0xaaafe`), sets `r6 = 0`, constructs campaign
  `Mission(184, 0, 111)` (`0xb8, 0, 0x6f`), calls
  `setProductionGoods(0, 83)` (`0xaab2a`), and branches to `0xab0e8` for
  `setStatusValue(83)` and `this->field_178 = 0` (`0xab0f0`).
- **Step 94 (`0x5e`) -> `0xaab36`:** constructs `Mission(170, 0, 10)`
  (`0xaa, 0, 0xa`) and branches to `0xaba12` (`setCampaignMission` at
  `0xaba16` and `setStatusValue(0)` at `0xaba1e`–`0xaba2c`).
- **Step 95 (`0x5f`) -> `0xaab54`:** constructs ordinary
  `Mission(11, 0, 98)` (`0xb, 0, 0x62`) and branches to `0xa9c30`.
- **Step 96 (`0x60`) -> `0xaab72`:** constructs ordinary
  `Mission(4, 0, 85)` (`0x4, 0, 0x55`) and branches to `0xa9c30`.
- **Step 97 (`0x61`) -> `0xaab90`:** sets
  `(*Globals::status->systemVisibilities)[29] = true` (`strb r1, [r0, #29]`
  at `0xaaba6`), constructs ordinary `Mission(11, 0, 120)`
  (`0xb, 0, 0x78`), and branches to `0xa9c30`.
- **Step 98 (`0x62`) -> `0xaabbe`:** constructs `Mission(170, 0, 10)`
  (`0xaa, 0, 0xa`) and branches to `0xaba12` (`setCampaignMission` and
  `setStatusValue(0)`).
- **Step 99 (`0x63`) -> `0xaa25c`:** constructs ordinary
  `Mission(4, 0, 98)` (`0x4, 0, 0x62`) and branches to `0xa9c30`.
- **Step 100 (`0x64`) -> `0xaabdc`:** constructs ordinary
  `Mission(11, 0, 98)` (`0xb, 0, 0x62`) and branches to `0xa9c30`.
- **Step 101 (`0x65`) -> `0xaabfa`:** calls
  `(*Globals::items)[207]->makeItem(1)` (`offset 0x33c` at
  `0xaac0e`–`0xaac12`) and `Globals::status->ship->addCargo(...)`
  (`0xaac20`–`0xaac24`), constructs campaign `Mission(184, 0, 113)`
  (`0xb8, 0, 0x71`, `setCampaignMission` at `0xaac3e`), calls
  `setProductionGoods(0, 1700)` (`0x6a4` at `0xaac48`–`0xaac50`), and
  branches to `0xaba20` for `setStatusValue(1700)` (`0xaac58`–`0xaac5c`).

## Recovery and checks

`kSteps[0x5b..0x65]` now hold all eleven tuples, pre-mission branches
handle cargo additions (`205` at step 93, `207` at step 101) and system
visibility (`29` at step 97), and campaign post-mission branches set
production goods, status values, and `field_178 = 0` (at step 93). The
smoke runner extracts the production implementation and checks steps
91–101 at `-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Both exact membership
  sets have zero losses and zero gains relative to the step 90 run. The
  `Status` unit retains **103 linked-exact** and **66 byte-exact**
  functions.
- Focused `nextCampaignMission`: source-shape **35.9% -> 38.3%**,
  strict fuzzy **12.2% -> 12.1%** (`ordered_control_flow_match`
  **14.0% -> 16.2%**); generated instructions **933 -> 1,038** against
  2,884 original. This function remains non-exact.

Live campaign progression and full ARM32 shared linking remain
unverified. An ARM64 shared link does not establish a playable APK.
Next campaign transition to inspect: step 102 (`0x66`).
