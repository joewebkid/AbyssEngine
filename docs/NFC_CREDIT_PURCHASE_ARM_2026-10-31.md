# NFC credit-pack purchase requests and callback (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The five request wrappers start at `0x697f8`, `0x69858`, `0x698b8`,
`0x69918`, and `0x69978`, for 100,000 through 10,000,000 credits.
Each checks `g_pEnv`, sets its corresponding
`gi_iap_buy_credit_pack1..5_pressed` global, finds
`NFC::interface_path`, and invokes the matching static Java method
with signature `()V`. The previous source dereferenced null local
JNI and flag pointers and passed empty method names. The recovered
five request wrappers now follow the ARM-observed path.

The original `setBaughtCredits` callback at `0x9462c` maps 100,000
credits to `pack1` and 10,000,000 to `pack5`. ARM assembly at
`0x94660–0x94676` confirms these exact global targets. The recovered
callback had those two flags reversed. The two targets are corrected,
so each request and its callback refer to the same flag. The middle
three pack mappings were already correct. `ndk_iapBoughtConsumable`
calls this callback with the five original credit amounts.

Focused ARM source-shape / strict fuzzy for each request improves
**20.5% / 0.0% → 96.1% / 49.4%**. The 41-row `engine/core/NFC`
unit improves **66.3% / 43.3% → 75.5% / 49.3%**, retaining nine
linked-exact and two byte-exact rows. The `setBaughtCredits`
instruction form remains **100% / 100%**; the correction changes
the referenced global targets. The 19-row `platform/android/NdkHooks`
focused comparison retains 11 linked-exact rows and no exact loss.
Windows and ARM64 static builds pass; ARMv7 compiles **209/209** units.

The full ARM corpus compares **4,323/4,524** originals over **4,752**
pairs: **95.45% mean source-shape**, **85.09% mean strict fuzzy**,
**2,687 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Complete `(unit, symbol)` exact-set comparison against the
preceding revision shows no linked-exact or byte-exact gains or losses.
Reports are stored outside Git under `D:\gof2-arm64-crypto`.

On non-Android builds the request wrappers remain no-ops. Real store
requests, callback timing, and Java method availability are unverified
while device work is deferred. Restore-purchases and free-credit
wrappers remain separate recovery work.
