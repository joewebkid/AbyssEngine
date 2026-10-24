# Pan first class-one freighter prelude and constructor — host gate (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` `Level::createShip` (`0xc0214`)
compares `shipClass` with 1 at `0xc032c`. Its class-one path chooses an
HP multiplier of 5 (25 for type 14) at `0xc033c–0xc034a`, triples EMP
points at `0xc0346` and sets EMP duration to 45000 at `0xc034c`.
For the ordinary Pan freighter type 15, campaign mission 50 and the
fixture's difficulty 1.0, this gives 2025 HP. The existing full-function
ARM comparison remains **96.4% source-shape / 48.5% strict fuzzy**,
nonexact. `PlayerFixedObject` constructor C1/C2 remains **97.4% /
67.7%**, nonexact; these are not improvements claimed by the host test.

`tools/run_level_stage1_first_freighter_prelude_pan_gate.py` extends
the original-asset Pan chain through the first `field_168` dispatch
and the source-extracted `createShip` prelude. It checks class 1,
type 15, the RNG-selected race (Pan or the foreign faction), null
waypoint, HP/EMP and the three jitter coordinates. A replay from the
captured RNG state checks draw order; it uses the reconstructed RNG
algorithm, so it is not an independent validation of that algorithm.

`tools/run_level_stage1_first_freighter_ctor_pan_gate.py` then invokes
the production `PlayerFixedObject` constructor with the captured
arguments and original item catalog. It checks the Player back-reference,
faction/type, float and integer positions, initial moving/bounds/wreck
fields, spawned flag and loot-list shape. Uncalled virtual functions
fail closed. The constructor is invoked by the test after the prelude
boundary: the gate does **not** yet execute the class-one branch to
`Level::createShip` return or insert this freighter into `enemies`.

The fixture keeps the original RNG draws but forces `field_164 = 1`
and `field_168 = 1`, sets synthetic security 3, and excludes outposts
after consuming their selection draws. These changes guarantee this
ordinary-freighter path; they do not represent Pan's natural random
distribution or verify the outpost branches. Original Pan catalog,
mesh and texture assets are read-only. Windows SwiftShader `-O0/-O2`
passes after **3/4** ordinary friends and one pre-dead Fighter.

No production C++ changed. The last full ARM corpus remains
**4,323/4,524** original functions over **4,736** pairs, mean
**94.19% source-shape / 83.54% strict fuzzy**, with **2,731
100%-fuzzy**, **2,564 linked-exact**, **1,082 byte-exact**, **201
missing**, zero wrong-type and zero skipped. The previous revision
passed native `gof2`, **208/208** ARM units and **93** verifier tests
(one skip); those were not repeated for this test-only package.

The next priority is to execute the class-one `createShip` branch
through collision bounds, ship-group/LOD setup and return, then verify
insertion and the large/small outpost alternatives. The allocated
freighter is not destroyed by this bounded gate; lifetime, update/render,
stage-one return and live Android behavior remain unverified. ADB
listed no device.
