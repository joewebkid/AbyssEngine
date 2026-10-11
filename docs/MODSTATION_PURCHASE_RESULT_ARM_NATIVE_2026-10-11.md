# ModStation purchase-result handoff (2026-10-11)

Scope: the purchase/restore result branch in `ModStation::OnUpdate`, its
application-data accesses in `OnTouchEnd`, and the called product-ID lookup.
Primary evidence is Android 2.0.16
`libgof2hdaa.so`, `ModStation::OnUpdate` at `0x000d9c78`:

- `0xd9dc2..0xd9df2`: byte `GameData+0x42` displays text 100 and is cleared.
- `0xd9dfc..0xd9e18`: byte `+0x41` gates the successful purchase; `+0x58` is
  a pointer whose first integer must be nonzero, and code `+0x48` must be
  50..54.
- `0xd9e1a..0xd9e30`: the product-ID list passed to
  `Globals::getInAppPurchaseArrayIndex` comes from `+0x54`. A **separate**
  `Array<int>` at `+0x60` supplies the indexed credit amount and its sign test.
- `0xd9e3c..0xd9e80`: the amount is passed to `Status::changeCredits`, followed
  by autosave, text 125, clearing `+0x41`, and hiding the hangar message.
- `Globals::getInAppPurchaseArrayIndex` at `0xe8c88` builds IDs from
  `www.fishlabs.net.gof2hd` (`.rodata 0x1cb17a`), `.` (`0x1ca487`), and
  suffixes at `0x1cb192..0x1cb1e8`: `dlc1`..`dlc5` for codes 0..4 and
  `dlcCredits1`, `dlcCredits2`, `dlcCredits3`, `dlcCredits4.0`,
  `dlcCredits5` for codes 50..54.

The old source passed the `+0x60` array as a string-ID list, interpreted its
element as a pointer, and called an unresolved one-argument credit stub without
the amount. It also used `int`-returning application-data fragments that truncate
native pointers. The lookup had empty ID literals and dereferenced a null
placeholder for the original stack protector. `GameData` now types
`+0x54/+0x58/+0x60` separately, with ARM32 offset assertions; the lookup uses
the original ID literals and no null protector placeholder; `ModStation` calls
the real application-data getter, credit,
notice, hide-message and menu-destruction methods. Its two `OnTouchEnd` writes
to `+0x0d` use the same typed object. The `GameData` constructor and other
JNI consumers were checked through native and ARM builds.

Focused `OnUpdate` changes from **65.7%/10.5%** to **66.7%/28.3%**
source-shape/strict fuzzy. `OnTouchEnd(int,int,void*)` changes from
65.4%/12.1% to 65.7%/12.4%. Native `gof2`, 206/206 ARM translation units and
36 verifier tests (one optional skip) pass. The lookup itself stays at 69.2%
source-shape; strict fuzzy changes from 28.2% to 25.4% because the old
null-dereferencing protector placeholder was removed for correct native
behavior. Full report:
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_modstation_purchase_20261011.json`.
It compares **4,323/4,524** originals and 4,689 rows: **91.89%** mean
source-shape, **79.98%** strict fuzzy, **2,319 linked-exact** and **997
byte-exact**. Compared with the preceding GameData report, no comparison row
or exact pair was lost; missing remains 201, wrong-type and skipped zero.

This is not an end-to-end billing test. Native/Java producers of the three
purchase pointers are still unverified; the original path does not validate
the lookup index before reading the credit array, and the much larger
`OnUpdate` function still contains unresolved fragment calls. A native
purchase-result smoke has not been run. Next trace the
producer/ownership of `GameData+0x54/+0x58/+0x60` and exercise a representative
purchase result when that path is callable on the host.

The later [Android/iOS producer cross-check](PURCHASE_APPDATA_PRODUCER_CROSS_PLATFORM_2026-10-11.md)
found an iOS StoreKit array producer but no confirmed Android producer. The
Android Google Play consumable JNI callback uses a separate direct credit path.
