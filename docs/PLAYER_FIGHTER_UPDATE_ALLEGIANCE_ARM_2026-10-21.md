# PlayerFighter update allegiance/timer block (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. This package reconstructs the
contiguous block at `0xdd116–0xdd1c6` after the previously recovered
prelude, plus its wanted-state side branch at `0xdd62a–0xdd63a`. It is not
a reconstruction of the full, approximately 3,556-instruction method.

When the Fighter flag at `+0x42` is set, ARM accumulates `dt` in the gun
switch timer at `+0x2e8`. Only when the sum exceeds 20,000 does it clear
the timer and, if slot 1 is available, toggle the word at `+0x140`. If
the associated Player is not already always-enemy, the level has a
PlayerEgo, the Ego's Radar currently locks this Fighter, and the Fighter
flag at `+0x43` is clear, ARM calls `Player::setAlwaysEnemy(true)` and
`Level::uncoverWanted(this->field_0x48)`. Static ARM layout assertions
identify `PlayerEgo::field_0x14` as the Radar pointer and
`Radar::lockedEnemy` at `+0x4`.

The next ARM sequence clears the two allegiance bytes for ship group 8
in an active black-market system with Status byte `+0x110` set. The
Player's `turnedEnemy()` flag then overrides the bytes with `0x0001`,
and `isAlwaysFriend()` takes final priority with `0x0100`. The C++
follows that order before the still-unresolved `PF_update_body` call.

The expanded production-method smoke tests the timer boundary,
available/unavailable gun, radar match and suppression flag, wanted
notification, and allegiance priority at `-O0` and `-O2`. Native UCRT64
`gof2` builds, all 208 NDK r18b ARM translation units compile, and 93
verifier tests pass (one existing skip). Focused whole-method
source-shape/strict fuzzy improves **5.3%/3.7% → 8.4%/6.0%**. The
44-function Fighter unit retains 21 linked-exact and 10 byte-exact
matches. The low whole-method score reflects the vast unreconstructed
remainder, not a standalone score for this block.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs, with **94.03%/83.37%** mean source-shape/strict
fuzzy, **2,557 linked-exact** and **1,082 byte-exact**. There are zero
skipped or wrong-type symbols. A `(unit, symbol)` comparison with the
preceding mission-crate report finds no exact-pair losses and only
`PlayerFighter::update` changed source-shape. Full report:
`fighter-update-allegiance-full-current.json` (SHA-256
`f5904d721c54c35e172c45d6a7b47484a794c25116e714b52f9567d004e861db`).

The smoke mocks external services; it does not demonstrate wanted-state
UI behavior or a complete Fighter frame in the game. The remaining ARM
body beginning with route, trail, matrix, movement, combat and mission
branches must be recovered before removing `PF_update_body` or claiming
end-to-end `update` behavior. The next bounded target is the route/trail/
matrix section at `0xdd1c6–0xdd240`.
