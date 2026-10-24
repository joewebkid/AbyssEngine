# Android Google Play SKU JNI recovery (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original APK DEX declares both `ToJNI` SKU methods as
`(I)Ljava/lang/String;` and passes indexes 0 through 4 from the
Google Play integration. ARM exports at `0x6b8dc` (premium) and
`0x6b940` (consumable) reject indexes outside 0..4, then branch by
index to `JNIEnv::NewStringUTF` with string literals in `.rodata`.

| Index | Premium ARM literal | Consumable ARM literal |
| --- | --- | --- |
| 0 | `iap_managed_valkyrie` | `iap_managed_credits_100_000` |
| 1 | `iap_managed_kaamo_club` | `iap_managed_credits_300_000` |
| 2 | `iap_managed_supernova` | `iap_managed_credits_1_000_000` |
| 3 | `iap_managed_vip` | `iap_managed_credits_3_000_000` |
| 4 | `iap_managed_full_package` | `iap_managed_10_000_000` |

The original literals occupy `0x1ca2da..0x1ca3cf`; the ARM PC-relative
loads at `0x6b8f0..0x6b93c` and `0x6b954..0x6b9a0` identify the
index-to-string mapping. The former recovered implementation returned
invented `net.fishlabs.gof2hd2012.dlcN` and `creditpackN` strings.
It now returns the confirmed literals through the original five-way
switch, retaining `nullptr` for all other indexes.

Focused ARM source-shape/strict fuzzy for each getter changes from
**48.1%/34.0%** to **100.0%/100.0%**. Neither getter is linked- or
byte-exact, so matching machine-code shape does not establish every
relocation or string address. `test_jni_sku_mapping.py` guards the
confirmed mapping and out-of-range branch. Native `gof2`, **209/209**
ARM units and **94** verifier tests (one skip) pass. The full corpus
compares **4,323/4,524** originals over **4,737** pairs at **94.60%**
mean source-shape and **84.11%** strict fuzzy, with **2,594
linked-exact** and **1,082 byte-exact**. Both exact symbol sets have
zero losses relative to the previous full report.

The original APK reaches its main menu with its OBB on a connected
device, but the recovered JNI getters have not been exercised by a
live purchase workflow. Google Play ownership, billing responses and
post-purchase content unlocks are outside this package.
