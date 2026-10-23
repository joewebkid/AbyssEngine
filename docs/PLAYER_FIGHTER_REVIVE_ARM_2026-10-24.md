# Fighter revive recovery (2026-10-24)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::revive()` at `0xdff5c–0xe005c`. The local recovered IDA
`PlayerFighter.c` lines 3451–3521 independently cross-check the call order;
it is not original C++ source.

ARM calls `Player::turnedEnemy()`, resets the Player, then reapplies enemy
status when needed. It constructs an empty `String`, assigns it to Fighter
`name` at `+0x18`, and destroys the temporary. The previous source read
`turnedEnemyFlag` directly and omitted the name reset. ARM also calls
`KIPlayer::setActive(true)` after clearing the death timer and before
resetting `+0xf0`, speed, explosion and push state. That call was missing
despite `Player::reset()` also setting its own active flag. The rest of
the state reset and the ship-group 9/10 versus generated-loot branches
remain in their source-backed order.

The new production-method smoke runs at `-O0/-O2` with recording
dependencies. It checks the call order, enemy preservation across Player
reset, name clearing, activation, state fields, route and explosion reset,
geometry selection, and both loot branches. All 12 Fighter smokes, native
UCRT64 `gof2`, 208 NDK r18b ARM translation units and 93 verifier tests
pass (one existing skip). Focused comparison of 44 Fighter pairs changes
`revive` source-shape/strict fuzzy **77.0%/66.3% → 99.6%/93.0%**; the
function remains nonexact. All **21 linked-exact** and **10 byte-exact**
focused pairs are retained; only `revive` changes score.

The full ARM corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **201** still absent, zero skipped and zero
wrong-type symbols. Rounded mean source-shape/strict fuzzy changes
**94.05%/83.38% → 94.06%/83.39%**. Pairwise comparison with the
preceding full report found no missing pair, linked-exact or byte-exact
loss, and no score change outside `PlayerFighter::revive`. All
**2,557 linked-exact** and **1,082 byte-exact** pairs are retained.

The fixture uses a controlled loot generator, so it does not validate
real loot contents or a complete post-revival game frame. Those runtime
outcomes and the residual compiler-level mismatch remain unverified.
