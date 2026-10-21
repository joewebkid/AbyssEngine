# Fighter death and freelance-mission flag (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`PlayerFighter::update(int)` at `0xdcfd8` was checked at
`0xddd94–0xddf18` against ARM disassembly. REA opened the same binary
and confirmed its hash, but its IDA function-analysis provider returned
malformed output for this query; no decompiler interpretation is claimed
for this package.

The branch is entered only when current hitpoints are nonpositive and
the Fighter state is neither 3 nor 4. It obtains the freelance mission,
requires mission type 13, current station equal to the mission's target
station, and a clear `Status` byte at `+0xf1`. If the Fighter name has
storage and compares equal to game-text entry 1663, it sets status byte
`+0xf0` to one. Otherwise the original writes halfword `0x0100` at
`+0xf0`, i.e. clears `+0xf0` and sets `+0xf1` in little-endian ARM.
The host source writes the two named bytes separately. The same
type-13/target-station/flag relation appears in the independent
`Level::createMission` implementation, but the ASM is the primary source
for this death-side transition.

The production-method Fighter smoke checks null name, matching and
nonmatching name, repeated flag-gated frames, mismatched station and
mission type, excluded state 3, positive health, and absent mission.
All 11 Fighter smokes, native UCRT64 `gof2`, all 208 NDK r18b ARM units,
and 93 verifier tests pass (one existing skip). Focused whole-`update`
source-shape/strict fuzzy changes **44.3%/12.2% → 45.9%/20.1%**.
The 44-function Fighter unit retains **21 linked-exact** and **10
byte-exact** pairs.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact**
matches, with zero skipped, wrong-type, or exact-pair losses and 124
extra symbols. Comparing `(unit, symbol)` pairs with the preceding
damage-particle report finds only the expected `PlayerFighter::update`
score change. Full report: `fighter-update-freelance-death-full.json`
(SHA-256
`5683e832fbe25b9be14932878527ada06f2a46f1f308983cc50a13a1f8b4748f`).

Wanted rewards, death effects, movement and the common tail still
remain behind `PF_update_body`; no complete game frame is validated.
