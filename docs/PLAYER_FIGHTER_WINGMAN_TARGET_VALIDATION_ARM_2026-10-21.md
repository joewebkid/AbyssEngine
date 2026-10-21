# PlayerFighter wingman target and validation (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. ARM disassembly at
`0xdd50e–0xdd542`, `0xdd74e–0xdd7c0` was cross-checked against REA/IDA
pseudocode. This extends the target-search prefix, not the complete update.

Only after the timer-triggered random search, a wingman with command 3
and a non-null `wingmanTarget` checks that target. A dead target receives
`setWingmanCommand(1, nullptr)` through the virtual slot. Otherwise the
enemy list is scanned from index zero: a matching target Player replaces
the selected index only if active and not `isAlwaysFriend()`. Whether or
not that scan finds a replacement, ARM rechecks the indexed Player's
`isActive()` and `isDead()`. An inactive or dead target clears
`field_0x12e` and sets the index to -1. An active, live target is copied
to `targetPos`; if any coordinate difference lies outside the strict
`(-field_0x128, +field_0x128)` cube, the index becomes -1 without
clearing `field_0x12e`. The ordinary ordered scan bypasses this second
validation. At the shared join, a zero `Player::enemyFlagsLo` and zero
index clear `field_0x12e` and change the index to 1.

The production-method smoke exercises the wingman match, always-friend
exclusion, dead wingman command reset, inactive/dead selection, distant
live selection, and the zero-index fallback at `-O0` and `-O2`. All 11
Fighter smokes pass. Native UCRT64 `gof2` builds with its local archive
output on C: because A: is nearly full; all 208 NDK r18b ARM units
compile and 93 verifier tests pass (one existing skip). Focused
whole-`update` source-shape/strict fuzzy improves
**26.4%/12.9% → 29.8%/14.8%**. The 44-function Fighter unit retains
**21 linked-exact** and **10 byte-exact** matches without pair losses.

The final NDK r18b full corpus compares **4,323/4,524** original
functions across **4,735** pairs at **94.04%/83.38%** mean
source-shape/strict fuzzy. It retains **2,557 linked-exact** and
**1,082 byte-exact** matches, with zero skipped, wrong-type, or
exact-pair losses and 124 extra symbols. Comparing `(unit, symbol)`
to the preceding target-search report finds only this `update` source-
shape change. Full report: `fighter-update-wingman-target-full.json`
(SHA-256 `8cab4f252c5a1c1dd2e55be9fa599e19338cf0caeaa1f763548cfa822b54e093`).

The follow-on candidate rescan at `0xdd7c2–0xdd8da`, route/combat,
movement and the common tail remain in unresolved `PF_update_body`.
The recording smoke does not exercise a complete game frame or the
production `setWingmanCommand` implementation through this path.
