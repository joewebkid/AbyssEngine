# Pan stage-one hostile route after scene return (2026-10-20)

## Evidence and executed path

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::createMission` at `0xaffd8`. After returning from the
mode-zero `createScene` call, it restores mission mode three at `0xb01d8`.
The original then reads system and station, rolls `nextInt(100)` at
`0xb0208`, checks the freelance mission and pirate-base branch, computes
security/attackers, draws route X/Z at `0xb04e0` and `0xb04f6`, constructs
`Route(int*, 3)` at `0xb050a`, and stores it at `Level+0x180` at `0xb0512`.

`tools/run_level_stage1_hostile_route_pan_gate.py` extends the continuous
original-asset Pan gate with that **source-extracted post-scene slice**.
It links the production `Status`, `Mission`, `SolarSystem` and `Route`
methods needed by the path. The fixture confirms a non-null empty mission,
no freelance mission, a non-early system and no pirate base. The test checks
the route's one waypoint and one docking slot, its owner link, zero Y, and
its X/Z against a separate replay of the three RNG calls after the
time-seeded reset. Its inherited gates still check the Pan assets, full
asteroid field, gas clouds and mode-zero scene cleanup.

Windows SwiftShader and physical Android ARMv7 pass at `-O0` and `-O2`.
The first host attempt failed because the test's text slicer selected an
earlier occurrence of a repeated `int race` line; no route had run. The
slice was corrected to search **after** `bool earlySystem`, and both host
and device passes are from the corrected script. This was a test-harness
error, not evidence of a production-code defect.

The complete `createMission` body is not linked or executed. The slice
stops immediately after route creation. Pirate-base, freelance, early-system,
other mission and populated-enemy paths, attacker counts and stage-one
return are not certified by this gate.

## Metrics and next priority

Production C++ is unchanged. The last canonical NDK r18b full report
remains **208/208** ARM translation units, **4,323/4,524** original
functions across **4,735** pairs, **93.97%** mean source-shape, **83.29%**
strict fuzzy, **2,554 linked-exact**, **1,082 byte-exact**, zero skipped
and zero wrong-type. This test-only step claims no new exact match. Native
`gof2` and the 93-test verifier suite were green on this production
revision (one verifier skip when temporary files used D:).

Next, recover the post-route hostile/friendly count decisions and their
object-creation boundary with source-backed fixtures; then continue toward
stage-one return. A playable frame remains a separate milestone.
