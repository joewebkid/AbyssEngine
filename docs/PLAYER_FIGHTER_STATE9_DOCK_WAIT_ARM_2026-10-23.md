# Fighter state-9 docking wait and route advance (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` state-9 dispatch at `0xde1c4` and path at
`0xde714–0xde83e`. Fighter `+0x2bc` is the integer elapsed dock timer;
`+0x2c0` is the last derived count in the special mission branch;
interpreting it as passenger progress follows the called methods,
not a separately verified field name. The nearby state-8 arrival path
clears both slots before entering state 9.

The original fetches the current mission, increments elapsed time by
signed `dt`, then checks for a nonempty mission of type 184 docked at a
`PlayerFixedObject` whose docking type is 1. It converts elapsed time
to float, divides by **1500.0f** or, when Fighter `+0x7c` is 51, by
**200.0f** (literals at `0xde70c/0xde710`), and truncates to the new
integer count. If campaign mission 94 is active, a mission
status value no greater than the current ship's max passengers blocks
the status change. Otherwise, when the derived count grew and the
level's delivered count remains below the mission's production-good
amount, the mission status is reduced by the count delta and clamped
to zero if negative.

At elapsed time **equal** to `Route::getDockingTime()`, state 9 waits.
Only when elapsed time is strictly greater does the original restore
exhaust, call `reachWaypoint(getCurrent())`, set state 1, re-enable the
two damage-particle emitters if Fighter `+0x1fc` is set, and release
and clear the occupied `SpacePoint *` at `+0x2c4` when non-null.
There are no local null/type checks for the mission, route, or docking
target. This reconstruction follows the observed call contract but
does not establish those upstream preconditions or full game-frame
docking behavior.

The production-method fixture checks the exact timer boundary and
completion effects, both divisor choices, derived-count increments,
campaign-94 ship-capacity gate, delivered-count cap and status clamp
at `-O0/-O2`. The fixture uses mock mission, route and docking-target
providers; a real mission/route lifecycle remains untested.

Focused NDK r18b verification compiles all 208 ARM translation units
and compares 44 Fighter pairs. All 21 linked-exact and 10 byte-exact
pairs are retained. Whole-`PlayerFighter::update` source-shape/strict
fuzzy changes **70.6%/26.9% → 73.8%/27.2%** and remains nonexact.
Native UCRT64 `gof2`, all 11 Fighter smokes and 93 verifier tests pass
(one existing skip). The full `--no-build --fail-on-wrong-type` corpus
compares **4,323/4,524** original functions across **4,735** pairs, with
zero skipped or wrong-type symbols. Mean source-shape/strict fuzzy
remains **94.05%/83.38%**. All **2,557 linked-exact** and **1,082
byte-exact** pairs from the state-8 baseline are retained. A pair-by-pair
`(unit, symbol)` comparison found no exact losses or gains and exactly
one changed score: `PlayerFighter::update` as above.
