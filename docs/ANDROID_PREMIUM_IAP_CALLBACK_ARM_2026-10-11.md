# Android premium-DLC callback (2026-10-11)

Android 2.0.16 `libgof2hdaa.so` has a separate Java/JNI purchase callback:
`Java_net_fishlabs_googleplay_ToJNI_iapBoughtPremium` forwards to
`ndk_iapBoughtPremium` at `0x00094410`. This route does not fill the
`GameData+0x54/+0x58/+0x60` pointers read by `ModStation`; those producers
remain unidentified in the recovered source.

The ARM callback rejects a missing engine, pack outside 0..4, or `bought`
outside 0..1 (`0x94410..0x94426`). It clears the matching pending flag and
stores `bought == 1` in the DLC option byte, with these additional effects:

| Pack | ARM branch | Option bytes | Other effect |
| --- | --- | --- | --- |
| 0 | `0x9443a` | `+0x35` | `Status::setSystemVisibility(25, true)` |
| 1 | `0x9445a` | `+0x36` | `Status+0x114 = 3` |
| 2 | `0x94482` | `+0x37` | `Status::setSystemVisibility(25, true)` |
| 3 | `0x9449e` | `+0x38` | none |
| 4 | `0x944b8` | `+0x35`, `+0x39`, `+0x37` | `Status::setSystemVisibility(25, true)` |

The old recovered callback changed only one option byte, accepted invalid
`bought` values, and omitted the visibility/campaign side effects. The five
branches now follow the ARM behavior. An opt-in native smoke compiles the
actual callback in isolation and checks all five packs with `bought=0/1`,
pending flags, invalid arguments, and the missing-engine gate:

```powershell
cmake -S . -B <native-build-dir> -DGOF2_BUILD_PREMIUM_IAP_SMOKE=ON
cmake --build <native-build-dir> --target gof2_premium_iap_smoke
<native-build-dir>/gof2_premium_iap_smoke.exe
```

Result: `premium IAP smoke: OK`. Native `gof2` and 206/206 ARM translation
units build. Focused `ndk_iapBoughtPremium` rises from **45.7%** to **98.6%**
source-shape and from **20.1%** to **92.0%** strict fuzzy; it is not exact.
The remaining instruction differences are the equivalent order of the engine
gate and two option stores, not missing effects. The JNI/Google Play lifecycle
has not been executed end to end.

The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_premium_iap_20261011.json`:
**4,323/4,524** original functions and 4,689 rows compared, **91.91%**
mean source-shape, **79.99%** strict fuzzy, **2,319 linked-exact** and
**997 byte-exact**. Against the preceding purchase-result report, there are
no lost comparison rows or exact pairs; 201 missing, zero wrong-type and zero
skipped remain.

Next inspect the native item-information producer/reset path and its string
ownership. The `GameData` purchase-list owner remains a separate open question.
