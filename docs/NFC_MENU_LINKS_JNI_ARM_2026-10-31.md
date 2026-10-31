# NFC menu-link JNI calls (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The exported 4-byte `NFC::interface_path` object is at `0x214004`.
Its `R_ARM_RELATIVE` initializer points to the string at `0x1c9e10`,
`net/fishlabs/gof2hdallandroid2012/NativeFunctionCalls`. The
recovered object was previously null; it now holds the same class
path and remains four bytes in the ARMv7 object.

The original `NFC::showMoreGames` (`0x69b28`), `rateGame`
(`0x69b7c`), `openPrivacyPolicy` (`0x69c24`) and
`openTermsOfService` (`0x69c78`) first check `g_pEnv`, find the
class through `interface_path`, obtain a static method with the
matching Java name and `()V` signature, then call it. The recovered
four methods now follow that path. `MenuTouchWindow` calls the
rating, privacy and terms methods. The original methods do not
perform the exception check used by the separate `DeviceInfo` calls;
this packet keeps that distinction.

Focused ARM source-shape / strict fuzzy for each of these four methods
improves **20.6% / 0.0% → 96.0% / 55.1%**. The `engine/core/NFC` unit
improves **48.5% / 30.2% → 57.1% / 37.2%** over 41 comparison rows,
up from 40 because the matching JNI void-call wrapper is now emitted.
Linked-exact rises **8 → 9**, byte-exact stays **2**, with no focused
exact losses. Windows and ARM64 static builds pass; ARMv7 compiles
**209/209** units.

The full ARM corpus compares **4,323/4,524** originals over **4,752**
pairs: **95.29% mean source-shape**, **84.99% mean strict fuzzy**,
**2,687 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Complete `(unit, symbol)` comparison against the preceding
revision shows one linked-exact gain (`_JNIEnv::CallStaticVoidMethod`),
no linked-exact losses, and no byte-exact changes. Reports are stored
outside Git under `D:\gof2-arm64-crypto`.

On non-Android builds these menu actions are no-ops. Live Java-method
availability and UI behavior remain unverified while device work is
deferred. Purchase and free-credit `NFC` wrappers still contain
placeholder JNI state and require separate recovery.
