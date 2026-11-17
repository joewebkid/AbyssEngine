# Campaign transitions, steps 121–161 and >161 fallback (Android ARM32)

Date: 2026-11-16

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb-2 branch table (`0xa9ac6: tbh [pc, r1, lsl #1]`, base
`0xa9aca`, 162 entries `0..161`) and the out-of-range guard
(`cmp r1, #0xa1; bhi.w #0xa9ee4` at `0xa9ac0`–`0xa9ac2`) dispatch steps
121–161 (`0x79`–`0xa1`) and `> 161` as follows:

- **Step 121 (`0x79`) -> `0xaae9c`–`0xaaee8`, `0xaba30`–`0xaba62`:**
  scans `this->ship->getCargo()`; when non-null, clears
  `setUnsaleable(false)` (`0xaba38`) on the first item with
  `getIndex() == 209` (`0xd1`), calls
  `Globals::status->ship->removeCargo(209, 1)` (`0xaba3c`–`0xaba4c`),
  and constructs ordinary `Mission(11, 0, 10)` (`0xaba50`–`0xaba62`).
- **Steps 122–124 (`0x7a`–`0x7c`) -> `0xaaeec`, `0xaaf0a`, `0xaaf28`:**
  construct ordinary `Mission(4, 0, 121)`, `Mission(11, 0, 121)`, and
  `Mission(4, 0, 55)`.
- **Step 125 (`0x7d`) -> `0xaaf46`:** constructs campaign
  `Mission(170, 0, 120)` with `setStatusValue(0)` (`0xaba12`).
- **Step 126 (`0x7e`) -> `0xaaf64`:** constructs ordinary
  `Mission(11, 0, 98)`.
- **Steps 127 (`0x7f`), 129 (`0x81`), 161 (`0xa1`) -> `0xaaf82`,
  `0xaaf98`, `0xab374`:** construct default `Mission()` (`0x6384c`) and
  branch to `0xab51a` (`setCampaignMission` + `setVisible(false)`).
- **Step 128 (`0x80`) -> `0xa9c0e`:** re-enters the top achievement
  loop (`step = next`), checking `g_ncmAchTable` on `129` and advancing
  to dispatch step 129 (`next = 130`).
- **Steps 130–131 (`0x82`–`0x83`) -> `0xaafae`, `0xaafcc`:** construct
  ordinary `Mission(4, 0, 112)` and `Mission(11, 0, 112)`.
- **Steps 132–133 (`0x84`–`0x85`) -> `0xaafea`, `0xab008`:** construct
  campaign `Mission(170, 0, 120)` and `Mission(11, 0, 22)` with
  `setStatusValue(0)` (`0xaba12`).
- **Step 134 (`0x86`) -> `0xab026`:** constructs campaign
  `Mission(174, 0, 103)` (`0xae, 0, 0x67`), calls
  `setProductionGoods(155, 140)` (`0xab04c`–`0xab054`), and branches to
  `0xaba1a` for `setStatusValue(0)`.
- **Steps 135–137 (`0x87`–`0x89`) -> `0xab05c`, `0xab07a`, `0xab098`:**
  construct ordinary `Mission(11, 0, 112)`, `Mission(4, 0, 58)`, and
  `Mission(11, 0, 58)`.
- **Step 138 (`0x8a`) -> `0xab0b6`–`0xab0f4`:** writes
  `(*this->systemVisibilities)[31] = true` (`strb r1, [r0, #31]` at
  `0xab0c4`), constructs campaign `Mission(168, 0, 131)` (`0xa8, 0, 0x83`),
  calls `setStatusValue(10)` (`0xab0e6`–`0xab0ec`), and stores
  `this->field_178 = 0` (`0xab0f0`).
- **Step 139 (`0x8b`) -> `0xab0f8`:** constructs ordinary
  `Mission(11, 0, 112)`.
- **Step 140 (`0x8c`) -> `0xab116`–`0xab13c`, `0xab858`–`0xab904`:**
  scans `this->bluePrints`; for the first blueprint with
  `getIndex() == 210` (`0xd2`), calls `BluePrint::unlock()` (`0xab860`)
  and adds four items at station `112` via
  `BluePrint::addItem((*Globals::items)[idx]->makeItem(), qty, 112)`
  for `(201, 847)`, `(202, 834)`, `(203, 861)`, and `(204, 892)`, then
  constructs ordinary `Mission(11, 0, 78)` (`0xab8f2`–`0xab904`).
- **Step 141 (`0x8d`) -> `0xab13e`–`0xab1c8`:** calls
  `Globals::status->ship->addCargo` for `(*Globals::items)[197]->makeItem(15)`,
  `(*Globals::items)[196]->makeItem(1)`, and
  `(*Globals::items)[198]->makeItem(1)`, then constructs ordinary
  `Mission(4, 0, 79)`.
- **Step 142 (`0x8e`) -> `0xab1cc`:** constructs campaign
  `Mission(8, 0, 112)` and calls `setProductionGoods(210, 1)`
  (`0xab1f2`–`0xab1f4` -> `0xab846`).
- **Step 143 (`0x8f`) -> `0xab1f6`–`0xab224`:** calls
  `Globals::status->ship->removeCargo(210, 1)` (`0xab1f8`–`0xab20e`),
  constructs campaign `Mission(170, 0, 112)`, and sets
  `setStatusValue(0)` (`0xaba12`).
- **Steps 144–159 (`0x90`–`0x9f`):** construct ordinary missions
  `(4,0,112)`, `(11,0,112)`, `(4,0,-1)`, `(171,0,96)` (steps 147–150 at
  `0xa9c16`), `(165,0,-1)`, `(11,0,98)`, `(4,0,-1)`, `(164,0,0)`,
  `(11,0,99)`, `(4,0,112)`, `(4,0,111)`, `(11,0,10)`, and `(170,0,10)`.
- **Step 160 (`0xa0`) -> `0xab358`:** constructs campaign
  `Mission(170, 0, 93)` with `setStatusValue(0)` (`0xaba12`).
- **Default (`step > 161`) -> `0xa9ee4`:** stores `currentCampaignMission`
  and `field_0x100`/`field_0x104` and branches to `0xab96a` without
  creating a mission.

## Recovery and checks

`kSteps` now covers all 162 entries (`0x00..0xa1`), the achievement loop
re-enters for `step == 128`, pre/post branches implement steps 121–161,
and the out-of-range fallback (`step > 161`) returns without constructing
a mission. The production-code smoke runner verifies steps `0..162` at
`-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Both exact membership
  sets have zero losses and zero gains relative to the steps 102–120
  run. The `Status` unit retains **103 linked-exact** and **66
  byte-exact** functions.
- Focused `nextCampaignMission`: source-shape **38.5% -> 44.1%**,
  strict fuzzy **14.3% -> 12.8%** (`instruction_inventory_match`
  **46.0% -> 51.1%**, `opcode_inventory_match` **49.2% -> 55.7%**);
  generated instructions **1,017 -> 1,274** against 2,884 original.
  This function remains non-exact.

Live campaign progression and full ARM32 shared linking remain
unverified. An ARM64 shared link does not establish a playable APK.
All 162 `tbh` cases (`0..161`) and the `> 161` fallback in
`Status::nextCampaignMission(bool)` are now recovered; next priority
is auditing the remaining 201 missing functions or low-match `Status`
methods.
