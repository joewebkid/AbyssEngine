# NFC restore and free-credit actions (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`NFC::iap_restore_purchases` starts at `0x697a4`; the five
`free_credits_*` actions start at `0x699d8`, `0x69a2c`, `0x69a80`,
`0x69ad4`, and `0x69bd0`. Each original function checks `g_pEnv`,
finds `NFC::interface_path`, obtains a static method whose name
matches the exported C++ method and whose JNI signature is `()V`,
then calls it. None of these six ARM functions writes a local
purchase or reward flag. The previous source instead dereferenced
an uninitialized `nfc_env` pointer and used empty method strings.
All six names also occur as strings in the original `classes.dex`,
SHA-256 `b9786340ee2dcdb8d1a351c5f1d60c66d9c48423996077f33ec6d1ad41a096a1`;
string presence alone does not establish class membership or runtime
availability.

All six recovered methods now use the observed JNI path. The obsolete
file-local null JNI pointers and empty strings, plus their unused
manual-vtable helper declarations in `NFC.h`, were removed after a
source-wide reference check. They emitted no callable runtime
definitions. `HangarWindow` reaches the free-credit actions; the
Android bridge handles offerwall credits separately in
`ndk_checkPlaytimeAndSpendOfferwallCredits`.

Focused ARM source-shape / strict fuzzy for each of the six methods
improves **20.6% / 0.0% → 96.0% / 55.1%**. The 41-row
`engine/core/NFC` unit improves **75.5% / 49.3% → 86.6% / 57.3%**;
linked-exact stays **9**, byte-exact **2**, with no focused exact
losses. Windows and ARM64 static builds pass; ARMv7 compiles
**209/209** units.

The full ARM corpus compares **4,323/4,524** originals over **4,752**
pairs: **95.55% mean source-shape**, **85.16% mean strict fuzzy**,
**2,687 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Complete `(unit, symbol)` exact-set comparison against the
preceding revision finds no linked-exact or byte-exact gains or losses.
Reports are stored outside Git under `D:\gof2-arm64-crypto`.

Non-Android builds leave these actions as no-ops. Java method
availability, external store behavior, reward callbacks, and the
live UI path remain unverified while device work is deferred.
