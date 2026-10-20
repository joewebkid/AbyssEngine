# PlayerFighter enemy selection and no-enemies route (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. This package extends the
wingman-route and Player audio handoff with the first branches after
`Player::getEnemies()`; it is not a recovery of the full update method.

At `0xdd274–0xdd2b6`, `aiDisabled` at Fighter `+0x2e4` bypasses both
enemy and route handling. When the enemy list exists, ARM unsigned-
compares `field_0x38` against its count and sets the index to -1 if it
is out of range. If `field_0x12e` is zero it also sets the index to -1.
Otherwise, for a nonnegative index it tests the selected Player's
`isActive()` and clears `field_0x12e` when that Player is inactive.
The subsequent AI/combat branches from `0xdd3c6` are still unresolved.

When `getEnemies()` returns null, ARM instead checks `routeClone`
(`0xdd35c–0xdd3be`). With a route it calls `Route::update` using the
Fighter geometry's current position. If `getWaypoint()` returns a
point, it reads each axis through a separate `getWaypoint()` call,
converts the integer coordinates to float, stores them in `resetVecB`
at `+0x164`, and sets `field_0x12c`. With no route it sets state 5
(`0xdd622–0xdd628`). The route/AI paths subsequently join at
`0xdda88`; that later section remains in `PF_update_body`.

The production-method smoke covers an out-of-range index, inactive
and unselected targets, the AI-disabled bypass, null/non-null route,
null/non-null waypoint, waypoint call count and coordinate transfer
at `-O0` and `-O2`. All 11 Fighter smokes pass. Native UCRT64 `gof2`
builds, all 208 NDK r18b ARM translation units compile, and 93
verifier tests pass (one pre-existing skip). Focused whole-method
source-shape/strict fuzzy improves **17.9%/10.1% → 20.2%/11.2%**.
The low whole-method score reflects the much larger unreconstructed
remainder.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs, at **94.03%/83.38%** mean source-shape/strict
fuzzy, with **2,557 linked-exact** and **1,082 byte-exact**. There are
zero skipped or wrong-type symbols. Comparing `(unit, symbol)` with
the preceding wingman-route report finds no exact-pair losses; only
`PlayerFighter::update` changes source-shape. Full report:
`fighter-update-enemies-route-full-current.json` (SHA-256
`ad21d6626065c9398ef82dfccfc3c531ca2585326bf54e6f7ecbea69ee828d3a`).

Target-search, combat, route arrival and movement logic, plus the
common update tail, remain unverified in a complete game frame.
