# Android consumable purchase dispatch (2026-10-11)

The Android 2.0.16 JNI export
`Java_net_fishlabs_googleplay_ToJNI_iapBoughtConsumable` at `0x6b9ac`
forwards the Java index to `ndk_iapBoughtConsumable` at `0x945d8`.
That function rejects indexes outside 0..4 or a missing `gEngine`, then
selects a credit amount with a Thumb `tbb` switch and calls
`setBaughtCredits`. The table and literal pool map indexes 0..4 to
100,000; 300,000; 1,000,000; 3,000,000; and 10,000,000 credits.
`setBaughtCredits` at `0x9462c` calls `Status::changeCredits`, clears the
matching pending flag if set, optionally records first-pack state, and
autosaves. It measures 100.0% source-shape and strict fuzzy, but is not
linked-exact.

The previous recovered dispatch used an indexed constant array. Its focused
ARM scores were **36.3% source-shape / 39.1% strict fuzzy**. An explicit
switch with one call in each case raises them to **70.2% / 83.3%**. A
single post-switch call compiled back to the old indexed array and was
rejected. The remaining difference is mainly Clang tail-calling the local
helper with `b` instead of the original PLT `blx` plus return; no artificial
post-call work was added solely to suppress that optimization.

The opt-in native IAP smoke now exercises all five index-to-amount mappings
through the production dispatch, plus invalid-index and missing-engine gates.
It stubs `setBaughtCredits`, so it validates the dispatch contract rather
than actual store billing, credit persistence, or Java lifecycle. Native
`gof2`, the smoke, 206/206 ARM translation units, and verifier tests pass.
The full corpus compares **4,323/4,524** original functions and 4,689 rows:
**91.91%** mean source-shape, **80.00%** strict fuzzy, **2,319 linked-exact**
and **997 byte-exact**. Against the preceding item-copy report, no exact pair
was lost; 201 missing, zero wrong-type and zero skipped remain. Full report:
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_consumable_20261011.json`.

This Android JNI path does **not** populate the `GameData+0x54/+0x58/+0x60`
purchase fields read by `ModStation`. That observation does not prove no
other original producer exists. The producer and ownership of those fields,
and end-to-end billing on a device, remain unverified.

Next priority: trace other ARM writes to those three `GameData` offsets or
cross-check iOS/PC application-data producers before adding a native owner.
