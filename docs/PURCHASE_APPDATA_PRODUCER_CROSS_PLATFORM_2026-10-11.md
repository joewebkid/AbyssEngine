# Purchase application-data producer: Android/iOS cross-check (2026-10-11)

Android 2.0.16 `ModStation::OnUpdate` (`libgof2hdaa.so`, `0xd9c78`, purchase
branch `0xd9dfc..0xd9e80`) reads a product-ID array at application-data
`+0x54`, a nonempty-count pointer at `+0x58`, and an indexed integer result
array at `+0x60`. `OnCreateApplication` initializes these three pointers to
null. A search of the recovered Android C++ tree found no later assignment
to them; `ModStation` remains their only recovered reader. That source search
is not proof that the original binary has no producer.

The Android Google Play consumable callback at `0x6b9ac` instead forwards
an index to `ndk_iapBoughtConsumable` at `0x945d8`, which selects a fixed
credit amount and calls `setBaughtCredits` at `0x9462c`. This route does not
write the three application-data pointers. The `ModStation` branch therefore
must not be wired to that callback merely because both concern credits.

The separate iOS Hex-Rays dump
`analysis/gof2_ios_ida_full_dump.c` supplies a semantic cross-check, not an
Android layout source. In `-[AppController productsRequest:didReceiveResponse:]`
at `0x1e8f20`, the StoreKit product response releases previous arrays,
allocates fresh array containers, appends product identifiers and localized
titles/descriptions, and appends an integer per product. The integer path
uses `sub_337CF4` at `0x337cf4` (`atoi` of a temporary string) or stores
`-1`, depending on the product-ID branch. The response code explicitly
mentions `dlcCredits1` through `dlcCredits5`, including `dlcCredits4.0`.
`-[AppController request:makePurchase:]` at `0x1e7f24` checks a populated
list and records the selected product code; `completeTransaction:` at
`0x1ea5b0` forwards the product ID to `provideContent:` at `0x1ea750`.
These paths corroborate the ID-list/indexed-value interpretation but their
data offsets differ from Android: for example, the iOS request writes its
selected code at `+0x3c`, whereas Android `GameData::purchaseCode` is at
`+0x48`. The iOS offsets must not be copied into Android `GameData`.

No Android producer, ownership/destruction contract, or end-to-end billing
test is confirmed yet. Do not allocate the three pointers in JNI or invent a
store response to satisfy `ModStation` without an Android write trace or a
deliberately separate native-host service design. Next priority: trace ARM
writes to the application-data fields/flags, then test the station consumer
with controlled inputs once its surrounding calls are isolated. The last
source-changing package's full ARM report remains
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_consumable_20261011.json`:
**4,323/4,524** functions, **91.91%** source-shape, **80.00%** strict,
**2,319 linked-exact** and **997 byte-exact**. This audit changes no C++
code, so it makes no new matching claim.
