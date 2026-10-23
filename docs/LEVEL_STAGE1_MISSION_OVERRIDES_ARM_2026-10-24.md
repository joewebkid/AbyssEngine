# Pan stage-one mission override boundary (2026-10-24, pending runtime)

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
