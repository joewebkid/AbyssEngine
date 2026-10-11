# Android item-information reset (2026-10-11)

The Android 2.0.16 `ndk_setNativeItemInformationList` callback copies five
fields for each of five store items into `Globals`: ID, name, description,
currency, and price. The corresponding original ARM reset at `0x94e38`
checks each five-pointer group, calls `operator delete[]` on its members,
and clears that group. Its GOT references resolve to the same 25 named
globals, in that order.

The recovered reset previously used `cItemListID_05` through `_24` for
the latter four groups. Those are separate globals and are never written by
the setter, so a populated reset left all names, descriptions, currencies,
and prices allocated. The reset now names the original ARM targets without
changing the group gates, deletion order, or clearing order.

The opt-in `gof2_item_info_reset_smoke` compiles the actual reset function,
populates all 25 fields, and confirms every field is null afterward:

```powershell
cmake -S . -B <native-build-dir> -DGOF2_BUILD_ITEM_INFO_RESET_SMOKE=ON
cmake --build <native-build-dir> --target gof2_item_info_reset_smoke
<native-build-dir>/gof2_item_info_reset_smoke.exe
```

Result: `item info reset smoke: OK`. Focused ARM comparison remains
**100.0% source-shape** and **98.0% strict fuzzy**, with no exact match.
The native `gof2` build and 206/206 ARM translation units pass.
Verifier tests pass (36 run, one skipped). The full ARM corpus compares
**4,323/4,524** functions, with **91.91%** mean source-shape, **79.99%**
strict fuzzy, **2,319 linked-exact** and **997 byte-exact**. Against the
preceding premium-IAP report, no linked-exact or byte-exact pair was lost;
201 missing, zero wrong-type, and zero skipped remain. Full report:
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_item_reset_20261011.json`.

Remaining limits: The Java-side sequence of setter/reset calls has not been
observed, so repeated setter calls without reset and partial/null JNI arrays
are not validated. The original helper allocates copied strings via `malloc`
while reset calls `operator delete[]`; their allocator compatibility is
platform-dependent and remains to be checked. An apparent ARM use of item
name 3 for both name slots 3 and 4 was not introduced into the readable
source solely to improve matching.
