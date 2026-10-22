# Fighter state-6 acceleration and death (2026-10-22)

Primary evidence is Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)`. Its state table at `0xde1c4` sends state 6
to `0xde590–0xde5d8`, then returns through `0xde1dc`.

The original loads `currentSpeed` at Fighter `+0x1e8`, multiplies it by
the float literal `1.1f` at `0xde700`, stores the new speed, multiplies
it by float-converted `dt`, converts the result to integer and back to
float, and passes that value to `AEGeometry::moveForward`. It then calls
`KIPlayer::setDead()` only if the new speed is strictly above the `100.0f`
literal at `0xde2a8`. The source retains the integer truncation before
movement rather than treating the product as a continuous distance.

The production-method fixture exercises fractional-distance truncation,
speed below the death threshold, and a crossing above it. It checks that
`setDead` deactivates the Player via the existing KIPlayer contract.
The fixture runs at `-O0/-O2` with dependency mocks; a complete game frame
and physical-device execution are not verified. States 0, 1, 7–9 and the
shared movement tail remain unrecovered.

Validation: all 11 Fighter production-method smokes pass, including this
fixture at `-O0/-O2`. Native UCRT64 `gof2`, all 208 NDK r18b ARM
translation units, and 93 verifier tests pass (one existing skip).
Focused ARM verification compares 44 Fighter pairs. Whole-
`PlayerFighter::update` source-shape/strict fuzzy changes
**66.1%/25.0% -> 66.4%/26.4%**; it is still nonexact.

The full ARM corpus compares **4,323/4,524** original functions across
**4,735** pairs, with mean source-shape/strict fuzzy **94.04%/83.38%**,
**2,557 linked-exact** and **1,082 byte-exact**. There are 201 missing
originals, 124 extra build symbols, zero skipped/wrong-type pairs, and
zero exact-pair losses. Pairwise comparison with the state-5 checkpoint
found only `PlayerFighter::update` changed score. Generated local report:
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261022/fighter-state6-full.json`,
SHA-256 `adff06038dbfbf2cc2a6e867e8fe77d4bd442d036578c79c8e2e6001f661c8c9`.
The report and original binary are not committed.
