# ModStation type-0 and type-14 cargo completion (Android ARM)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
The relevant `ModStation::OnTouchEnd(int,int,void*)` path is
`0xd77ba–0xd7962`.

The original skips this cargo handling for mission types 3, 5 and 11.
For type 0, it searches the ship's cargo for the first unsaleable
`Item` with index `0x74` or `0x75`, removes that `Item*`, and refreshes
the hangar when it exists. It then adds
`Mission::getProductionGoodAmount()` to the status word at offset
`0x9c` (`0xd794c–0xd795e`). The update occurs even when cargo is null
or no item matches. For type 14, it finds the first cargo item with
index `0x73` without testing `isUnsaleable`, removes that `Item*`, and
refreshes the hangar when it exists. It does not update the status
word. The ARM loops and branches at `0xd781c–0xd7836` and
`0xd78b2–0xd7948` confirm those distinct conditions.

The recovered source used unbound `*_ote` shims and passed a ship
pointer cast to `Item*` into the cargo-removal shim. It also guarded
the type-0 status update with `cargo != nullptr`, contrary to the ARM
branch. This path now calls the typed mission, ship and item methods,
passes the actual cargo element to `Ship::removeCargo(Item*)`, and
performs the type-0 status update after the cargo search.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 units. `OnTouchEnd` source-shape rises
**76.4% → 76.9%**, strict fuzzy changes **15.2% → 13.0%**, and
generated instructions **1,202 → 1,220** versus original **1,925**.
The ARM object retains the type-0 `Status+0x9c` update after the cargo
path.

Full ARM corpus: **4,323/4,524** original functions over **4,767**
pairs, **96.47%** mean source-shape, **86.35%** strict fuzzy,
**2,761 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. No pair or exact-set losses; corpus means are unchanged after
rounding. Live cargo and status effects remain unverified. The
campaign mission continuation of `OnTouchEnd` still contains unbound
calls and is the next priority. Redmi testing stays stopped.
