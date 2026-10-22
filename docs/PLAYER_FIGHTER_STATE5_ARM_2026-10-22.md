# Fighter state-5 visibility and reactivation (2026-10-22)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xde1c4` (state dispatch) and
`0xde4c4–0xde58e` (state 5). This package restores the complete state-5
branch after the existing update prelude. It does not reconstruct other
states or the shared movement tail.

The original first checks the Player byte at `+0x5c` (`enemyFlagsLo`). If
set and `Status::getCurrentCampaignMission() >= 2`, it hides the Fighter's
subgeometry when present, otherwise its main geometry. It then returns if
the Fighter's target Player pointer at `+0x148` is null or that Player's
`+0x5e` byte is set. No meaning beyond the byte check is claimed for the
latter field.

For a target without that flag, each component of Fighter `resetVecC` at
`+0x170` must lie strictly between the positive and negative `+0x128`
integer radius converted to float. On success, the branch writes state 1,
shows the same geometry, activates the associated Player and, if Fighter
`+0x12f` is nonzero, calls `Level::pirateStationAction(true)`. Equality at
either distance boundary and unordered float comparisons do not activate.
The earlier pre-dispatch proximity override may already change state 5 to
state 1; the production-method fixture isolates both routes.

The fixture checks mission threshold 1/2, visibility through main and
subgeometry, null/flagged target exits, the strict distance boundary, and
activation plus pirate-station notification. It runs the real production
method at `-O0/-O2` using small dependency mocks. The fixture is not a
complete game-frame or physical-device validation. Other dispatch states,
the common movement tail, and semantics of Player `+0x5e` are unverified.

Validation: all 11 Fighter production-method smokes pass; the state-5
fixture runs at `-O0/-O2`. Native UCRT64 `gof2`, all 208 NDK r18b ARM
translation units, and 93 verifier tests pass (one existing skip). Focused
ARM verification compares 44 Fighter pairs. Whole-`PlayerFighter::update`
source-shape rises **63.9% -> 66.1%**; strict fuzzy is **25.1% -> 25.0%**,
so the target remains nonexact. This change was retained for confirmed
behavior, not fuzzy optimization.

Full ARM corpus: **4,323/4,524** original functions compared across
**4,735** pairs; mean source-shape/strict fuzzy **94.04%/83.38%**,
**2,557 linked-exact** and **1,082 byte-exact**. There are 201 missing
originals, 124 extra build symbols, zero skipped/wrong-type pairs, and zero
exact-pair losses. Pairwise comparison with the state-4 checkpoint found
only `PlayerFighter::update` changed score. Generated local report:
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261022/fighter-state5-full.json`,
SHA-256 `65ad47e8ef9acb13a27f9156255d8500075669c2ee14792e90039cead44b284c`.
The report and original binary are not committed.
