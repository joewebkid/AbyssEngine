# PlayerFighter update: wingman target and Player audio handoff (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. This is a bounded extension
of the existing prelude, not a recovery of the complete update method.

At `0xdd22c–0xdd23e`, ARM enters the wingman-route branch only when
`wingmanFlag` is nonzero, state is not 4, and `routeClone` exists. That
branch copies `Level::getPlayer()->geometry->getPosition()` to the
Fighter's `targetPos` at `+0x90` (`0xdd2b8–0xdd2d2`). If `field_0xe4`
is not 1, execution rejoins at `0xdd240`; `field_0xe4 == 1` enters
the still-unrecovered wingman-command maneuvers. The recovered C++
does not run the common audio path prematurely in that case.

The common path stops the associated Player's engine sound in states
3 and 4, then jumps to a later tail. In other states it calls
`Player::update(dt, state != 5 && fov != -1)` followed by
`Player::getEnemies()` (`0xdd240–0xdd27e`). The returned enemy list
feeds subsequent AI/combat branches, which remain in the unresolved
`PF_update_body` region. The implementation currently records the
known call and does not claim to reproduce that later use.

The production-method smoke covers the death-state sound stop, normal
audio update, state-5 and `fov == -1` sound suppression, wingman target
copy and the unrecovered command-1 gate at both `-O0` and `-O2`. All
11 Fighter smokes pass. Native UCRT64 `gof2` builds, all 208 NDK r18b
ARM translation units compile, and 93 verifier tests pass (one
pre-existing skip). Focused whole-method source-shape/strict fuzzy
improves **10.2%/7.5% → 12.1%/8.4%**. The low whole-method score
reflects the much larger unreconstructed remainder.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs, at **94.03%/83.37%** mean source-shape/strict
fuzzy, with **2,557 linked-exact** and **1,082 byte-exact**. There are
zero skipped or wrong-type symbols. Comparing `(unit, symbol)` with
the preceding route/trail report finds no exact-pair losses; only
`PlayerFighter::update` changes source-shape. Full report:
`fighter-update-audio-full-current.json` (SHA-256
`35e9209b7e85dea8fa06a53e4bd1f7bd013d07578cd2347849ac00e7d21369f8`).

The remaining work starts with the wingman-command branches around
`0xdd2dc–0xdd6ca`, then the enemy-list and route handling from
`0xdd27e` onward. `PF_update_body` remains unresolved; a full game
frame and audio behavior have not been validated.
