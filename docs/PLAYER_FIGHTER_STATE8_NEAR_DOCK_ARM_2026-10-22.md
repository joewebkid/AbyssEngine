# Fighter state-8 near-docking transition (2026-10-22)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` state dispatch at `0xde1c4` and state-8
entry/arrival path at `0xde5da–0xde6e8`. This package restores the
near-docking transition only. The far approach continues into the
unrecovered easing/movement path at `0xde8a4` and the existing
`PF_update_body` placeholder.

The Fighter's route at `+0x6c` supplies a docking target. Its world
position plus its geometry matrix applied to the Fighter's `SpacePoint`
at `+0x2c4` supplies an approach point for
`KIPlayer::getNearestDockingPoint`. The selected SpacePoint is then
rotated by the target's matrix and added to its world position; this
becomes Fighter `objectPosition` at `+0x9c`. The original compares the
integer-converted length from the Fighter's virtual `getPosition()` to
that object position against a ship-kind-indexed table at Android
`.rodata` `0x1fe130`. All 64 32-bit table entries were checked against
the original binary and match the existing Level hangar-offset table.
The table's apparent other use does not change this exact state-8 read.

When distance is strictly below the indexed threshold, the original
hides exhaust, clears Fighter `+0x2bc/+0x2c0`, sets state 9, and disables
the two damage-particle emitters if `+0x1fc` is set. Equality follows
the far path. The production-method fixture covers approach-point
composition, nearest-point selection, the arrival side effects, two
ship-kind thresholds and an exact threshold boundary. It uses mocks for rotation,
nearest-point selection, and the virtual position calls; it is not a
physical docking or complete game-frame test. The original branch has
no local null or kind-index bounds checks; this package does not prove
all upstream preconditions. The far approach, state 9 and shared
movement tail remain unverified. `Route::getDockingTarget()` is exact in
the previous corpus, but `KIPlayer::getNearestDockingPoint()` and
`KIPlayer::getPosition()` are not; the original `getPosition` assembly
does confirm the Player matrix at `+0x4` as its input. The real nearest-
point selection remains a separate validation dependency.

Next mapped branch, not implemented: at `0xde8a4–0xde930`, the far path
advances `easeMatrix` at Fighter `+0x2b8` by float-converted `dt`, reads
its matrix value, computes `(objectPosition - getPosition()) * dt *
0.0006f` (literal at `0xdec28`), adds that displacement to the current
position, updates the matrix translation, assigns the matrix to the
geometry and Player transform, then exits through `0xdf450`. This map
does not validate ease timing, actual motion or the pointer ownership.

Validation completed 2026-10-23: the production-method fixture passes at
`-O0/-O2`; all 11 Fighter smokes, native UCRT64 `gof2`, and 93 verifier
tests pass (one existing skip). The focused ARM build compiled all 208 NDK
r18b translation units and compared 44 Fighter pairs without linked-exact
or byte-exact losses (21 and 10 respectively). The subsequent full
`--no-build --fail-on-wrong-type` corpus compared **4,323/4,524** original
functions across **4,735** pairs, with zero skipped or wrong-type symbols.
Mean source-shape/strict fuzzy remains **94.04%/83.38%**, and all
**2,557 linked-exact** and **1,082 byte-exact** pairs from the state-6
baseline are retained. A pair-by-pair `(unit, symbol)` comparison found
no exact losses or gains and exactly one changed score:
`PlayerFighter::update` source-shape/strict fuzzy **66.4%/26.4% ->
68.9%/27.9%**. The function is still nonexact. This verifies code-shape
non-regression, not a complete game-frame or physical docking run.
