# Status kill counters notify Achievements (Android ARM, 2026-10-20)

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Status::incKills` at `0xac6c4` and `Status::incPirateKills` at `0xac6ec`.
Each increments its own field (`+0x1c0` or `+0x1d8`), loads the global pointer
through GOT slot `0x210214`, and tail-calls a Thumb/ARM veneer. The GOT
relocation identifies **`Globals::achievements`**; the veneers at `0x19bf68`
and `0x19bf78` land on the PLT entries for `Achievements::incKills` at
`0x63990` and `Achievements::incPirateKills` at `0x6399c`. These references
rule out both the previous no-op hooks and the distinct
`Achievements::gAchievements` holder as the source expression.

`Status.cpp` now invokes those existing `Achievements` methods after updating
its own counters. The original code has no null guard on the global pointer,
so none was added. `Globals::init` constructs and stores the achievements
object before the `Status` object. The only source caller of `incKills` is
`Level::enemyDied`. The original has the `incKills` call at `0xc426a` inside
that function. Its `incPirateKills` call is at `0xddfde` inside
`PlayerFighter::update`, after bounty handling. The recovered update still
delegates later branches to the unresolved `PF_update_body`, so there is no
source caller of `incPirateKills` yet. Restoring that caller is the next
behavioral priority; this package fixes the callee, not the whole kill path.

The focused NDK r18b result for each function changed from **47.4% source-shape
/ 0% strict fuzzy, nonexact** to **100% / 100%, linked-exact** (not byte-exact).
The changed ARM translation unit compiles, the UCRT64 `gof2` static library
builds, and the verifier suite passes 93 tests with one existing skip.
An end-to-end kill event in a running game is still untested; this recovery
establishes the counter propagation and ARM call shape, not the complete
medal-award path.

The final full NDK r18b report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/status-kills-full-current.json`
(SHA-256 `c7225d0d9aafdf3a5d6b26bc5d6c943f9528c47d83fbadf93c4df2e21dfd591b`).
It compares **4,323/4,524** original functions across **4,735** pairs:
**94.01%** mean source-shape, **83.35%** strict fuzzy, **2,557 linked-exact**
and **1,082 byte-exact**. There are zero skipped and wrong-type symbols.
Against the preceding `agent-mission-full-current.json`, exact-pair identity
comparison finds zero losses and exactly these two linked-exact gains; no
other function's source-shape changed. The 201 missing names remain bundled
library and runtime functions, not game-class methods.
