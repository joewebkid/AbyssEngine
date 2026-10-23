# Fighter state-1/state-7 direction prelude (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xde9c6–0xded44`. This patch covers the
non-shooting fallthrough of the shared state-1/state-7 branch before its
still-unrecovered orientation and movement calculations.

When Fighter `+0x148` holds a target and route-motion flag `+0x12c` is
clear, the original first checks whether the current world-direction
vector at `+0x170` lies strictly inside a component-wise cube. The cube
has half-width **8000**; a target marked at Player `+0x69` expands it to
**12000** only if the Level player exists and is docked to a docking
point. Inside the cube, the direction is replaced by the Fighter
geometry's right vector. Equality to either bound is outside.

The selected world direction is normalized. The inverse of the
Player's transform matrix rotates it into ship-local coordinates; the
local Y component is then negated. Fighter `+0x24` receives the Player
byte at `+0x68` (`empDisabledByte` in the current layout). The latter
is a byte transfer confirmed by the ARM loads and stores; this patch
does not infer additional meaning for the Fighter field.

The original branches into a shooting path when `+0x12e` is set and
`+0x12c` is clear. That path is not recovered here. The implemented
fallthrough is therefore gated to the route/non-shooting condition;
later orientation, combat and translation still reach `PF_update_body`.
No complete game-frame route or target-following run is validated.

The production-method fixture checks a quarter-turn inverse transform,
Y reflection and byte transfer, the strict 8000 boundary, the docked
12000 boundary, and right-vector substitution at `-O0/-O2` using mock
target, route and Level providers.

Focused NDK r18b verification compiles all 208 ARM translation units
and compares 44 Fighter pairs. Whole-`PlayerFighter::update`
source-shape/strict fuzzy changes **81.1%/33.7% → 83.3%/34.1%** and
remains nonexact. All 21 linked-exact and 10 byte-exact Fighter pairs
are retained; only the target function's score changes. Native UCRT64
`gof2`, all 11 Fighter smokes and 93 verifier tests pass (one existing
skip).

The full ARM corpus compares **4,323/4,524** original functions across
**4,735** pairs at **94.05%** mean source-shape and **83.38%** mean strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact** pairs,
with zero exact-pair losses, skipped or wrong-type symbols. Pairwise
comparison against the preceding full report shows that only
`PlayerFighter::update` changes score. The 201 missing originals remain
unimplemented; the corpus means round to the same displayed values.
