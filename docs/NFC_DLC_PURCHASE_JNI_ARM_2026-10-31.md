# NFC DLC purchase requests (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The five purchase wrappers at `0x6958c` (`valkyrie`), `0x69624`
(`kaamo_club`), `0x69684` (`supernova`), `0x696e4` (`vip`) and
`0x69744` (`full_package`) first check `g_pEnv`. Each then sets its
own 4-byte `gi_iap_buy_dlcN_pressed` global to 1 before finding
`NFC::interface_path` and calling a static Java method with the same
name as the C++ method and signature `()V`. The five globals occupy
`0x2179c8–0x2179d8` in the original ELF. The recovered Android
bridge already defines and later clears these same globals in
`ndk_iapBoughtPremium`.

The previous `NFC` source dereferenced an uninitialized local
`nfc_env` pointer, wrote through one null purchase-flag pointer,
and passed empty Java method names. The five wrappers now use the
actual `g_pEnv` and their matching flag and Java method. The class
path was recovered in the preceding menu-link packet. The operations
remain no-ops on non-Android builds; platform purchase behavior and
Java class availability are unverified while device work is deferred.

Focused ARM source-shape / strict fuzzy improves **20.5% / 0.0% →
96.1% / 49.4%** for each of the five wrappers. The 41-row
`engine/core/NFC` unit improves **57.1% / 37.2% → 66.3% / 43.3%**;
linked-exact remains **9**, byte-exact **2**, and no focused exact
symbol is lost. Windows and ARM64 static builds pass; ARMv7 compiles
**209/209** units.

The full ARM corpus compares **4,323/4,524** originals over **4,752**
pairs: **95.37% mean source-shape**, **85.04% mean strict fuzzy**,
**2,687 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Complete `(unit, symbol)` exact-set comparison against the
preceding revision finds no linked-exact or byte-exact gains or losses.
Reports are stored outside Git under `D:\gof2-arm64-crypto`.

Credit-pack, restore-purchases, and free-credit `NFC` wrappers still
need separate source-backed recovery.
