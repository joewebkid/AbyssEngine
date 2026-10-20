# Level::createAsteroids ARM recovery — 2026-10-20

Primary source is Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The function begins at Thumb `0xaf6f8`. Its call sequence at
`0xaf76e–0xaf788` loads the random singleton, obtains the current station
index and calls `AERandom::setSeed`; the original routine calls bounded
`nextInt(40)`, `nextInt(100000)`, `nextInt(8)` and `nextInt(100)`. The sorted probability
array is interleaved `[id, threshold]` pairs. The four mesh IDs in original
`.rodata` at `0x1fb808` are `0x4204, 0x4219, 0x4208, 0x4994`.

The earlier C++ treated the random-pointer slot as an `int`, reseeded with
the pointer value, multiplied a probability offset twice, used incorrect
mesh/LOD/radius values, read asteroid position out of an integer offset,
and passed the wrong PlayerAsteroid ID/quality. The corrected source keeps
the pointer typed, seeds from the station index, reads pairs directly and
uses the original table. The original calls `BoundingSphere` with the
generated center and radius 100000, uses LOD meshes `mesh+1..mesh+3`,
chooses iPad/near/far LOD ranges separately, and derives a 4–7 quality
grade from float-to-double scale comparisons (`0xafbf2–0xafc42`).

Focused ARM verification for `_ZN5Level15createAsteroidsEv` changes from
**83.5% to 89.4% source-shape** and **29.6% to 47.3% strict fuzzy**.
It remains nonexact. The final focused report is
`asteroids-focused-final.json` (SHA-256
`e91b303d5aa0dd6fbda5ca44f09206d5e4d2334a49b16a49ee4034f0b0ad2b4a`).
The full ARM corpus compares **4,323/4,524** original functions in
**4,735** pairs at **93.95%** mean source-shape and **83.24%** mean strict
fuzzy, with **2,551 linked-exact**, **1,079 byte-exact** and zero skipped
units. Both exact `(unit, symbol)` sets have zero losses and zero gains
against the preceding source tree. Report `asteroids-full-final.json` has
SHA-256 `f73a5ab9b28f719d1b42bb1b3c410d225bc4acfd9acada3a1596500525886d2`.

`tools/run_level_stage1_asteroid_prelude_pan_gate.py` makes two continuous
`Level::init` calls after original Pan assets. The first returns from stage
zero; the second enters the production asteroid prelude and stops before
`Waypoint` construction. On Windows SwiftShader and Android ARMv7 at
`-O0/-O2`, it verifies the stage stays 1, the seeded array length and
center coordinates, and the preceding **38/38** file lifetimes. Its Galaxy
probability provider is a bounded test service. The gate does **not**
execute the probability-selection loop, construct any asteroid, prove
collision/LOD behavior, finish stage one, or render a playable frame.
The native `gof2` static library builds, all **208** ARM translation units
compile, and the verifier test suite passes **93** tests with one existing
skip.

Next: carry this gate through original probability generation, Waypoint and
BoundingSphere construction, asteroid geometry/player creation, then
`createGasClouds` and the remaining stage-one dispatch. The currently
unverified branches include alien orbit, special campaign origins, iPad
LOD and quality randomization.
