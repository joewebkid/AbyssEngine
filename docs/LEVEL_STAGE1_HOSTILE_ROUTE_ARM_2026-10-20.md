# Pan stage-one hostile route and initial counts (2026-10-20)

## Evidence and executed path

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::createMission` at `0xaffd8`. After returning from the
mode-zero `createScene` call, it restores mission mode three at `0xb01d8`.
The original then reads system and station, rolls `nextInt(100)` at
`0xb0208`, checks the freelance mission and pirate-base branch, computes
security/attackers, draws route X/Z at `0xb04e0` and `0xb04f6`, constructs
`Route(int*, 3)` at `0xb050a`, and stores it at `Level+0x180` at `0xb0512`.
At `0xb0524–0xb06a2` it chooses an enemy race, calculates the initial
hostile count with difficulty/level adjustments, draws two auxiliary count
fields and an additional friend roll, then writes `friendCount` at
`Level+0x160`. The campaign-32-or-later
override draws an additional `nextInt(100)` when attackers are present.

`tools/run_level_stage1_hostile_route_pan_gate.py` extends the continuous
original-asset Pan gate with that **source-extracted post-scene slice**
through the initial count decisions. It links the production `Status`,
`Mission`, `SolarSystem` and `Route`
methods needed by the path. The fixture confirms a non-null empty mission,
no freelance mission, a non-early system and no pirate base. The test checks
the route's one waypoint and one docking slot, its owner link, zero Y, and
its X/Z and the four count fields against a separate replay of the RNG
calls after the time-seeded reset. The fixture sets difficulty to `1.0f`
and retains campaign mission 50; the replay includes the conditional
campaign roll and checks the final RNG seed. Its inherited gates still
check the Pan assets, full asteroid field, gas clouds and mode-zero scene
cleanup.

Windows SwiftShader and physical Android ARMv7 pass at `-O0` and `-O2`.
The first host attempt failed because the test's text slicer selected an
earlier occurrence of a repeated `int race` line; no route had run. The
slice was corrected to search **after** `bool earlySystem`. A later test
assertion incorrectly assumed an early campaign although the fixture uses
mission 50; the replay now models its extra conditional roll. Both host
and device passes are from the corrected script. These were test-harness
errors, not evidence of production-code defects.

The complete `createMission` body is not linked or executed. The slice
stops after the initial count decisions. Pirate-base, freelance,
early-system, hardcore-difficulty, other mission and populated-enemy paths,
later count overrides, actual ship spawning and stage-one return are not
certified by this gate. The RNG replay uses the reconstructed `AERandom`
implementation; it checks call order, not that RNG algorithm independently.

## Metrics and next priority

Production C++ is unchanged. The last canonical NDK r18b full report
remains **208/208** ARM translation units, **4,323/4,524** original
functions across **4,735** pairs, **93.97%** mean source-shape, **83.29%**
strict fuzzy, **2,554 linked-exact**, **1,082 byte-exact**, zero skipped
and zero wrong-type. This test-only step claims no new exact match. Native
`gof2` and the 93-test verifier suite were green on this production
revision (one verifier skip when temporary files used D:).

Next, recover the later mission-specific count overrides and their
object-creation boundary with source-backed fixtures; then continue toward
stage-one return. A playable frame remains a separate milestone.
