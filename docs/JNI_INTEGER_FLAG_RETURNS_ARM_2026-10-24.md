# Android JNI integer flag and DLC returns (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The JNI exports for `getExitFlag` (`0x6b850`), `getScreenshotFlag`
(`0x6b854`), `getLogoShown` (`0x6b9d0`) and `isInMainMenu`
(`0x6b9d4`) are single tail branches to their respective native
functions. The latter two native functions at `0x947d0` and `0x947e0`
load a full 32-bit flag. The five `getDLC1BOUGHT` through
`getDLC5BOUGHT` JNI exports at `0x6b888..0x6b898` likewise tail-call
their native functions at `0x94524..0x945b4`. Those functions load
the raw unsigned option bytes at offsets `0x35..0x39`, returning zero
when `gEngine` is null. None of these nine JNI exports normalizes the
result to zero or one.

The installed original APK (SHA-256
`515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`)
DEX declares all nine
`ToJNI` methods as `()I` (integer return), including the five DLC
getters. This independently confirms the native return type and makes
the earlier recovered `jboolean` signatures a real JNI contract error.

Recovered `JniBridge.cpp` had `jboolean` wrappers for all nine. Four
also compared their integer results with zero; the five DLC helpers
themselves converted the option bytes to `bool`. All nine JNI wrappers
now return `jint`, and the DLC helpers return the original raw byte as
`int`. The two flag sources defined in the same translation unit
retain a no-inline boundary so the exit-flag call is not folded into
its JNI wrapper. No Java or phone state was changed.

Focused ARM source-shape/strict fuzzy before and after:

| JNI symbol | Before | After | Exact after |
| --- | --- | --- | --- |
| `getExitFlag` | 22.2% / 0% | 100% / 100% | nonexact |
| `getScreenshotFlag` | 30.0% / 0% | 30.0% / 0% | nonexact |
| `getLogoShown` | 2.9% / 0% | 100% / 100% | linked-exact |
| `isInMainMenu` | 2.9% / 0% | 100% / 100% | linked-exact |
| Each of five DLC JNI getters | 5.0% / 0% | 100% / 100% | linked-exact |
| Each of five native DLC getters | 84.3% / 56.2% | 100% / 100% | linked-exact |

The original `getScreenshotFlag` wrapper branches to a native
function that always returns zero. Current Clang 7 folds the call to
`movs r0, #0; bx lr` even when the native function is no-inline. That
wrapper remains behaviorally equivalent but nonexact; forcing another
source shape solely for the metric was not warranted. Both
`ndk23_getExitFlag` and `ndk23_getScreenshotFlag` retain their prior
linked-exact shape, the latter byte-exact.

The ABI evidence is static. A recovered on-device JNI execution gate
and observation of non-boolean flag or DLC option values remain
unverified.

Native `gof2`, **208/208** ARM translation units and **93** verifier
tests (one skip) pass. The full corpus compares **4,323/4,524**
original functions over **4,737** pairs at **94.56%** mean
source-shape and **84.05%** strict fuzzy, with **2,592 linked-exact**
and **1,082 byte-exact**. Relative to the preceding report, the five
DLC JNI wrappers, five native DLC getters, and two menu/logo wrappers
enter the linked-exact set; no prior linked- or byte-exact pair is
lost.
