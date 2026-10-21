# PlayerFighter route and enemy handoff (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. The dispatch and target-position
instructions at `0xdd8d2–0xdda88` and waypoint coordinates at
`0xdded8–0xddf0e` were checked against REA/IDA pseudocode. This is one
continuous handoff, not the complete method.

An index of -1 with a route enters the route path directly. Other indices
consult `Player::doesNeverAttack()` before choosing the route or enemy path.
On the route path, a missing waypoint or inactive body resets the index to
zero and selects enemy zero when available. An active route updates from the
geometry position. A waypoint without a docking target copies its integer
coordinates into `resetVecB` and sets `field_0x12c`. With a docking target,
state 1 changes to 7 and `field_0x12c` is set, but the subsequent navigation
point and docking movement remain unrecovered. On the direct-enemy path, an
invalid index falls back to zero; the selected enemy's position updates
`targetPos` and `resetVecB`. The ARM word at Fighter offset `0x148` holds a
`Player*`, so the source field is now a pointer rather than an integer; this
also avoids truncating the pointer in the native 64-bit build.

The production-method smoke covers these branches at `-O0` and `-O2`.
All 11 Fighter smokes pass, as do native UCRT64 `gof2`, all 208 NDK r18b
ARM translation units, and 93 verifier tests (one existing skip). Focused
whole-`update` source-shape/strict fuzzy improves **33.0%/15.8% →
36.7%/18.7%**. The 44-function Fighter unit retains **21 linked-exact**
and **10 byte-exact** matches without pair losses.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact** matches,
with zero skipped, wrong-type, or exact-pair losses and 124 extra symbols.
Comparing `(unit, symbol)` pairs with the preceding candidate-rescan
report finds only the expected `update` score change. Full report:
`fighter-update-route-dispatch-full.json` (SHA-256
`c389d59cbfe046a46fea42520edbf8217012d1ae06442140ad7e1d6d33a0abe4`).

The navigation/docking subpath, the subsequent movement, and the common
tail remain in unresolved `PF_update_body`. `doesNeverAttack()` implies
a valid route in this original control-flow path; no independent null-route
behavior has been established. A complete game frame has not been validated.
