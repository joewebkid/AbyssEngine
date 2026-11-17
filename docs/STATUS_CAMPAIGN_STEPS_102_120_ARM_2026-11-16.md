# Campaign transitions, steps 102–120 (Android ARM32)

Date: 2026-11-16

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb-2 branch table (`0xa9ac6: tbh [pc, r1, lsl #1]`, base
`0xa9aca`) dispatches steps 102–120 (`0x66`–`0x78`) as follows:

- **Step 102 (`0x66`) -> `0xaac60`:** constructs ordinary
  `Mission(11, 0, 10)` (`0xb, 0, 10`) and branches to `0xa9c30`.
- **Step 103 (`0x67`) -> `0xaac7e`–`0xaaca4`, `0xab7f4`–`0xab854`:**
  scans `this->bluePrints`; for the first blueprint with
  `getIndex() == 206` (`0xce`), calls `BluePrint::unlock()` (`0xab7fc`)
  and `BluePrint::addItem((*Globals::items)[163]->makeItem(), 10, 10)`
  (`0xab812`–`0xab822`), then constructs campaign `Mission(166, 0, 10)`
  (`0xa6, 0, 10`) and calls `setProductionGoods(206, 1)`
  (`0xab840`–`0xab854` via veneer `0x19bf48 -> 0x61ca4`).
- **Steps 104 (`0x68`) and 105 (`0x69`) -> `0xaaca8`, `0xaacc6`:**
  construct ordinary `Mission(4, 0, 109)` and `Mission(4, 0, 111)` and
  branch to `0xa9c30`.
- **Step 106 (`0x6a`) -> `0xa9ee4`:** stores `currentCampaignMission`
  and `prevTimeLo`/`prevTimeHi`, then returns at `0xab96a` without
  constructing a mission.
- **Step 107 (`0x6b`) -> `0xaace4`:** constructs ordinary
  `Mission(11, 0, 10)` and branches to `0xa9c30`.
- **Step 108 (`0x6c`) -> `0xaad02`:** constructs `Mission(170, 0, 114)`
  (`0xaa, 0, 0x72`) and branches to `0xaba12` (`setCampaignMission` and
  `setStatusValue(0)`).
- **Steps 109 (`0x6d`) and 110 (`0x6e`) -> `0xaa278`, `0xaad20`:**
  construct ordinary `Mission(171, 0, 10)` and `Mission(171, 0, 38)` and
  branch to `0xa9c30`.
- **Step 111 (`0x6f`) -> `0xaad3e`:** constructs campaign
  `Mission(172, 0, 38)` (`0xac, 0, 0x26`, `setCampaignMission` at
  `0xaad5c`) and branches to `0xab846` with `r1 = 146` (`0x92`) for
  `setProductionGoods(146, 1)`.
- **Step 112 (`0x70`) -> `0xaad6a`:** calls
  `Globals::status->ship->removeCargo(146, 1)` (`0xaad6c`–`0xaad82`),
  constructs ordinary `Mission(171, 0, 82)`, and branches to `0xa9c30`.
- **Steps 113 (`0x71`) and 114 (`0x72`) -> `0xaa294`, `0xaad9c`:**
  construct ordinary `Mission(4, 0, 83)` and `Mission(171, 0, 82)` and
  branch to `0xa9c30`.
- **Step 115 (`0x73`) -> `0xaadba`:** constructs `Mission(171, 0, 93)`
  (`0xab, 0, 0x5d`) and branches to `0xaba12` (`setCampaignMission` and
  `setStatusValue(0)`).
- **Step 116 (`0x74`) -> `0xaadd8`:** writes
  `(*this->systemVisibilities)[30] = true` (`strb r1, [r0, #30]` at
  `0xaade6`), constructs ordinary `Mission(11, 0, 126)`, and branches to
  `0xa9c30`.
- **Step 117 (`0x75`) -> `0xaadfe`:** constructs `Mission(8, 0, 126)`
  (`0x8, 0, 0x7e`) and branches to `0xaae8a` (`setCampaignMission` and
  `setProductionGoods(209, 1)` via `0xab846`).
- **Step 118 (`0x76`) -> `0xaae1a`–`0xaae4e`, `0xab9f4`–`0xaba2c`:**
  calls `this->ship->getCargo()`; when non-null, marks the first cargo
  item with `getIndex() == 209` (`0xd1`) unsaleable
  (`setUnsaleable(true)` at `0xab9fc`), then constructs campaign
  `Mission(170, 0, 10)` with `setStatusValue(0)` (`0xaba08`–`0xaba2c`).
- **Step 119 (`0x77`) -> `0xaae52`:** constructs ordinary
  `Mission(4, 0, 40)` (`0x4, 0, 0x28`) and branches to `0xa9c30`.
- **Step 120 (`0x78`) -> `0xaae70`:** constructs campaign
  `Mission(8, 0, 93)` (`0x8, 0, 0x5d`, `setCampaignMission` at
  `0xaae8e`) and calls `setProductionGoods(209, 1)`
  (`0xaae96`–`0xaae98` -> `0xab846`).

## Recovery and checks

`kSteps[0x66..0x78]` now hold all nineteen tuples, pre-mission branches
handle blueprint `206` unlock/item addition (step 103), early return
(step 106), cargo `146` removal (step 112), system visibility `30`
(step 116), and cargo `209` unsaleable marking (step 118), and campaign
post-mission branches handle `setStatusValue(0)` (steps 108, 115, 118)
and `setProductionGoods` (steps 103, 111, 117, 120). The smoke runner
extracts the production implementation and verifies steps 102–120 at
`-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Both exact membership
  sets have zero losses and zero gains relative to the steps 91–101 run.
  The `Status` unit retains **103 linked-exact** and **66 byte-exact**
  functions.
- Focused `nextCampaignMission`: source-shape **38.3% -> 38.5%**,
  strict fuzzy **12.1% -> 14.3%**; generated instructions **1,038 ->
  1,017** against 2,884 original. This function remains non-exact.

Live campaign progression and full ARM32 shared linking remain
unverified. An ARM64 shared link does not establish a playable APK.
Next campaign transition to inspect: step 121 (`0x79` -> `0xaae9c`).
