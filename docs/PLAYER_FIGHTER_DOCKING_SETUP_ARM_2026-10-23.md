# Fighter navigation-to-docking setup (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdd938–0xdda1c` and
`0xdf55a–0xdf75a`. This recovers the bounded state-1/state-7 route branch
which prepares state 8, not the rest of state-1/state-7 movement.

When a route waypoint has a docking target, state 1 becomes state 7.
The original selects the nearest navigation point using the Fighter's
current position and occupied point at `+0x2c4`. A changed point releases
the old lease with `giveFree()` and takes the new one. The target's
position plus its rotated local navigation offset becomes the Fighter's
`objectPosition` at `+0x9c`. Distance from the Fighter is truncated to
an integer before comparing with **2000**. At or beyond that distance,
`+0x164` receives the navigation position; below it, state becomes 8.

On that near branch, the original destroys any prior `EaseInOutMatrix`
at `+0x2b8` and clears the pointer. It selects the docking point nearest
the navigation world position, rotates and normalizes the docking
direction, derives an up vector from the rotated navigation and docking
offsets, then normalizes their cross product for the right vector.
An identity-based destination matrix receives those basis vectors.
The new `EaseInOutMatrix` starts from the Player's transform matrix,
ends at that destination and uses **3000 ms**. State-8 processing may
then run in the same update call. Original allocation is `0xf4` bytes,
consistent with the recovered ARM class layout.

The production-method fixture checks point acquisition/replacement,
old ease-object destruction, navigation and docking positions with a
translated target, both sides of the 2000 threshold, destination basis,
3000-ms duration, and same-frame state-8 use at `-O0/-O2`.
The point-selection providers are mocks. The original has no local null
check for a returned docking point; the reconstruction follows that
contract but does not verify its upstream precondition. A physical
game-frame route/docking run and remaining state-1/state-7 motion are
still untested.

Focused NDK r18b verification compiles all 208 ARM translation units
and compares 44 Fighter pairs. `PlayerFighter::update` source-shape and
strict fuzzy change **73.8%/27.2% → 79.7%/27.9%**; the function remains
nonexact. All 21 linked-exact and 10 byte-exact Fighter pairs are retained.
Native UCRT64 `gof2`, all 11 Fighter smokes and 93 verifier tests pass
(one existing skip). The full `--no-build --fail-on-wrong-type` corpus
compares **4,323/4,524** original functions across **4,735** pairs, with
zero skipped or wrong-type symbols. Mean source-shape/strict fuzzy remains
**94.05%/83.38%** after rounding. All **2,557 linked-exact** and **1,082
byte-exact** pairs from the state-9 baseline are retained. Pair-by-pair
comparison finds zero exact losses or gains and only one changed score:
`PlayerFighter::update` as above.
