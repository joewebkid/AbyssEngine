# ModStation dialogue mission slot and production cargo (Android ARM)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ModStation::OnTouchEnd(int,int,void*)` starts at `0xd7600`; its first
dialogue mission branch is at `0xd768c–0xd76e4`. The original reads
the `Mission*` from station offset `0x9c`, calls `Mission::getType`
and `isCampaignMission`, and, for non-campaign type 8, passes both
`getProductionGoodIndex()` and `getProductionGoodAmount()` to
`Ship::removeCargo(int,int)` before refreshing the hangar. The ARM
`OnUpdate` writes the mission pointer at offset `0x9c` at `0xda594`
and `0xda756`. `OnTouchEnd` clears that same slot on mission completion
(for example `0xd7c1a–0xd7c1e`). Offset `0x54` holds a radio object in
the original (`0xd79ce` and `OnRelease` at `0xd91e4`).

The recovered code conflated the two slots, storing and reading the
dialogue mission at `0x54`; the original `0x9c` field was called
`introTimer`. The recovered `OnTouchEnd` also discarded the production
amount and called an unresolved cargo shim with only the item index.
The source now uses a typed `Mission* dialogueMission` field at `0x9c`
on ARM32 for the two `OnUpdate` writes, the dialogue reads and mission
clears.
The radio slot at `0x54` remains separate. The type-8 branch uses the
verified method calls and both cargo arguments. A 32-bit layout
assertion pins the new field name to `0x9c`.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 translation units. `OnTouchEnd`
source-shape improves **73.5% → 74.0%** (generated instructions
**1,132 → 1,145** versus original **1,925**); strict fuzzy changes
**15.0% → 13.9%**. The generated ARM branch now loads the mission from
`0x9c` and calls `Ship::removeCargo(int,int)` with both arguments.

Full ARM corpus: **4,323/4,524** original functions, **4,767** pairs,
**96.47%** mean source-shape, **86.35%** strict fuzzy,
**2,761 linked-exact / 1,098 byte-exact**, 201 missing, zero skipped.
No pairs or exact symbols are lost. The corpus means are unchanged
after rounding.

The rest of the dialogue mission path still contains unbound `*_ote`
calls and a null-initialized `g_ote_status` placeholder, including
the reward continuation after the type-8 cargo step. Live mission
completion and cargo changes remain unverified. The typed mission
pointer avoids truncating this field in a 64-bit host build; other
integer-held pointers in the station still need recovery. Next restore
the status and reward continuation with typed calls and confirmed ARM
control flow. Redmi testing stays stopped.
