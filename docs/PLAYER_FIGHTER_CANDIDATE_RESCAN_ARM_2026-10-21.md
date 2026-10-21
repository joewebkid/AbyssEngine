# PlayerFighter candidate rescan (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. ARM instructions at
`0xdd7c2–0xdd8da` were cross-checked against REA/IDA pseudocode.
This is the next continuous target-selection block, not a recovery
of the complete update method.

A command-3 wingman skips the rescan. Otherwise, an index below 1
also skips it; for an index of at least 1, ARM first resets it to -1
and scans candidates from index 1. It skips inactive and dead Players.
`Player::getKIPlayer()` supplies a group when present and the raw
`Player::field_69` byte is zero; a nonzero byte instead gives group
zero. A null KIPlayer with a zero byte disqualifies the candidate.
The meaning of `field_69` beyond this branch is not established.

For a wingman, a candidate qualifies when its `enemyFlagsLo` is
nonzero or it is the current wingman target's Player. Otherwise the
original compares the candidate and Fighter groups: 8, 9 and 10
qualify when exactly one side has that group; 0↔1 and 2↔3 are
additional directed pairs. The first qualifying candidate sets
`field_0x12e` and becomes `field_0x38`. At the join, ARM clears
`field_0x12c`; a wingman with command 1 and no selection also sets
the index to -1 before entering the still-unrecovered route/combat
branch.

The production-method smoke exercises every listed group pair,
same-group rejections, a null KIPlayer with both `field_69` states,
dead-candidate rejection, and wingman enemy-flag/target matching at
`-O0` and `-O2`. All 11 Fighter smokes pass. Native UCRT64 `gof2`
builds with its local archive output on C:; all 208 NDK r18b ARM
translation units compile and 93 verifier tests pass (one existing
skip). Focused whole-`update` source-shape/strict fuzzy improves
**29.8%/14.8% → 33.0%/15.8%**. The 44-function Fighter unit retains
**21 linked-exact** and **10 byte-exact** matches without pair losses.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact**
matches, with zero skipped, wrong-type, or exact-pair losses and 124
extra symbols. Comparing `(unit, symbol)` with the preceding wingman-
validation report finds only this `update` source-shape change. Full
report: `fighter-update-candidate-rescan-full.json` (SHA-256
`7fcac0c76f50a342cbae74fa75e24af144fbd92315abd714e5a0322aa3ae44a7`).

The next route/combat dispatch at `0xdd8e2–0xdda88`, later movement,
and the common tail remain in unresolved `PF_update_body`. A complete
game frame has not been validated.
