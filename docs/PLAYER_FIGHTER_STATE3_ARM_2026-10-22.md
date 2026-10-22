# Fighter state-3 update (Android ARM, 2026-10-22)

Primary evidence is Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xde1a8–0xde4aa`. The dispatch checks
`aiDisabled` before the state table: when set, it refreshes the geometry
reference matrix and calls `handleCloaking` without entering state 3.

State 3 enters at `0xde2c0`. It clears `finished` at `+0x101` and the byte
at `+0x13e`, forms a rotation from the drift Vector at `+0x188`, and applies
the composed geometry matrix only for `dt >= 1`. It translates by the
direction Vector at `+0x194` times `float(dt)` times `currentSpeed`, refreshes
the reference matrix, and subtracts `dt` from the timer at `+0x1f8`. The
frame ends while the timer is nonnegative.

At expiration, `Explosion::start` receives the frame's cached render position
and a zero Vector. The engine-trail emitter stops; the two damage emitters
also stop when the float setting at `Globals::options + 0x28` is positive.
The code sets bomb force to `50.0f + nextInt(50) * 0.01f`, then normalizes
three `nextInt(200) - 100` components into the previously untyped Vector at
`+0x17c`. It zeros the timer, enters state 4, and releases the `SpacePoint *`
at `+0x2c4` only if that point is occupied. If cargo is present, the
mission-crate case (`+0xd0` set, `+0x69` clear) clears the cargo flag and
sets `+0x68`; otherwise `createCrate` receives **4** for ship group 9 or
**0** for other groups. The type-4 argument is instruction-backed despite
the current `KIPlayer::createCrate` implementation treating it like its
default case. ARM32 offset assertions cover both newly typed slots.

The production-method smoke checks pre-expiry movement, `dt == 0`, timer
expiration, effect handles, RNG order, bomb force, occupied-point release,
ship-group-9 crate type, the disabled-AI path, and mission-crate loss with
the quality gate off. It runs at `-O0` and `-O2`. This is a controlled
fixture, not a full game-frame or rendered-effect test.

Focused ARM comparison of the whole `update` changes source-shape/strict
fuzzy from **53.3%/17.8%** to **58.1%/21.4%**. The Fighter unit retains
**21 linked-exact** and **10 byte-exact** pairs. Native UCRT64 `gof2`, all
**208** NDK r18b ARM translation units, all **11** Fighter smokes, and the
**93** verifier tests (one existing skip) pass. The full NDK r18b corpus
compares **4,323/4,524** original functions across **4,735** pairs at
**94.04%/83.38%** mean source-shape/strict fuzzy. It retains **2,557
linked-exact** and **1,082 byte-exact** pairs, with zero exact-pair losses,
skipped or wrong-type symbols and 124 extra symbols. Only the target
function's source-shape/fuzzy score changed relative to the preceding death
transition. Full report: `fighter-state3-final-full.json` (SHA-256
`e334dc8935b9aea9a31d2cb1b94f5c43bb19d09b45278a2cc48c265afee2c9cb2`).

Other state handlers and their common tail still use `PF_update_body` and
remain unverified. The next bounded target is the state-4 post-explosion
update, followed by the remaining shared dispatch paths.
