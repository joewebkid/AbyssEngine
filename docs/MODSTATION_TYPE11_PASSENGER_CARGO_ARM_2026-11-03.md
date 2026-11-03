# ModStation type-11 passenger and cargo completion (Android ARM)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
The relevant `ModStation::OnTouchEnd(int,int,void*)` path is
`0xd778c–0xd78b0`.

After a non-campaign mission of type 11, the original calls
`Status::setPassengers(0)`, checks `Mission::isCampaignMission` again,
and adds `Mission::getProductionGoodAmount()` to the status word at
offset `0xb8` when it is non-campaign. It obtains the current ship's
cargo and searches in order for the first `Item` that is unsaleable
and has index `0x74` or `0x75`. It calls `Ship::removeCargo(Item*)`
with that actual array element, conditionally refreshes the hangar,
and continues to the shared reward path. If no item matches, it
continues without removing cargo. The ARM loop and branches at
`0xd7862–0xd78b0` confirm these conditions and call arguments.

The recovered source used unbound shims for the mission, status, ship
and item calls; the cargo-removal shim received a cast ship pointer
instead of the `Item*`. This branch now uses typed methods and
`Globals::status`, preserving the original order and the `0xb8`
field offset. Other mission-type branches remain separate.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 units. `OnTouchEnd` source-shape rises
**75.7% → 76.4%**, strict fuzzy **13.6% → 15.2%**, and generated
instructions **1,188 → 1,202** versus original **1,925**.

Full ARM corpus: **4,323/4,524** original functions over **4,767**
pairs, **96.47%** mean source-shape, **86.35%** strict fuzzy,
**2,761 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. No pair or exact-set losses; corpus means are unchanged after
rounding. Live passenger, cargo and reward behavior remain unverified.
Next restore the type-0 and type-14 cargo branches; Redmi testing stays
stopped.
