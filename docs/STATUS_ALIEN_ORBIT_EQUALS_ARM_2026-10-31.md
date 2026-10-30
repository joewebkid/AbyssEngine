# Status alien-orbit station comparison (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::inAlienOrbit` at `0xa963c–0xa9642` loads the station pointers
at offsets `0x19c` and `0x78`, then tail-calls `Station::equals`.
`Station::equals` at `0xa70ec–0xa70fe` returns false for a null argument
and otherwise compares station indices at offset `0x0c`. The previous
source instead compared pointer addresses with `!=`. That gave the
opposite answer for the same station object and could disagree for
distinct objects with matching indices. The recovered source now calls
`station->equals(playerStation)`, retaining the original index-based
comparison and call path.

Focused `Status::inAlienOrbit` improves from **47.8% / 44.4%** to
**100% / 100%** source-shape / strict fuzzy and becomes linked-exact.
The 146-symbol `game/mission/Status` unit improves from **94.5% /
85.5%** to **96.6% / 88.0%**. Linked-exact symbols rise **96 → 103**;
byte-exact stays **66**. The seven linked-exact gains are
`inAlienOrbit`, `inFogSkyboxOrbit`, `inSupernovaOrbit`,
`inPirateLootOrbit`, `inPlanetRingOrbit`, `inDeepScienceOrbit`, and
`inBlackMarketSystem`. The focused unit has no exact losses.
Windows and ARM64 static builds pass; ARMv7 compiles **209/209** units.

The full ARM corpus compares **4,323/4,524** originals over **4,749**
pairs: **95.18% mean source-shape**, **84.90% mean strict fuzzy**,
**2,684 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Complete `(unit, symbol)` exact-set comparison against the
preceding revision finds precisely the seven linked-exact gains above,
no linked-exact losses, and no byte-exact changes. Reports are stored
outside Git under `D:\gof2-arm64-crypto`.

The original assumes the current `station` receiver is valid; this
packet does not add a new null policy. Live orbit behavior remains
unverified while device work is deferred.
