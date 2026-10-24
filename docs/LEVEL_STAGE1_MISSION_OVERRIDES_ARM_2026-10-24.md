# Pan stage-one mission override boundary (2026-10-24)

## Source evidence and scope

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Level::createMission` at `0xaffd8`. After the initial count writes,
`0xb06a6–0xb07ac` calls the pirate-base, hidden-blueprint, alien-attack and
wanted services. `0xb07cc–0xb08d4` computes ninja chance, handles the
freelance/attacked-friends cases and applies the four-friend fallback when
all ordinary counts vanish. `0xb08d8–0xb0a2a` handles special station IDs,
campaign 36/37 and 42/43, black-market and pirate-loot overrides. The
station-108 branch begins at `0xb0a2a`; object creation is later.

`tools/run_level_stage1_mission_overrides_pan_gate.py` extends the existing
original-asset Pan scenario with the source-extracted normal path from
`int pirateBase` to just before station-108 dispatch. It links the production
`Status`, `Station`, `Wanted`, `Mission` and `Route` services reached by that
slice. The fixture checks that Pan has no pirate base, hidden blueprint,
alien attack, wanted target, attacked friends, black-market or pirate-loot
state. The RNG replay now includes the four-friend fallback before comparing
final ordinary counts. The gate is intentionally bounded: it does **not**
certify the station-108 branch, guarded-freighter branch, ship spawning,
stage-one return or a playable frame. Other override branches are compiled
but not exercised by the Pan fixture.

## Validation state

The new gate passes Python syntax checking and compiles as an Android ARMv7
`-O0` executable with NDK r18b. The device disappeared from ADB immediately
before that executable could be pushed; **the new runtime assertions have
not passed yet**. A Windows build with WebKit ANGLE compiles, but its driver
does not advertise ETC1 and the inherited sun-upload gate correctly stops
before mission logic. Neither compile-only result is a runtime pass.

The existing hostile-route gate did execute through the mission path on the
physical ARMv7 device at `-O0` after the r18b runner was configured with
its legacy headers, GNU ARM linker, static libc++ and libunwind. Its `-O2`
run reached the asteroid gate but failed the pre-existing first-asteroid
scale/quality range assertion **before** the new mission slice. The new
gate prints both values if this recurs so the assertion can be diagnosed
without weakening it. This is a regression in the r18b test configuration
relative to the earlier documented NDK 25 O0/O2 passes, not evidence that
the new mission override logic is wrong.

Production C++ is unchanged in this package, so the last verified corpus
remains **4,323/4,524** originals, **4,736** pairs, **94.09%** mean
source-shape, **83.41%** strict fuzzy, **2,557 linked-exact** and **1,082
byte-exact**, with zero skipped and wrong-type symbols. The native and
208-unit ARM project builds were not rerun for this test-only package.

Next: reconnect the phone, run the new gate at O0, inspect the O2 asteroid
diagnostic and only then promote this boundary to a green checkpoint.

## Ordinary Pan enemy-array continuation (prepared, not runtime-certified)

`tools/run_level_stage1_enemy_array_pan_gate.py` now extends that same
fixture through the guarded-freighter test, the second one-waypoint Route,
mining/outpost/supernova conditions and the ordinary `Array<KIPlayer*>`
allocation, stopping before the first `createShip`. Station 108 and the
guarded-freighter object path fail closed rather than being simulated.
The original ELF calls the second Route constructor at `0xb0ae6–0xb0af6`,
tests the campaign-167 mining-station case at `0xb0e30–0xb0e64`, makes the
outpost rolls at `0xb0eb0–0xb0f3c`, and allocates the ordinary enemy array
at `0xb2a36–0xb2a8c`. The gate independently replays the second route's
three coordinate draws and the conditional outpost rolls, then checks its
waypoint, final RNG state, counts and zero-initialized array length.

Both `-O0` and `-O2` Android ARMv7 executables compile and link with NDK
r18b. The phone remains absent from ADB, so neither executable has been
run. In particular, the new assertions and the earlier r18b `-O2`
asteroid failure remain unresolved. No production source or corpus metric
changed at this step. The next runtime milestone is the first spawned
ordinary friend, then the remaining array and stage-one return.

## First-asteroid assertion correction (pending device rerun)

The inherited near-asteroid assertion used `scale >= 1.2f`, although the
production expression is `float(radius) * 0.01f` for integer radius
`120..219`. At radius 120, float32 multiplication yields
`1.1999999284744263`, below the float32 literal `1.2f`
(`1.2000000476837158`). This is a false-rejection case, not evidence of
an incorrect game value. The gate now captures the actual integer radius
and checks both its `120..219` range and the exact production scale
formula. The full asteroid-field gate retains the first asteroid's radius
when it processes later objects. The prior `-O2` failure might have been
this boundary case, but without its logged value that cause is unproven.

The updated source-extracted chain prepares successfully, and both Android
ARMv7 `-O0` and `-O2` gate executables compile and link with NDK r18b.
The device is still absent from ADB; no runtime pass or metric change is
claimed. Next: run both executables on the device and inspect the printed
radius, scale and quality if either check still fails.

## Windows SwiftShader runtime update (2026-10-24)

The later `run_level_stage1_enemy_array_pan_gate.py` passed its complete
inherited Pan chain at both `-O0` and `-O2` on Windows using the Android
SDK's SwiftShader GLES implementation. Both executions printed
`Level::init stage 0 returned and stage 1 allocated the Pan ordinary enemy
array before spawning: OK` and exited successfully. The driver advertised
ETC1, so the inherited original-sun upload and texture checks also ran;
the previous WebKit ANGLE attempt stopped there and did not test the mission.
This pass includes the mission-override assertions and the corrected
first-asteroid radius/scale assertion, so those checkpoints are now
host-runtime verified. It does not establish a physical Android ARM runtime
pass for either new boundary: ADB still lists no device. The gate stops before
the first ordinary `createShip`, remaining enemy population, stage-one return
and a playable frame. Production C++ did not change, so the corpus metrics
above remain the latest applicable measurements.
