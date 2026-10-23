# Fighter state-8 far-docking movement (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xde8a4–0xde930`. The state-8 distance
comparison at `0xde69e–0xde6a4` enters this path when the integer-converted
distance is greater than or equal to the ship-kind reach threshold.

The original converts signed `dt` to float, calls
`EaseInOutMatrix::Increase(dt)` on Fighter `+0x2b8`, and copies its
`GetValue()` matrix. It then calls the Fighter's virtual `getPosition()`
twice: the first result is the base position and the second is subtracted
from `objectPosition` (`+0x9c`). It multiplies that difference first by
float `dt`, then by the literal float `0.0006f` (`0x3a1d4952` at
`0xdec28`), and adds the result to the first position. Finally it calls
`MatrixSetTranslation`, `AEGeometry::setMatrix`, and assigns the same
matrix to `Player::transformMatrix` at Player `+0x4`, then returns via
`0xdf450`. No `PF_update_body` call belongs to this path.

The production-method fixture tests the exact threshold boundary,
`Increase` argument and call count, two additional virtual position
calls after the distance check, matrix translation, and both matrix
destinations at `-O0/-O2`. The near-arrival case verifies that easing and
matrix writes do not run after the state-9 transition. This fixture uses
mock easing, position and docking-point providers; it does not exercise
physical docking or a complete game frame. The original has no local
null check for `easeMatrix`. The current reconstruction initializes
that pointer to null and has no recovered state-8 setup path that creates
it, so the upstream ownership/precondition remains unverified.

Focused NDK r18b verification compiles all 208 ARM translation units and
compares 44 Fighter pairs. Linked-exact 21 and byte-exact 10 are retained.
Whole-`PlayerFighter::update` source-shape/strict fuzzy changes
**68.9%/27.9% → 70.6%/26.9%**; it remains nonexact. Native UCRT64 `gof2`,
all 11 Fighter smokes and 93 verifier tests pass (one existing skip).
The full `--no-build --fail-on-wrong-type` corpus compares **4,323/4,524**
original functions across **4,735** pairs, with zero skipped or wrong-type
symbols. Mean source-shape/strict fuzzy changes **94.04%/83.38% →
94.05%/83.38%**. All **2,557 linked-exact** and **1,082 byte-exact**
pairs from the near-docking baseline are retained. A pair-by-pair
`(unit, symbol)` comparison found no exact losses or gains and exactly
one changed score: `PlayerFighter::update` as above. The lower strict
fuzzy score for this nonexact function is recorded, not treated as
evidence against the directly observed ARM behavior. State 9, other
dispatch branches and complete game-frame docking remain unverified.
