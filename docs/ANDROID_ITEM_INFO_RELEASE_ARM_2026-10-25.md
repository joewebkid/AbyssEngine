# Android store item-info UTF release (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `ndk_setNativeItemInformationList` at `0x94858` obtains
five elements from each of five Java arrays, obtains 25 UTF strings,
copies them into the `Globals::cItemList*` fields, then passes every
element and UTF pointer to `releaseStringUTFChars` (calls at
`0x94cf4..0x94dc0`). The helper at `0x94828` conditionally calls JNI
`ReleaseStringUTFChars` only when its inputs are present, with the
original fallback cleanup otherwise. It is independently
**100%/100% source-shape/strict fuzzy and linked-exact**.

The recovered setter previously bypassed this helper and called JNI
`ReleaseStringUTFChars` directly in its cleanup loop. It now uses the
recovered original helper for all 25 slots. The compiled ARM object
contains a relocation to `releaseStringUTFChars` in the setter. This
restores the original cleanup path when JNI returns an incomplete
element or UTF result; it does not add a new validation policy for
null input arrays or prove that Android JNI accepts them.

Focused setter source-shape/strict fuzzy changes from **9.6%/4.7%**
to **8.9%/4.2%**. The original unrolls all 25 operations, while the
recovered setter uses loops, so this small semantic correction does
not improve compiler shape. The original also copies item name index
3 into both name slots 3 and 4; the recovered setter keeps the
distinct index-4 name to avoid a visible data regression. This is a
documented source-behavior deviation, not an exact-match claim.

Native `gof2`, **209/209** ARM units and **95** verifier tests (one
skip) pass. The full corpus compares **4,323/4,524** original
functions over **4,737** pairs, at **94.60%** mean source-shape and
**84.11%** strict fuzzy, with **2,594 linked-exact** and **1,082
byte-exact**. Neither exact symbol set loses an entry against the
previous full report. The original app running on a phone is not a
runtime test of this recovered setter. A recovered Android
billing/store flow and the Java-side setter/reset sequence remain
unverified.
