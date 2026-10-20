# ASM Validation: original `.so` ↔ our build

> Read [PROJECT_STATUS.md](PROJECT_STATUS.md) before interpreting a saved
> metric. `report.json` is a dated snapshot; a fresh local verifier run is the
> authoritative result for the revision and environment being inspected.

## Pan stage-one mode-zero scene cleanup (2026-10-20)

`run_level_stage1_scene_zero_pan_gate.py` extends the original-asset Pan
path through a source-extracted mode-zero `Level::createScene` slice. It
checks cleanup of an empty allocated enemy array and return to
`Level::createMission`, which restores mode three before the next boundary.
Windows SwiftShader and physical Android ARMv7 pass at `-O0/-O2`.
Populated enemy cleanup, other scene modes and post-scene mission behavior
remain untested. No production C++ changed; the full ARM corpus metrics
below remain applicable. See
[mode-zero scene evidence](LEVEL_STAGE1_SCENE_ZERO_ARM_2026-10-20.md).

## Pan stage-one empty-mission prefix (2026-10-20)

`run_level_stage1_empty_mission_prefix_pan_gate.py` continues the
original-asset Pan path through production `Level::init` and a source-
extracted slice of the empty `Level::createMission` path. It executes mission
and position reads, and verifies the time-derived RNG reset,
`missionPtr == 0`, and entry to `createScene`. Alien and nonempty paths fail
closed; `createScene` is a stub boundary, not a completed scene. Windows
SwiftShader and physical Android ARMv7 pass at `-O0/-O2`. The production
revision and corpus metrics are unchanged from the following checkpoint. See
[empty-mission prefix evidence](LEVEL_STAGE1_EMPTY_MISSION_PREFIX_ARM_2026-10-20.md).

## Pan stage-one mission dispatch (2026-10-20)

`run_level_stage1_mission_dispatch_pan_gate.py` extends the original-asset Pan
path to the empty-mission `createMission` call. Production `Level::init`,
`Mission::isEmpty` and `Status::getMission` execute with a non-null empty
mission; the fixture also checks `Mission::isCampaignMission`. `setMission`'s
null branch and `createMission` are outside the gate.
Windows SwiftShader and physical Android ARMv7 pass at `-O0/-O2`. Production
C++ is unchanged, so the last full ARM corpus stays **4,323/4,524** originals,
**4,735** pairs, **93.97%/83.29%** mean source-shape/strict fuzzy,
**2,554 linked-exact** and **1,082 byte-exact**, with zero skipped or
wrong-type symbols. The 93-test verifier suite passes with one skip. See
[mission dispatch evidence](LEVEL_STAGE1_MISSION_DISPATCH_ARM_2026-10-20.md).

## Gas-cloud recovery (2026-10-20; full ARM corpus verified)

`run_level_stage1_gas_clouds_pan_gate.py` executes production
`Level::createGasClouds` after the complete Pan asteroid field. A synthetic
equipment item and bounded plasma-probability fixture enter the positive
branch; a second call without that item checks the early return. Windows
SwiftShader and physical Android ARMv7 pass at `-O0/-O2` with the recovered
`PlayerGasCloud` layout. Native `gof2` and both changed ARM translation units
compile with NDK 25. The original constructor places `center` at `+0x128`
and the mesh IDs at `+0x164/+0x168`; the reconstructed ARM object is now
`0x16c` bytes. The intervening `+0x124` slot has unknown semantics. The
original ARMv7 function at `0xafd2c` supports the recovered branch, count
and placement logic. Follow-on direct calls now verify the alien-orbit early
return and the mission-`0x8e`/station-`0x4f` boss count and fixed first-cloud
position on Windows and physical Android ARMv7 at `-O0/-O2`. Full stage-one
return, real plasma probabilities, other alien/boss conditions and cloud
updates remain untested.

The local canonical NDK r18b toolchain verified all **208/208** ARM translation
units and **4,735** pairs covering **4,323/4,524** original functions, with
zero skipped and zero wrong-type symbols. Mean source-shape/strict fuzzy is
**93.97%/83.29%**; **2,554 linked-exact** and **1,082 byte-exact** pairs
include all previous exact matches plus three `PlayerGasCloud` gains in each
set. Focused `createGasClouds` is **93.9%/55.2%**, nonexact, versus
**88.3%/41.5%** before. Native `gof2` is green. The verifier unit suite passes
**93 tests, one skip** with temporary files on D:. The prior two linker
failures when C: was full were environmental. See
[gas-cloud evidence](LEVEL_GAS_CLOUDS_ARM_2026-10-20.md).

## Pan Level stage-one complete asteroid field (2026-10-20)

`run_level_stage1_asteroid_field_pan_gate.py` continues the original-asset
Pan path through all production `Level::createAsteroids` iterations and its
return, stopping on entry to `createGasClouds`. Windows SwiftShader and
physical Android ARMv7 pass at `-O0/-O2`. The bounded Galaxy probability
fixture supplies one asteroid kind; near/far density is time-seeded, so the
gate checks valid groups rather than a fixed density. Gas clouds, stage-one
return and asteroid update/collision/render are not covered. No production
C++ changed, so the prior native build and full ARM corpus remain applicable:
**4,323/4,524** originals, **4,735** pairs, **93.96%/83.24%** means,
**2,551 linked-exact**, **1,079 byte-exact**, zero skipped.
See [full-field evidence](LEVEL_ASTEROID_FIELD_PAN_ARM_2026-10-20.md).

## Pan Level stage-one first PlayerAsteroid (2026-10-20)

`run_level_stage1_first_asteroid_pan_gate.py` continues the original-asset
Pan path through the production first `PlayerAsteroid` and `Explosion`
constructors, insertion, level binding and center assignment. Windows
SwiftShader and Android ARMv7 pass at `-O0/-O2`. The test checks both
explosion meshes, player health/radius, quality, position and ownership.
Galaxy probabilities remain a bounded fixture; non-executed virtual methods
are fail-closed, and the remaining field and stage-one return are untested.
Original ARM GOT relocations identify `Globals::Canvas` and `Globals::rnd`
as the `Explosion` global sources. Native `gof2`, **208/208** ARM TUs and
**93** verifier tests pass (one existing skip). Focused mean `Explosion`
source-shape improves **90.93% → 92.25%**, with four linked-exact and
three byte-exact pairs unchanged. Full ARM: **4,323/4,524** originals,
**4,735** pairs, **93.96%/83.24%** mean source-shape/strict fuzzy,
**2,551 linked-exact**, **1,079 byte-exact**, zero skipped and zero
exact-pair losses/gains. See
[first-asteroid evidence](EXPLOSION_GLOBALS_FIRST_ASTEROID_ARM_2026-10-20.md).

## Pan Level stage-one first asteroid geometry (2026-10-20)

`run_level_stage1_asteroid_first_geometry_pan_gate.py` continues the
original-asset Pan path through Waypoint, collision sphere, first asteroid
mesh, three LODs and scale/quality selection. Windows SwiftShader and Android
ARMv7 pass at `-O0/-O2`. The Galaxy probability provider is still a bounded
fixture; the gate stops before `PlayerAsteroid` construction. Native `gof2`,
**208/208** ARM TUs and **93** verifier tests pass (one existing skip).
Focused asteroid source-shape/strict fuzzy remains **89.4%/47.3%** and is
nonexact. `sizeof`-based allocations prevent host under-allocation while
retaining the original ARM constants; the original 0x48 BoundingSphere
allocation remains because its reconstructed layout is incomplete. The full
ARM report is byte-identical to the prior one: **4,323/4,524** originals,
**4,735** pairs, **93.95%/83.24%** mean source-shape/strict fuzzy,
**2,551 linked-exact**, **1,079 byte-exact**, zero skipped and no exact-pair
losses. See
[first-geometry evidence](LEVEL_ASTEROID_FIRST_GEOMETRY_ARM_2026-10-20.md).

## Pan Level stage-one asteroid prelude (2026-10-20)

`run_level_stage1_asteroid_prelude_pan_gate.py` makes two continuous
`Level::init` calls. Original Pan assets complete stage zero, then the
production `createAsteroids` prelude seeds from the station and creates
the expected array length and center coordinates. Windows SwiftShader
and Android ARMv7 pass at `-O0/-O2`, with **38/38** closed files. A bounded
Galaxy probability provider replaces the full calculation, and the gate
stops before Waypoint/asteroid construction. Native `gof2`, **208/208** ARM
TUs and **93** verifier tests pass (one existing skip). Focused asteroid
source-shape/strict fuzzy is **83.5%/29.6% → 89.4%/47.3%**, nonexact.
Full ARM corpus: **4,323/4,524** originals, **4,735** pairs,
**93.95%/83.24%** means, **2,551 linked-exact**, **1,079 byte-exact**,
zero skipped, zero exact-pair losses/gains. See
[asteroid recovery evidence](LEVEL_CREATE_ASTEROIDS_ARM_2026-10-20.md).

## Continuous Pan Level stage-zero return (2026-10-20)

`run_level_stage0_pan_gate.py` completes the original-asset Pan stage-zero
path after the production `createPlayer` return. Windows SwiftShader and
Android ARMv7 pass at `-O0/-O2`: `Level::init` returns `0`, the stage
advances to `1`, the player is at `(10, 10, 10000)`, and **38/38** files
close. Stage one, a continuous armed player, real particle allocation and
material loading, and a playable frame remain open. No source changed:
the last full ARM corpus is **4,323/4,524** originals, **4,735** pairs,
**93.95%/83.24%** mean source-shape/strict fuzzy, **2,551 linked-exact**,
**1,079 byte-exact** and zero skipped units. Fresh focused source-shape/
strict fuzzy is **86.1%/35.2%** for `createPlayer` and **97.5%/49.9%**
for `loadWeaponPositions`, both non-exact. See
[stage-zero return evidence](LEVEL_INIT_STAGE0_PAN_RETURN_ARM_2026-10-20.md).

## Continuous Pan createPlayer return (2026-10-20)

`run_level_player_weapon_positions_pan_gate.py` executes the production
weapon-position reader and four Pan type-3 geometries. The succeeding
`run_level_player_complete_pan_gate.py` executes cleanup and returns from
`Level::createPlayer` into the same `Level::init` call. Both pass on Windows
SwiftShader and Android ARMv7 at `-O0/-O2`, with **38/38** closed file
operations. A separate full-body CPU contract passes 118 equipped cases;
those cases are not yet continuous with the original-asset Level gate.
No C++ changed, so the last full ARM corpus for this source tree remains
**4,323/4,524** originals, **4,735** pairs, **93.95%/83.24%** mean
source-shape/strict fuzzy, **2,551 linked-exact**, **1,079 byte-exact**
and zero skipped units. See
[weapon-file integration evidence](LEVEL_CREATE_PLAYER_WEAPON_FILE_PAN_ARM_2026-10-20.md).

## Level player yaw and empty gun buckets (2026-10-20)

`run_level_player_rotation_pan_gate.py` continues the Pan `Level::init`
path through production Y-axis hull rotation and three empty gun buckets.
Windows SwiftShader and Android ARMv7 pass at `-O0/-O2`; **37/37**
file lifetimes still close. ARM `Level::createPlayer` focused
source-shape/strict fuzzy changes **86.0%/35.5% → 86.1%/35.2%** and
remains non-exact. The full ARM corpus stays at **4,323/4,524** originals,
**4,735** comparisons, **93.95%/83.24%** mean source-shape/strict fuzzy,
**2,551 linked-exact**, **1,079 byte-exact** and zero skipped units; exact
pairs did not change. Positive gun slots and weapon positions are outside
the gate. See [yaw evidence](LEVEL_CREATE_PLAYER_YAW_ARM_2026-10-20.md).

## Continuous Level-to-particle Pan gate (2026-10-20)

`run_level_player_particles_pan_gate.py` crosses production
`PlayerEgo::setLevel` during the Pan `Level::init` call. Windows SwiftShader
and Android ARMv7 pass at `-O0/-O2`. A separate mission-1 probe checks the
three manager/set/handle pairs; manager allocation and rendering remain
outside this bounded fixture. The production pointer assignment is now
64-bit-safe. Focused ARM `setLevel` stays **100%** source-shape and strict
fuzzy, with one linked-exact match. The full ARM corpus has **4,323/4,524**
originals, **4,735** comparisons, **93.95%/83.24%** mean
source-shape/strict fuzzy, **2,551 linked-exact**, **1,079 byte-exact** and
zero skipped units; the exact `(unit, symbol)` sets are unchanged. See
[level-binding evidence](PLAYER_EGO_SET_LEVEL_PAN_ARM_2026-10-20.md).

## Continuous Level-to-ship Pan gate (2026-10-20)

`run_level_player_ship_pan_gate.py` continues the production
`Level::createPlayer` source through `PlayerEgo::setShip` inside the
ongoing `Level::init` call. Windows SwiftShader and Android ARMv7 pass
at `-O0/-O2`, with five original ship AEM meshes and **37/37** closed
file operations. It stops before `setLevel`; material creation remains
a bounded test service. No production source changed, so focused
`Level::createPlayer` remains **86.0%/35.5%** source-shape/strict fuzzy.
The last full ARM corpus for this same source tree retains
**4,323/4,524** originals, **2,551 linked-exact**, **1,079 byte-exact**
and zero skipped units. See
[continuous gate evidence](LEVEL_CREATE_PLAYER_SET_SHIP_PAN_ARM_2026-10-20.md).

## PlayerEgo ordinary ship binding (2026-10-20)

Android ARM Thumb `0x99a16` clears `PlayerEgo+0x1b0..+0x1bc`; the C++
constructor now initializes the previously omitted tractor-beam,
repair-beam and asteroid-target fields. `run_player_ego_set_ship_pan_gate.py`
extracts the complete production `setShip` and runs the gear-free Pan path
with five original AEM meshes at `-O0/-O2` on Windows SwiftShader and
Android ARMv7. Both paths close **37/37** files. Native `gof2`, **208/208**
ARM TUs and **93** verifier tests pass (one prior skip). Focused constructor
source-shape/strict fuzzy changes **83.6%/51.7% → 83.9%/53.2%**.
The final corpus compares **4,323/4,524** originals in **4,735** pairs,
averaging **93.95%/83.24%** source-shape/strict fuzzy. There are **0**
skipped ARM units and no changes to the **2,551 linked-exact** or
**1,079 byte-exact** `(unit, symbol)` sets.
This gate starts after the `Level::createPlayer` constructor boundary and
uses a bounded material service; it does not prove a completed
`Level::init` or equipment/material branches. See
[ship-binding evidence](PLAYER_EGO_SET_SHIP_PAN_GATE_2026-10-20.md).

## Globals ship-group tables and original-asset loader (2026-10-20)

The five production `Globals::getShipGroup` tables now contain all **576**
values from the original Android ARM `.rodata`, verified byte-for-byte.
Three formerly zero-ID `TransformAddMesh` calls now pass the ARM-observed
additive and engine mesh IDs with `false` flags. A focused extracted-branch
test covers ordinary and exceptional indexes. The continuous Pan gate calls
the full production `getShipGroup(0,0,true)` after `PlayerEgo` construction
and loads five original Midorian AEM files at `-O0/-O2` on Windows
SwiftShader and Android ARMv7; **37/37** original-file operations close.
Native `gof2`, **208/208** ARM units and **93** verifier tests pass (one
existing skip). Focused source-shape/strict fuzzy moves
**82.6%/18.7% → 83.0%/7.7%**, with no exact match for this function.
The next boundary is `PlayerEgo::setShip` with material state; the full
`Level::init` still has not returned. See
[ship-group evidence](GLOBALS_SHIP_GROUP_TABLES_ARM_2026-10-20.md).
The final full ARM corpus compares **4,323/4,524** original functions
(**4,735** pairs, **201** absent, **0** skipped), averaging **93.95%**
source-shape and **83.24%** strict fuzzy. Its **2,551 linked-exact** and
**1,079 byte-exact** symbol sets have no losses from the previous baseline.

## PlayerEgo complete normal-path constructor (2026-10-20)

`tools/run_station_player_ego_stars_gate.py` now stops after the production
`PlayerEgo` constructor returns in the continuous Pan `Level::init` gate.
Windows SwiftShader and Android ARMv7 pass at `-O0/-O2`: all 50 stars,
five original-AEI HUD images, steering/visibility defaults, normal-path
supernova fields and **32/32** file lifetimes are asserted. The guarded
supernova effect, positive black-market case, `setShip`, and completion of
`Level::init` remain outside this gate. The original ARM constructor calls
`Status::inBlackMarketSystem` and writes the result at `PlayerEgo+0x354`;
the C++ now restores that initialization. Focused constructor
source-shape/strict fuzzy rises **82.9%/50.8% → 83.6%/51.7%**; neither
alias is exact. Native `gof2`, **208/208** ARM units and **91** verifier tests
pass (one prior skip). The full corpus remains **4,323/4,524** originals,
**4,735** pairs, **93.95%/83.24%** mean source-shape/strict fuzzy,
**2,551 linked-exact** and **1,079 byte-exact**; the exact `(unit, symbol)`
sets are unchanged and ARM skipped units are zero. See
[the constructor evidence](PLAYER_EGO_CONSTRUCTOR_PAN_GATE_2026-10-20.md).

## PlayerEgo HUD atlases and equipment sounds (2026-10-20)

At the earlier HUD checkpoint, `tools/run_station_player_ego_stars_gate.py`
exercised five production
`Image2DCreate` calls through two original AEI atlases and real GLES uploads
on Windows SwiftShader and Android ARMv7 at `-O0/-O2`. It checks the AEI
region coordinates, managed texture handles, GPU pixel readback and
**32/32** file lifetimes. Synthetic Items of sorts 19, 9 and 10 exercise
the three newly restored equipment-gated sound branches. At that checkpoint
the constructor test copy stopped before steering-limit initialization;
`setShip` and full `Level::init` were not covered. Focused ARM constructor source-shape/strict
fuzzy improves **78.5%/47.6% → 82.9%/50.8%**; exact aliases remain
nonmatching. Native `gof2`, **208/208** ARM TUs and **91** verifier tests
pass (one prior skip). Full corpus: **4,323/4,524**, **4,735** pairs,
**93.95%/83.24%** means, **2,551 linked-exact** and **1,079 byte-exact**;
exact `(unit, symbol)` sets have no losses or gains. See
[HUD/sound evidence](PLAYER_EGO_HUD_SOUNDS_ARM_2026-10-20.md).

## PlayerEgo Ship-defaults gate (2026-10-20)

At the earlier checkpoint `tools/run_station_player_ego_stars_gate.py`
continued through `Player::getCombinedHP`, the empty-equipment synthetic
Ship's boost,
handling, cargo and shield-regeneration values, sound-resource registration
and `Status::hardCoreMode` on Windows SwiftShader and Android ARMv7 at
`-O0/-O2`. The boundary is before the first HUD image. This does not
establish equipment-present behavior, full `PlayerEgo` construction or
`Level::init` completion. Production code and the full ARM metrics below
are unchanged. See [Ship-defaults evidence](PLAYER_EGO_SHIP_DEFAULTS_GATE_2026-10-20.md).

## PlayerEgo / MovingStars constructor-prefix gate (2026-10-20)

`tools/run_station_player_ego_stars_gate.py` passes the inherited Pan
original-asset/GLES checks while continuing through `Level::createPlayer`
into the extracted `PlayerEgo` prefix and real `MovingStars` constructor on
Windows SwiftShader and Android ARMv7 at `-O0/-O2`. It checks 50 procedural
billboards/transforms, their resource material, RNG and parked positions.
The canvas mesh allocator/writers are explicit test services. That earlier
checkpoint was before `getCombinedHP`; the later Ship-defaults gate above
crosses it. Neither establishes `Level::createPlayer` completion or a full
`PlayerEgo` runtime.
No production source changed; the latest full ARM report remains
**4,323/4,524**, **93.95%/83.24%** mean source-shape/strict fuzzy,
**2,551 linked-exact**, **1,079 byte-exact**. See
[gate evidence](PLAYER_EGO_MOVING_STARS_PREFIX_GATE_2026-10-20.md).

## Level::createPlayer Player allocation (2026-10-20)

The continuous original-asset Pan/GLES gate executes the production `Player`
constructor and shield/armor setters from `Level::init` on Windows SwiftShader
and Android ARMv7 at `-O0/-O2`. Hull 300, shield 80, armor 90 and three empty
gun buckets are asserted. The synthetic Ship has no installed equipment;
the boundary is before `PlayerEgo`. Native `gof2`, ABI/lifetime smoke and
**91 verifier tests** pass (one prior skip). No production code changed, so
the prior full ARM corpus remains **4,323/4,524** originals, **4,735 pairs**,
**93.93%/83.22%** means, **2,551 linked-exact**, **1,079 byte-exact**. See
[Player-allocation evidence](LEVEL_CREATE_PLAYER_ALLOCATION_GATE_2026-10-20.md).

## Level::createPlayer slot entry (2026-10-20)

The continuous original-asset Pan/GLES gate now calls the production
`createPlayer` from `Level::init` and executes its Ship/equipment/three-slot
prelude on Windows SwiftShader and Android ARMv7 at `-O0/-O2`. At that
checkpoint the boundary stopped before `Player` allocation; the later gate
above crosses it. The Ship has empty equipment by fixture.
Native `gof2`, ABI/lifetime smoke and **91 verifier tests** pass (one prior
skip). No production source changed; the prior full ARM report is unchanged:
**4,323/4,524** originals, **4,735 pairs**, **93.93%/83.22%** means,
**2,551 linked-exact**, **1,079 byte-exact**. See
[slot-entry evidence](LEVEL_CREATE_PLAYER_SLOT_ENTRY_GATE_2026-10-20.md).

## Level::createPlayer weapon factor bits (2026-10-20)

Original Android ARM passes the two `Ship` factor getters' raw words
directly into VFP float registers. The corrected production branch uses
`bitsToFloat` at all three call sites. Extracted `-O0/-O2` tests cover
ordinary/negative factors; native `gof2`, ABI/lifetime smoke, **208/208**
ARM units and **91 verifier tests** pass (one prior skip). Focused
`createPlayer` strict fuzzy rises **34.5% → 35.5%**, source-shape remains
**86.0%** at one decimal, nonexact. Full corpus: **4,323/4,524**
originals, **4,735 pairs**, **93.93%/83.22%** mean source-shape/strict
fuzzy, **2,551 linked-exact**, **1,079 byte-exact**. Exact-pair sets have
zero gains or losses; skipped and wrong-type signatures are zero. The
ignored report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/level-player-factors-full.json`
(SHA-256 `79c04bc41f531c75cceb997f4e3304229011790690c5dffef864c1442e2dc50a`).
The repaired CPU contract executes the full extracted body against all 59
original weapon records in both equipment orders at `-O0/-O2` (118 cases);
its historical negative control still fails at the expected turret selector.
The later continuous slot-entry gate reaches `createPlayer` but stops before
`Player` allocation; see
[weapon-factor evidence](LEVEL_CREATE_PLAYER_FACTOR_BITS_ARM_2026-10-20.md).

## Pan Level::createSpace return (2026-10-20)

Production `Level::createSpace` now returns into `Level::init` in the
original-asset Pan gate at `-O0/-O2` on Windows SwiftShader and Android
ARMv7. A separate `systems.bin` decoder confirms the star RGB **21/18/10**
and derived Level color fields. No production source changed; the prior
full ARM report for this same source revision remains **4,323/4,524**
originals, **4,735 pairs**, **93.93%/83.22%** source-shape/strict fuzzy,
**2,551 linked-exact** and **1,079 byte-exact**, with zero skips or
wrong-type signatures. At that checkpoint the gate stopped before
`createPlayer`; the later slot-entry gate above crosses it. See
[Pan createSpace evidence](LEVEL_PAN_CREATE_SPACE_RETURN_2026-10-20.md).

## Pan light and StarSystem constructor return (2026-10-20)

The original-asset Level/GLES gate now executes all real Engine light
setters and `PaintCanvas::FogEnable` in Pan, then observes the production
`StarSystem` constructor returning into `Level::createSpace`. Windows
SwiftShader and Android ARMv7/Adreno pass at `-O0/-O2`, checking light and
material state, 18 meshes, 22 transforms, 44 collision volumes and 26/26
closed asset reads. No production source changed; the last full ARM report
for this exact source revision remains **4,323/4,524** originals,
**93.93%/83.22%** mean source-shape/strict fuzzy, **2,551 linked-exact**,
**1,079 byte-exact**, zero skipped and wrong-type signatures. Shader-enabled
lighting, fog variants and Level stage 0 remain open. See
[Pan light/constructor evidence](STARSYSTEM_PAN_LIGHT_CONSTRUCTOR_GATE_2026-10-20.md).

## Engine scene ambient (2026-10-20)

Original ARM `0x85836–0x85840` stores the float-one bit pattern in
`Engine::sceneAmbient[3]`; source now uses `1.0f`. The original-asset
Level/GLES gate executes the real setter at `-O0/-O2` on Windows SwiftShader
and Android ARMv7/Adreno, checking RGB, alpha and the GLES1-compatible
call. Native `gof2`, ABI/lifetime smoke, **208/208** ARM units and **89
verifier tests** pass (one pre-existing skip). Focused setter
source-shape/strict fuzzy rises **91.3%/52.2% → 93.4%/68.1%**, nonexact.
Full ARM corpus remains **4,323/4,524** originals, **4,735 pairs**,
**93.93%** mean source-shape, **83.22%** strict fuzzy, **2,551 linked-exact**,
**1,079 byte-exact**. Exact `(unit, symbol)` sets have zero gains or losses;
skipped units and wrong-type signatures are zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-ambient-full.json`
(SHA-256 `53beb4739cda71c4412ce6f9d411a979db413be872dd444527b8e43fea3c832f`).
This gate covers the fixed-function branch only and stops before rim color;
see [scene-ambient evidence](ENGINE_SCENE_AMBIENT_ARM_2026-10-20.md).

## All orbit planets and StarSystem light getter (2026-10-20)

The Pan original-asset Level/GLES gate now crosses all five orbit planets,
including two occupied-slot retries and the selected station, then enters
the real `StarSystem::initLight` after RNG reset. Original Android ELF
relocation `0x210374` identifies the old null callback as
`ApplicationManager::GetEngine`; the source now calls it and the gate checks
the resulting `Engine` and Pan sun color on Windows SwiftShader and Android
ARMv7 at `-O0/-O2`. Native `gof2`, ABI/lifetime smoke, **208/208** ARM units,
and **88 verifier tests** pass with one prior skip. Focused `initLight` rises
**24.6%/11.0% → 81.3%/44.9%** source-shape/strict fuzzy; constructor aliases
rise **85.2%/19.2% → 85.6%/40.6%**. Neither is exact. The gate stops before
the first light setter. The full corpus compares **4,323/4,524** originals
in **4,735 pairs**, at **93.93%** mean source-shape and **83.21%** strict
fuzzy, with **2,551 linked-exact** and **1,079 byte-exact**. Independent
exact `(unit, symbol)` set comparison against the preceding report finds
zero gains or losses; skipped units and wrong-type signatures remain zero.
The ignored report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-getter-full.json`
(SHA-256 `485e71a53bdd1c6627b6bd4de0d92f689e18b67fbb19179a42afffedda6b57e1`).
Fog, constructor return and Level stage 0 are open.
See [all-planets/light evidence](STARSYSTEM_ALL_PLANETS_LIGHT_ARM_2026-10-20.md).

## StarSystem first non-selected planet (2026-10-20)

Original Android ARM confirms the occupied-slot rejection loop,
`nextInt(40)` double-precision scale, selected-station distance branch,
pitch/yaw paths and optional final turn. The original-asset Level/GLES gate
now executes the first non-selected planet on Windows SwiftShader and Android
ARMv7 at `-O0/-O2`, checking slot 15, scale, pose, stored position and RNG
state. Native `gof2`, ABI/lifetime smoke, **208/208** ARM units and **87
verifier tests** pass with one previous skip. Constructor aliases change
**78.1%/13.3% → 85.2%/19.2%** source-shape/strict fuzzy, nonexact. The
complete corpus compares **4,323/4,524** originals in **4,735 pairs**,
with **93.92%** mean source-shape, **83.20%** strict fuzzy, **2,551
linked-exact** and **1,079 byte-exact**. Exact `(unit, symbol)` set
comparison against the previous report finds zero gains or losses; skipped
units and wrong-type signatures remain zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/first-planet-full.json`
(SHA-256 `aa1c18f1d31a2896618e2ba6a6498fb97bed7bd2f6d9754b3bc37918604872be`). The
rejection retry and selected-station branch are not covered by this fixture;
Level stage 0 remains incomplete. See
[first-planet evidence](STARSYSTEM_FIRST_PLANET_ARM_2026-10-20.md).

## StarSystem first sun pose and normalize ABI (2026-10-20)

The original-asset Level/GLES gate now executes the second RNG draw, first
sun rotation and `moveForward(-20000.0f)`, then checks the published position
and light direction on Windows SwiftShader and Android ARMv7 at `-O0/-O2`.
Original ARM confirms the return-by-value `VectorNormalize` target in
`AEGeometry`, `PlayerFighter` and `PlayerGasCloud`; the pointer overload was
removed from their C++ calls. Native `gof2`, a fresh ABI/lifetime smoke,
**208/208** ARM units and **86 verifier tests** pass with one previous skip.
Constructor aliases change **76.3%/14.3% → 78.1%/13.3%**
source-shape/strict fuzzy. The full corpus retains **4,323/4,524** originals
in **4,735 pairs**, **93.92%** mean source-shape, **83.19%** strict fuzzy,
**2,551 linked-exact** and **1,079 byte-exact**. Independent exact
`(unit, symbol)` set comparison with the preceding report finds zero gains
or losses; skipped units and wrong-type signatures remain zero. The ignored
full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/sun-pose-full.json`
(SHA-256 `6304b675f7dadf1db4903d0041a5c17ba25efa30f4334ba994d06bb8d8864355`).
The first non-sun planet and
Level stage 0 remain unverified. See
[first-sun-pose evidence](STARSYSTEM_SUN_POSE_ARM_2026-10-20.md).

## StarSystem first sun setup (2026-10-20)

The continuous original-asset Level/GLES gate now checks the first
`nextInt(14)` result, RNG state, sun slot 5 and the real sun/streak scales
from ARM literals on Windows SwiftShader and a real Android ARMv7 device at
`-O0/-O2`. It reaches 18 meshes, 17 transforms, 44 collision volumes and
26/26 closed file opens before stopping ahead of rotation. Two focused tests
check original ARM branches/literals and compile the production normal and
supernova scale expressions at both optimizations. The bounded RNG oracle
checks 2,560 transitions. Native `gof2` builds in a C: directory, the
existing ABI/lifetime smoke passes, **208/208** ARM units compile and **85
verifier tests** pass with one prior skip. Constructor aliases change
**76.2%/13.0% → 76.3%/14.3%** source-shape/strict fuzzy. The full corpus
compares **4,323/4,524** originals in **4,735 pairs** at **93.92%** mean
source-shape, **83.19%** strict fuzzy, **2,551 linked-exact** and
**1,079 byte-exact**. Exact `(unit, symbol)` sets have no gains or losses;
skipped units and wrong-type signatures remain zero. The ignored report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/sun-scale-full.json`
(SHA-256 `3a61800f236b6848db5135772dc5fb1faebd0aae3cb7df7a90075f13ff959c80`).
The full supernova asset path and original rotation draw remain open. See
[sun-setup evidence](STARSYSTEM_SUN_SETUP_ARM_2026-10-20.md).

## StarSystem first-geometry boundary (2026-10-20)

Android ARM calls the first `AEGeometry` constructor and publishes
`planetsArray[0]` before `nextInt(14)`. The corrected production constructor
follows that order. The continuous original-asset gate passes at `-O0/-O2`
on Windows SwiftShader and a real Android ARMv7/Adreno device: it loads
`plane.aem`, constructs and publishes the geometry, and confirms that the
RNG has not yet drawn. Native `gof2`, ABI/lifetime smoke, **208/208** ARM
units and **83 verifier tests** (one pre-existing skip) pass.

The full report compares **4,323/4,524** original functions in **4,735**
pairs: **93.92%** mean source-shape, **83.19%** strict fuzzy,
**2,551 linked-exact**, **1,079 byte-exact**. Exact `(unit, symbol)` sets
have no gains or losses against the preceding seed report; zero skipped units
and wrong-type signatures. Both constructor aliases change
**76.7%/18.4% → 76.2%/13.0%** source-shape/strict fuzzy. The ignored report
is `C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/geometry-full.json`
(SHA-256 `5a6a2fcbce57b891a4e24280dc89af7e4c0c436b40922189215f8cf546a762ae`).
Stage 0 remains incomplete; see
[first-geometry evidence](STARSYSTEM_FIRST_GEOMETRY_ARM_2026-10-20.md).

## StarSystem seed-width gate (2026-10-20)

The original Android ARM constructor uses `mul.w` then `asrs` before
`AERandom::setSeed`; the rebuilt object now has the same sequence instead of
the previous `smull`. The production-expression test covers six station
indices against real `AERandom` at `-O0/-O2` and rejects the old expression.
The original-asset Level/GLES gate reaches and checks the real seed call on
Windows SwiftShader and Android ARMv7 at both optimizations. Native `gof2`,
ABI/lifetime smoke, **208/208** ARM units and **83 verifier tests** (one
pre-existing skip) pass.

The full report compares **4,323/4,524** original functions in **4,735**
pairs: **93.92%** mean source-shape, **83.20%** strict fuzzy,
**2,551 linked-exact**, **1,079 byte-exact**. Exact `(unit, symbol)` sets
have zero losses and zero gains against the preceding combined report. Both
StarSystem constructor aliases improve from **76.6% to 76.7%** source-shape;
strict fuzzy remains **18.4%**. The ignored report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/seed-after-full.json`
(SHA-256 `5f10a3fa2e5c2d9f09cfe59adeddabda0dc0b484c1859af837981c25cc4e281c`).
Level stage 0 and a playable frame remain incomplete. See
[seed evidence](STARSYSTEM_SEED_ARM_2026-10-20.md).

## Combined weapon-consumer and five-station-loop gate (2026-10-20)

The integrated native `gof2` static library and ABI/lifetime smoke pass.
The production `createPlayer` host gate passes all 59 original weapon records
at `-O0/-O2` (118 equipment-order cases); the old body fails its negative
control. The continuous original-asset station gate passes on Windows
SwiftShader and a real Android ARMv7/Adreno device at `-O0/-O2`. It verifies
all five station textures, record cleanup and typed target/planet/position
arrays, then stops before `AERandom::setSeed(2100)`. Level stage 0 and a
native game frame remain incomplete.

The full combined ARM build compiles **208/208** units; **81 verifier tests**
run with one pre-existing skip and no failures. It compares **4,323/4,524**
original functions in **4,735 pairs**: **93.92%** mean source-shape,
**83.20%** strict fuzzy, **2,551 linked-exact** and **1,079 byte-exact**.
Against the last
published report, `(unit, symbol)` exact sets have **zero losses** and three
linked-exact gains, all in `StarSystem`; byte-exact gains are zero. There are
zero skipped units, zero wrong-type signatures, 201 absent originals and 124
extra symbols. The fresh ignored report is locally stored at
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/integration-combined-full.json`
(SHA-256 `887da2e22e095a95f775c4cc3f43503bf54ebbe06595bcaa555d73926c87f5cd`).
See [weapon consumers](CREATEPLAYER_WEAPON_CONSUMERS_ARM_2026-10-20.md) and
[station loop](STATION_LOOP_ARRAYS_ARM_2026-10-20.md) for ARM anchors, fixed
families, asset hashes, verification boundaries and remaining work.

## Combined weapon-position and station-texture gate (2026-10-20)

The integrated native `gof2` build and ABI/lifetime smoke pass. The original-
asset weapon-position contract passes at `-O0/-O2`: all 59 IDs, 552 entries, four slots, scalar
read order, cleanup and a controlled failed-read negative case. The continuous
Windows SwiftShader and Android ARMv7 Adreno gate passes at `-O0/-O2`, checks
the atlas, all nine ETC1 sun mips, and exact first station-orbit RGBA base
pixels; generated RGBA mips are a separate bounded box-reference check. It
completes the first station-loop iteration and stops before texture 10021.
The isolated eight-case texture-loop smoke also passes at both optimizations.
Neither `Level::init` stage 0 nor a native game frame is completed.

The full combined ARM run builds **208/208** units with zero skips and wrong-
type signatures. It compares **4,323/4,524** originals in **4,731 pairs**:
**93.91%** mean source-shape, **83.19%** strict fuzzy, **2,548 linked-exact**,
**1,079 byte-exact**. Exact `(unit, symbol)` set comparison with the preceding
published report shows **zero lost and zero gained** linked- or byte-exact
pairs. The changed `FileRead.o` and `StarSystem.o` hashes equal their isolated
branch outputs. All **74 verifier tests** pass with one pre-existing skip.
The fresh ignored report is `_work/decomp_weapon_station_full_20261019.json`
(SHA-256 `f5a8968437ed5c84c4b5ae65065cf68b494cc737c88365f12d0fac9607e966db`).
The fixed family goals remain unmet: weapon positions **97.5%/49.9%** and
station texture **90.525%/48.175%** source-shape/strict fuzzy. See the
[weapon-position](WEAPON_POSITIONS_CONTRACT_ARM_2026-10-19.md) and
[station-texture](STATION_TEXTURE_STARSYSTEM_ARM_2026-10-19.md) notes for
original ARM addresses, asset checks and unverified semantics.

## Combined String float and ETC1 sun gate (2026-10-19)

The source-backed float caller now passes helper mode 1; `String(float)` C2
becomes linked-exact. The original ETC1 sun is uploaded through GLES on
Windows SwiftShader and Android Adreno 618 at `-O0/-O2`, with all nine mip
levels independently checked. The continuous gate stops before station
`TextureCreate(10014)`; it does not complete StarSystem or Level stage 0.
Native `gof2`, the ABI smoke, two bounded String gates, **208/208 ARM units**
and **69 verifier tests** (one previous skip) pass. The full corpus compares
**4,323/4,524** originals in **4,731 pairs**: **93.91%** mean source-shape,
**83.19%** strict fuzzy, **2,548 linked-exact**, **1,079 byte-exact**, zero
lost exact pairs, zero skipped units and zero wrong-type signatures. The
ignored report `_work/decomp_float_sun_full_20261019.json` has SHA-256
`ec8ea688f4a0dbd3603d001baf415a54aabeac0bdd744e9a78a5ff1a0f4b8b33`.
See [the float](STRING_FLOAT_FAMILY_ARM_2026-10-19.md) and
[sun](SUN_ETC1_STARSYSTEM_ARM_2026-10-19.md) evidence for branch-specific
checks, failed 98% sun family target and remaining semantic limits.

## Combined HD-tail/LensFlare integration gate (2026-10-19)

Native `gof2` and all 208 ARM translation units build. The verifier suite
passes 52 tests with one pre-existing skip. The source-extracted resource
gate compares all 256 flag combinations with the original ARM ELF at
`-O0/-O2`. The continuous original-asset Level gate now includes the
ordinary **234-record platform tail**, registers 2,724 resources total,
and reaches production `TextureCreateFromFileIntern::glGetError` through
`LensFlare`/`Image2DCreate` at both optimizations. It returns no texture,
does not complete `LensFlare` or Level stage 0, and does not run a game or
validate GLES. The separate cached-atlas CPU checks do not cross this
continuous GPU boundary. See [HD-tail](BUILD_RESOURCE_HD_TAIL_ARM_2026-10-19.md)
and [LensFlare](LENSFLARE_IMAGE2D_ENTRY_ARM_2026-10-19.md) evidence.

The fresh combined full report is ignored at
`_work/decomp_hd_lensflare_full_20261019.json` (SHA-256
`8cc79a127547bbcdc11753b77639127cb661f6416c31111506ed68ff1233c802`).
It compares 4,323/4,524 original functions in 4,731 pairs: 93.90% mean
source-shape, 83.17% mean strict fuzzy, 2,545 linked-exact and 1,079
byte-exact. Independent normalized `(unit, symbol)` set comparison with
the prior integrated baseline gives zero lost and zero gained exact pairs.
There are no skipped ARM units or wrong-type signatures. All 208 combined
ARM objects are byte-identical to the corresponding object in at least one
of the two isolated branches: 205 to both, `BuildResourceList.o` to the
resource branch, `LensFlare.o` and `PaintCanvas.o` to the image branch.

## Combined station/iPad integration gate (2026-10-19)

The integrated source has a green native `gof2` build and 208/208 ARM
translation units. The complete resource-table/platform host gate and the
continuous station-entry gate both pass at `-O0/-O2` with original Android
assets; 49 verifier tests pass with one pre-existing skip. The station gate
reaches `StarSystem` and deliberately stops at the unverified
`LensFlare::Image2DCreate(0x508)` boundary. It does not run a native game.
See [station](LEVEL_STATION_ENTRY_2026-10-19.md) and
[ordinary iPad](BUILD_RESOURCE_IPAD_TAIL_ARM_2026-10-19.md) evidence.

Fresh full ARM report: `_work/decomp_combined_full_20261019_final.json`
(ignored, SHA-256 `614f6dd4c335a720eb952318b3d95e5112ff9a12060a9e68575e0f15dba57c95`).
It compares 4,323/4,524 original functions in 4,731 unit/symbol pairs,
with 93.90% mean source-shape, 83.16% mean strict fuzzy, 2,545 linked-exact
and 1,079 byte-exact. Against the preceding combined baseline report,
normalized `(unit, symbol)` sets show **zero lost**, **16 gained**
linked-exact and **zero lost**, **three gained** byte-exact pairs. There are
zero skipped units and zero wrong-type signatures. The two recovered paths
remain bounded: full `BuildResourceList` and `Level::init` execution,
rendering and ARM-device behavior are not established.

## BuildResourceList missing-slot gate (2026-10-19)

Android ARM evidence restores ten omitted records in the 2,488-slot
resource list. The `Level` asset-backed gate loads restored texture 33410
from the original OBB at `-O0/-O2` and checks its cached handle; restored
mesh 14287 passes resource-ID `MeshCreate` into production
`MeshCreateFromFile`/`MeshReadData` with its original file. Animated-mesh
clone-on-reuse remains untested. All 2,488 table positions now have
records; the full construction/registration segment passes a bounded
host gate, but later resources and consumers remain untested. See
[the ARM evidence](BUILD_RESOURCE_LIST_GAPS_ARM_2026-10-19.md).
Native `gof2`, 208/208 ARM TUs and 40 verifier tests pass (one existing
skip). Full corpus: 4,323/4,524 compared, 2,529 linked-exact and
1,076 byte-exact, no exact-pair losses; mean source-shape/strict fuzzy
93.87%/83.06%. Focused `BuildResourceList` is 73.8%/21.7%, against
73.5%/22.8% before this patch. The strict fuzzy decline is recorded,
not treated as a regression in the verified resource records.
The follow-up material cluster adds four more ARM-backed records and
corrects one existing texture ID. Native and 208/208 ARM units build;
the same asset-backed gate passes `-O0/-O2`, and 40 verifier tests pass
(one existing skip). Full corpus remains 4,323/4,524 compared,
2,529 linked-exact and 1,076 byte-exact, with no prior exact-pair loss.
Mean source-shape/strict fuzzy is 93.87%/83.05%; focused
`BuildResourceList` is 73.9%/13.6%. The sizeable strict fuzzy fall is
documented, not hidden behind source-shape or corpus means.
The production `SetResourceList` body now calls bulk `ArrayAdd<Resource*>`
and is exercised on four source-table entries in the asset-backed host
gate at `-O0/-O2`. Their registration order and subsequent resource-ID
loading are checked. Focused ARM `SetResourceList` remains 100%/100%
source-shape/strict fuzzy and linked-exact. This is not yet a full
2,488-entry table execution gate.
Native `gof2` and 208/208 ARM units build; 40 verifier tests pass (one
existing skip). Full corpus remains 4,323/4,524 compared, 93.87%/83.05%
mean source-shape/strict fuzzy, 2,529 linked-exact and 1,076 byte-exact.
The previous full report's exact-pair sets have zero losses.
The following ship-material pass fills 28 omitted ARM table records and
corrects 15 existing material payloads. A source-record test checks all
41 consecutive slots 634–674 and edge slots 632/681; it is a table
regression guard, not a rendering test. Twelve slots remain unrecovered,
and texture IDs 34042/34050/34150 still lack direct C++ records.
Native `gof2`, 208/208 ARM units, the selected asset-backed `-O0/-O2`
gate, and 42 verifier tests pass (one existing skip). Focused
`BuildResourceList` source-shape/strict fuzzy is 74.7%/17.6%, up from
73.9%/13.6%, nonexact. Full ARM corpus remains 4,323/4,524 compared,
mean 93.87%/83.05%, 2,529 linked-exact and 1,076 byte-exact; the
exact-pair comparison with the preceding full report has zero losses.

The visitor-glow/mesh follow-up restores eight ARM-backed slots, corrects
three texture records, one material payload and two mesh IDs, leaving
four unrecovered slots (977, 997, 998, 1700). Native and 208/208 ARM
units build; 44 verifier tests pass (one existing skip), and the
selected original-asset gate passes at `-O0/-O2`. Focused
`BuildResourceList` source-shape/strict fuzzy is 74.9%/18.4%, up from
74.7%/17.6%, nonexact. Full corpus compares 4,323/4,524 with means
93.87%/83.05%, 2,529 linked-exact, 1,076 byte-exact, and zero losses
from the preceding exact-pair sets. Full-table execution remains open.

The final-slot follow-up fills positions 977/997/998/1700 and moves
mesh 6801 from slot 310 to its ARM slot 1700, restoring texture 33010
at 310. `test_resource_material_table.py` confirms exactly one record
at every index 0–2487. `run_full_resource_table_smoke.py` executes
all source-extracted table entries, constructors and production bulk
`SetResourceList` at `-O0/-O2`, checks non-null registration and the
five repaired records. The GL prelude, post-table additions and
`Level::init` are not included in this gate.
Native and 208/208 ARM units build, 45 verifier tests pass (one existing
skip), and the selected original-asset gate passes at `-O0/-O2`.
Focused `BuildResourceList` source-shape/strict fuzzy is 75.0%/21.2%
(previously 74.9%/18.4%), nonexact. Full corpus: 4,323/4,524 compared,
93.87%/83.06% mean source-shape/strict fuzzy, 2,529 linked-exact,
1,076 byte-exact, zero previous exact-pair losses.

The first `iPadHD` post-table branch now runs in the full-table host gate
at `-O0/-O2`: disabled adds zero 1440 resources, enabled adds twelve,
including original ID 27343 and its `_ipad_1440` shop-button path. The initial GL
call, later platform branches, remaining post-table resources and
`Level::init` are still outside this gate.
Native and 208/208 ARM units build; 45 verifier tests pass (one existing
skip), and the selected original-asset gate passes at `-O0/-O2`.
Focused `BuildResourceList` is unchanged at 75.0%/21.2%
source-shape/strict fuzzy, nonexact. Full corpus: 4,323/4,524
compared, 93.87%/83.06% means, 2,529 linked-exact, 1,076 byte-exact,
zero previous exact-pair losses.

The next branch gate source-extracts the nested `iPad`/`retinaDisplay`
checks from the same production body. It covers four combinations at
`-O0/-O2`: no HD package, twelve base 1440 records, twelve base
records without Retina, and twenty records including the eight
`ipad_large` entries. It checks that the first shop-button path is
`_ipad_1440` and the later one is `_ipad_large`. This is a bounded
branch gate, not a complete execution of the post-table tail.
Native and 208/208 ARM units build; 45 verifier tests pass (one
existing skip); the selected original-asset gate is green. The focused
`BuildResourceList` source-shape/strict fuzzy result is 75.0%/21.3%,
still nonexact. The full corpus remains 4,323/4,524 compared,
2,529 linked-exact and 1,076 byte-exact with zero prior exact-pair
losses; mean source-shape/strict fuzzy is 93.87%/83.06%.

The next platform-branch gate additionally checks the source-extracted
`else if (Globals::retinaDisplay)` run when `iPadHD=0`: Retina enabled
adds sixteen `iphone4` records, ending with shop-button ID 27343;
Retina disabled adds none in this earlier bounded gate. The original ARM
branches to the default-resource path in the latter case, recovered in
the following pass. Native `gof2`, 208/208 ARM units, the original-asset
gate and 45 verifier tests pass; the table/branch gate passes at
`-O0/-O2`.
Focused `BuildResourceList` source-shape/strict fuzzy is 75.0%/21.4%,
still nonexact. Full corpus coverage remains 4,323/4,524 with
93.87%/83.06% mean source-shape/strict fuzzy, 2,529 linked-exact
and 1,076 byte-exact; prior exact-pair losses are zero.

The ordinary non-Retina branch is now included in the source-extracted
table gate. With `iPadHD=0` and `retinaDisplay=0`, it registers nine
common textures and another six when `iPad=0`. The gate checks all
fifteen ordered ID/path pairs for non-iPad, the nine common pairs for
iPad, and the previous HD/Retina branches at `-O0/-O2`. All fifteen
files exist in the Android OBB extraction, but this gate neither
decodes nor renders them. The iPad continuation at `0x10be20` and
the rest of `BuildResourceList` still need recovery and execution.
Native `gof2` and 208/208 ARM units build, the selected original-asset
gate passes, and 45 verifier tests pass (one existing skip). See
[the ARM evidence](BUILD_RESOURCE_LIST_GAPS_ARM_2026-10-19.md).
Focused `BuildResourceList` source-shape/strict fuzzy changes from
75.0%/21.4% to 75.4%/21.1%, still nonexact. The full corpus remains
4,323/4,524 compared, with 93.87%/83.06% mean source-shape/strict
fuzzy, 2,529 linked-exact and 1,076 byte-exact. Pairwise comparison
with the preceding full report finds zero linked- or byte-exact losses.

## Continuous Level entry and portrait suffix gates (2026-10-19)

`run_level_init_asset_gate.py` executes the production Level constructor,
`init` and `createSpace` after source-extracted registration of all 2,488
table entries and two low textures. At `-O0/-O2`, it checks ten managers,
two original skybox AEM and two AEI loads, and four matched file closes.
An explicit throw at the first `PlayerStation` constructor is the measured
boundary: stage 0 does not return, and station/player/world behavior is not
claimed. A poisoned constructor fixture verifies the host-width clear
and the three ARM-confirmed bytes. See
[the Level gate evidence](LEVEL_INIT_CONTINUOUS_ASSET_GATE_2026-10-19.md).

`run_load_portraits_smoke.py` executes the production registration body
at `-O0/-O2` for 96 flag/value cases, checking 152 ordered
IDs/paths each time. `test_load_portraits_arm.py` validates five GOT
relocations and suffix references against the original ARM binary; it
requires the configured NDK and original-binary work directory. The
original asset inventory contains six absent iPad variants, so this
gate does not claim successful texture loading or rendering. See
[the portrait evidence](PORTRAITS_GLOBAL_FLAGS_ARM_2026-10-19.md).
Native `gof2` and 208/208 ARM translation units build on the combined
revision; 46 verifier tests pass (one existing skip). The focused
resource, portrait and continuous Level gates pass at `-O0/-O2`.
The fresh full corpus compares 4,323/4,524 originals with 93.87%
mean source-shape and 83.05% mean strict fuzzy, 2,529 linked-exact
and 1,076 byte-exact. The before/after `(unit, symbol)` set comparison
has zero lost linked- or byte-exact pairs; skipped and wrong-type
counts are both zero. Focused source-shape/strict fuzzy is
75.4%/21.1% for `BuildResourceList`, 92.5%/66.8% for
`loadPortraits`, and 84.8%/52.7% for `Level::init`. These gates do not
prove complete game initialization or rendering.

## Level skybox canvas-to-loader gate (2026-10-19)

The production `MeshCreateFromFile`/`MeshReadData` host smoke parses the
two ordinary-orbit skybox meshes from Android OBB bytes. The production
`ImageCreateFromFile` host smoke parses the corresponding two textures at
`-O0/-O2`, checks host allocation size and rejects a truncated payload.
The same asset-backed tests exercise production `PaintCanvas::AddResource`,
`FindResource` and resource-ID `MeshCreate`/`TextureCreate` calls, including
cache reuse; bounded material/GPU services are explicit.
`run_level_skybox_asset_integration.py` joins the production skybox
prelude of `Level::createSpace` to those calls and all four original files
in one process at `-O0/-O2`. Its four resource records and constructors
are source-extracted from the production tables; IDs, paths and payloads
are checked before loading. Four opens occur on the first call and none
on the second; the restored asteroid-belt texture is a separate fifth
resource loaded afterward. Focused `createSpace` remains 86.1%/46.2% source-shape/
strict fuzzy, nonexact.
Native `gof2`, 208/208 ARM TUs and 40 verifier tests pass (one skip).
Focused `ImageCreateFromFile` source-shape/strict fuzzy remains
97.3%/58.8%, nonexact. See
[the asset-loader evidence](LEVEL_SKYBOX_ASSET_LOAD_2026-10-19.md).
Focused `AddResource` remains 100% source-shape/fuzzy and linked-exact.
These tests isolate the file service; they do not run complete production
resource-table construction, GPU upload, the `createSpace` tail, full `Level::init`,
or rendering.
Full ARM corpus: 4,323/4,524 compared, 93.87%/83.06% mean source-shape/
strict fuzzy, 2,529 linked-exact and 1,076 byte-exact with zero pair
losses against the preceding report. `ArrayAdd<Resource*>` gains one
linked-exact pair; one synthetic helper raises extra symbols to 125.

## PaintCanvas fog parameter gate (2026-10-19)

The production `FogSetParameter` body runs at `-O0/-O2` with test GL and
shader services. The test checks mode/density/start/end call order, four
normalized fixed-function color channels and the shader engine state.
Native `gof2`, 208/208 ARM TUs and 40 verifier tests pass (one skip).
Focused source-shape/strict fuzzy improves 65.2%/31.8% to 92.0%/96.2%,
nonexact. See [the ARM evidence](PAINTCANVAS_FOG_PARAMETER_ARM_2026-10-19.md).
All 51 skybox resource paths exist in the Android OBB extract, but no
full asset-backed level render has been validated yet.
The full corpus is 4,323/4,524 compared, with 93.87%/83.05% means,
2,528 linked-exact and 1,076 byte-exact and no exact-pair losses.

## Level player-entry host layout gate (2026-10-19)

The host layout smoke checks size-based `Player`/`PlayerEgo` allocation and
the named byte replacing raw `Player+0x69`; the existing production Player
constructor smoke passes. Native `gof2` and 208/208 ARM TUs build. Focused
`createPlayer` source-shape/strict fuzzy remains 86.5%/37.8%, nonexact.
See [the player-entry evidence](LEVEL_CREATE_PLAYER_HOST_LAYOUT_2026-10-19.md).
Full corpus: 4,323/4,524 originals, 93.86%/83.04% means,
2,528 linked-exact and 1,076 byte-exact, with zero pair losses. All 40
verifier tests pass (one existing skip). Full asset-backed level entry
remains untested.

## Level stage-0 host allocation gate (2026-10-17)

The production stage-0 allocation block and constructors run at `-O0/-O2`
with guarded host allocations and correct particle resource IDs. Native
`gof2` and 208/208 ARM TUs build. Focused `Level::init` source-shape/strict
fuzzy remains 84.8%/52.7%, nonexact. See
[the allocation evidence](LEVEL_STAGE0_HOST_ALLOCATIONS_2026-10-17.md).
Full ARM comparison remains 4,323/4,524 originals, 93.86%/83.04%
means, 2,528 linked-exact and 1,076 byte-exact, with zero pair losses.
All 40 verifier unit tests pass (one existing skip). Full asset-backed
level entry remains untested.

## Level skybox setup gate (2026-10-17)

Production-source-extracted `-O0/-O2` host smokes cover ordinary, alien,
intro, supernova, ring/storm/flare, post-effect, fog/RNG and already-created
paths. Earlier Level station, jumpgate, wormhole-tail, StarSystem-tail,
entry-position and constructor smokes pass. Native `gof2`, 208/208 ARM TUs
and 40 verifier tests pass (one skip). Focused `createSpace` source-shape/
strict fuzzy rises 71.5%/39.0% → 86.1%/46.2%, nonexact. Full ARM comparison:
4,323/4,524 originals, 93.86%/83.04% means, 2,528 linked-exact and
1,076 byte-exact, zero exact-pair losses. See
[the skybox evidence](LEVEL_SKYBOX_SETUP_ARM_2026-10-17.md). Full asset-backed
level initialization and render are not yet validated.

## Level StarSystem/stream-out gate (2026-10-17)

Source-extracted `-O0/-O2` host smokes cover ordinary/gate/station-planet
entry and the common `StarSystem`/RGB tail. Earlier station, jumpgate and
wormhole-tail smokes, native `gof2`, 208/208 ARM TUs and 40 verifier tests
pass (one skip). Focused `createSpace` source-shape/strict fuzzy rises
67.2%/32.3% → 71.5%/39.0%; `init` rises 71.7%/47.2% → 84.8%/52.7%,
nonexact. The test doubles do not verify asset-backed full `Level::init`.
See [the entry evidence](LEVEL_ENTRY_STARSYSTEM_ARM_2026-10-17.md).
Full ARM comparison: 4,323/4,524 originals, 93.86% mean source-shape,
83.04% strict fuzzy, 2,528 linked-exact and 1,076 byte-exact with zero
exact-pair losses versus the preceding wormhole/entry-angle report.

## Level wormhole/entry-angle gate (2026-10-17)

The source-extracted `-O0/-O2` tail smoke covers ordinary, attacked-station,
alien, victorious, visible/hidden wormhole and entry-angle paths. Prior
station/jumpgate smokes, native `gof2`, 208/208 ARM TUs and 40 verifier
tests pass (one skip). Focused `Level::createSpace` source-shape/strict
fuzzy rises 55.0%/11.3% → 67.2%/32.3%, still nonexact. Focused
`PlayerWormHole` constructor C1/C2 rises to 97.1%/93.6%, update to
95.6%/53.6%, still nonexact. These smokes do not exercise full stage 0/1.
See [the wormhole evidence](LEVEL_SPACE_TAIL_WORMHOLE_ARM_2026-10-17.md).
Full ARM comparison: 4,323/4,524 originals, mean source-shape 93.86%,
strict fuzzy 83.03%, 2,528 linked-exact and 1,076 byte-exact with no
exact-pair losses against the preceding jumpgate report.

## Level jumpgate-landmark gate (2026-10-17)

The source-extracted `-O0/-O2` host smoke covers entry/exit gates, no-warp
suppression, alien race selection, and a missing station. The station smoke
continues to pass. Native `gof2`, 208/208 ARM TUs and 40 verifier tests pass
(one skip). Focused `Level::createSpace` source-shape rises 32.0% → 55.0%,
strict fuzzy 15.2% → 11.3% (nonexact). Focused `PlayerJumpgate` constructor
source-shape is 97.8% (strict fuzzy 86.0%, nonexact). These tests do not yet
execute full `Level::init`. See
[the jumpgate evidence](LEVEL_CREATE_SPACE_JUMPGATES_ARM_2026-10-17.md).
Full ARM comparison: 4,323/4,524 originals, mean source-shape 93.84%,
strict fuzzy 83.02%, 2,528 linked-exact, 1,076 byte-exact; no exact-pair
losses versus the previous station-landmark report.

## Level station-landmark gate (2026-10-17)

The source-extracted `-O0/-O2` host smoke covers creation, suppression,
exceptional station indices, and DLC renaming. Native `gof2`, 208/208 ARM
TUs and 40 verifier tests pass (one skip). Focused `Level::createSpace`
source-shape/strict fuzzy rises from 22.2%/11.6% to 32.0%/15.2%, nonexact.
The smoke does not run the real `PlayerStation` constructor or full level
entry. Full ARM coverage remains 4,323/4,524 originals, 93.83%/82.99%
means, 2,528 linked-exact and 1,076 byte-exact with no exact-pair losses.
See [the station evidence](LEVEL_CREATE_SPACE_STATION_ARM_2026-10-17.md).

## Level ordinary entry-position gate (2026-10-17)

The source-extracted `-O0/-O2` host smoke covers ordinary and first-mission
entry coordinates. Native `gof2`, 208/208 ARM TUs and 40 verifier tests pass
(one skip). Focused `Level::init` source-shape/strict fuzzy rises from
69.8%/44.8% to 71.7%/47.2%, nonexact. The special stream-out path and
full stage-0/1 runtime remain unverified. Full ARM comparison covers
4,323/4,524 originals, 93.83%/82.99% means, 2,528 linked-exact and
1,076 byte-exact with no exact-pair losses. See
[the entry-position evidence](LEVEL_ENTRY_POSITION_ARM_2026-10-17.md).

## Level init global-source gate (2026-10-17)

Android `Level::setInitStreamOut()` is now linked-exact and the focused
`Level::init()` source-shape/strict fuzzy rises from 64.3%/35.4% to
69.8%/44.8%. The extracted-body `-O0/-O2` host smoke checks the flag;
native `gof2` and 40 verifier tests pass (one skip). Neither the smoke nor
the focused comparison proves a full initialization runtime path. Full ARM
coverage remains 4,323/4,524 originals, with 93.83%/82.99% means,
2,528 linked-exact and 1,076 byte-exact and no exact-pair losses. See
[the source evidence](LEVEL_INIT_GLOBAL_SOURCES_ARM_2026-10-17.md).

## Level entry constructor/canvas gate (2026-10-17)

Android ARM confirms three `Matrix` member-constructor calls and the
`Globals::Canvas` source in `Level::init`. The `Level` constructor now passes
the extracted-production-body host smoke at `-O0/-O2`; native `gof2` and
40 verifier tests pass (one existing skip). Focused ARM source-shape rises
from 32.6% to 95.0% for C1/C2 and 63.0% to 64.3% for init, with all three
still nonexact. See
[the focused evidence](LEVEL_ENTRY_CTOR_CANVAS_ARM_2026-10-17.md). The full
ARM report compiled 208/208 TUs, compared 4,323/4,524 originals and returned
93.81%/82.97% mean source-shape/strict fuzzy, 2,527 linked-exact and 1,076
byte-exact, with zero exact-pair losses against the preceding full report.

## Agent stock and mission host gate (2026-10-17)

The catalog-backed `-O0/-O2` smoke runs production `createAgents`
and `createMission` inside `Status::departStation` for station 41,
plus retained special sellers at station 108 and a 31-seed offer run.
Both primary and supplementary-name modes pass with 179/179 and
551/551 balanced file lifetimes respectively. RNG is seeded at the
agent boundary because the preceding item-stock generator resets it
from wall-clock time. Native `gof2` and 40 verifier tests pass (one
existing skip). Only host tests changed; the last full ARM report
remains 208/208 TUs, 4,323/4,524 originals, 93.78%/82.95%
averages and 2,527/1,076 linked-/byte-exact pairs. See
[the agent-stock evidence](GENERATOR_AGENT_STOCK_MISSIONS_ASSET_HOST_2026-10-17.md).

## Galaxy system distance ARM/runtime correction (2026-10-17)

The ARM body at `0x175f80` confirms the `Globals::globals` load and
18.85 VFP scale. Catalog-backed host `-O0/-O2` checks same- and
different-system distances; native `gof2` and 40 verifier tests pass
(one existing skip). Focused source-shape/strict fuzzy rises from
96.8%/70.9% to 97.2%/71.7%, nonexact. Full ARM report
`A:/Galaxy on Fire new version/_work/galaxy_distance_full_20261017.json`
has 208/208 compiled TUs, 4,323/4,524 originals, 4,730 rows,
93.78%/82.95% averages, 2,527 linked-exact and 1,076 byte-exact.
There are zero skipped/wrong-type units and zero exact-pair losses
against the previous full report. See
[the distance evidence](GALAXY_DISTANCE_ARM_RUNTIME_2026-10-17.md).

## Real agent-creation host gate (2026-10-17)

The real-asset `-O0/-O2` host smoke now executes production
`Generator::createAgent`, `Globals::getRandomName`, name file probing
and `ImageFactory::createChar(bool, int)` for 64 generated agents.
Both runs cover item-sale and wingman offers. The primary asset subset
lacks names and preserves 68/68 balanced file lifetimes. An optional
overlay of 13 extracted Java-reference name binaries also passes with
nonempty names; exact Android provenance remains unconfirmed.
`createAgents` and mission creation remain test boundaries. Native
`gof2` and 40 verifier tests pass (one existing skip). With no
production source change, the last full ARM result remains
208/208 TUs, 4,323/4,524 originals, 93.78%/82.95% averages and
2,527/1,076 linked-/byte-exact. See
[the agent-creation evidence](GENERATOR_AGENT_CREATION_ASSET_HOST_2026-10-17.md).

## Real ship-stock host gate (2026-10-17)

The real-asset `-O0/-O2` smoke now executes production
`Generator::getShipBuyList`, `Globals::getRandomEnemyFighter` and
`Ship::adjustPrice` during `Status::departStation` to catalog station
41. It checks the forced first ship, race, IDs, price and owned clones;
**68/68** file lifetimes balance. Native `gof2` and 40 verifier tests
pass (one existing skip). No production source changed, so the last
full ARM report remains
`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`:
**208/208** TUs, **4,323/4,524** originals, **93.78%/82.95%** mean
source-shape/strict fuzzy, **2,527** linked-exact and **1,076**
byte-exact, with no pair losses. See
[the ship-stock evidence](GENERATOR_SHIP_STOCK_ASSET_HOST_2026-10-17.md).

## Generic market host gate (2026-10-17)

At the preceding checkpoint, the asset-backed `-O0`/`-O2` smoke called
the production generic `Generator::getItemBuyList` body with real
`AERandom` and checked catalog-backed invariants. It then executed production
`Status::departStation` for catalog station 91; **68/68** file
lifetimes balance. Native
`gof2` and 40 verifier tests pass (one existing skip). No production
source changed, so the latest full ARM report remains
`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`:
**208/208** TUs, **4,323/4,524** originals, **93.78%/82.95%** mean
source-shape/strict fuzzy, **2,527** linked-exact and **1,076**
byte-exact, with zero pair losses. See
[the host evidence](GENERATOR_GENERIC_MARKET_ASSET_HOST_2026-10-17.md).

## Opening stock and Galaxy distance (2026-10-17)

The real-asset host smoke now executes production
`Generator::getItemBuyList` for early campaign station 78 and checks
items 0/22/55, amount 1 and price 0. It also executes production
`Galaxy::distancePercent` and `invDistancePercent` while
`Globals::gGlobals` is null, confirming the corrected
`Globals::globals` path. `-O0`/`-O2` and **66/66** file lifetimes pass;
native `gof2` builds and 40 verifier tests pass (one existing skip).
The focused Galaxy report at
`A:/Galaxy on Fire new version/_work/galaxy_distance_after_20261017.json`
keeps both symbols 100% source-shape/strict fuzzy and linked-exact.
The full report at
`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`
has **208/208** ARM TUs, **4,323/4,524** originals in **4,730** rows,
**93.78%/82.95%** average source-shape/strict fuzzy, **2,527**
linked-exact and **1,076** byte-exact. Pairwise comparison with
`status_move_wanted_full_20261017.json` finds zero exact-pair changes;
201 symbols remain absent, with zero wrong-type and skipped units. See
[the evidence](GENERATOR_OPENING_STOCK_GALAXY_DISTANCE_ARM_HOST_2026-10-17.md).

## Real pathfinder wanted route (2026-10-17)

The asset-backed host smoke links production `SystemPathFinder` and
`Node`. `-O0/-O2` exercise a direct system-20 route and a route with
an intermediate system through `Status::moveWanted`, with **66/66**
balanced file lifetimes. Random station choice remains a deterministic
test boundary. Native `gof2` and 40 verifier tests (one existing skip)
pass. The focused four-symbol pathfinder report at
`A:/Galaxy on Fire new version/_work/system_pathfinder_focused_20261017.json`
averages **97.0% / 81.0%** source-shape/strict fuzzy and has one
linked-exact/byte-exact comparison. No production source changed, so
the last full corpus remains **208/208** ARM TUs,
**4,323/4,524** originals, **93.78% / 82.95%** averages and
**2,527 linked-exact / 1,076 byte-exact** with no exact-pair changes.
See [the evidence](SYSTEM_PATHFINDER_WANTED_ASSET_RUNTIME_2026-10-17.md).

## Saved-route wanted movement (2026-10-17)

The asset-backed host smoke now executes production `Status::moveWanted`
at `-O0/-O2`, checking the programmed-station guard, a same-system
saved-ID move and a controlled three-element reroute. It balances
**63/63** file lifetimes; real pathfinding and random choice remain
test boundaries. Native `gof2`, **208/208** ARM TUs and 40 verifier
tests (one existing skip) pass. Focused `moveWanted` source-shape is
**86.2%** (from 78.9%), strict fuzzy **33.7%** (from 34.9%), nonexact.
The full report at
`A:/Galaxy on Fire new version/_work/status_move_wanted_full_20261017.json`
compares **4,323/4,524** originals in **4,730** rows, averaging
**93.78%/82.95%**, with **2,527 linked-exact / 1,076 byte-exact**,
201 absent, zero wrong-type or skipped units and no exact-pair changes.
See [the evidence](STATUS_MOVE_WANTED_SAVED_ROUTE_ARM_2026-10-17.md).

## Station departure gate (2026-10-17)

The asset-backed host smoke now extracts the production
`Status::departStation` and `isOnStack` bodies. `-O0/-O2` verify the
stored station identity, mission type mask and stop, reroute counter,
ship refresh and **56/56** file lifetimes; Generator is a test-only
empty-market boundary. Native `gof2`/`gof2_host_file`, **208/208** ARM
units and 40 verifier tests (one existing skip) pass. Focused ARM:
`isOnStack` **100%/100% linked-exact**, `departStation` **84.6%/20.4%**
source-shape/strict fuzzy and nonexact. The full report at
`A:/Galaxy on Fire new version/_work/status_depart_full_20261017.json`
compares **4,323/4,524** originals in **4,730** rows, averaging
**93.78%/82.95%**, with **2,527 linked-exact / 1,076 byte-exact**,
201 absent, zero wrong-type or skipped units and zero exact-pair losses.
See [the gate evidence](STATUS_DEPART_STATION_ARM_HOST_2026-10-17.md).

## Asset-backed new-game startup (2026-10-17)

The file-stack host smoke now extracts the production `Status::resetGame`
and related catalog, Mission, BluePrint, Ship and Galaxy bodies. With
seven mounted Android catalogs, `-O0/-O2` tests pass **56/56** file
opens/closes and assert station 78, ship 10, campaign/empty mission slots,
27 agents, 25 wanted records, all catalog-derived blueprints, starting
equipment and jump-state reset. Native `gof2` / `gof2_host_file`, all
**208/208** ARM units and 40 verifier tests pass (one existing skip).
Focused `resetGame` source-shape/strict fuzzy rises
**32.9%/15.4% → 88.1%/34.7%**. The full report is
`A:/Galaxy on Fire new version/_work/status_reset_blueprint_full_final_20261017.json`:
**4,323/4,524** originals, **4,730** rows, **93.78%/82.94%** average
source-shape/strict fuzzy, **2,526 linked-exact / 1,076 byte-exact**.
Compared by `(unit, symbol)` to the preceding full report, there are
zero exact losses and five linked-exact gains, with zero wrong-type
missing and zero skipped units. See
[the startup evidence](STATUS_RESET_GAME_ASSET_RUNTIME_ARM_2026-10-17.md).

## Galaxy station lookup lifetime (2026-10-17)

`Galaxy::getStation` now follows the Android ARM lifetime and negative-ID
global. Asset-backed host `-O0/-O2` tests exercise both paths with
**48/48** balanced file opens/closes. Native `gof2` and `gof2_host_file`
build; **208/208** ARM units compile and 40 verifier tests pass (one
existing skip). The focused source-shape/strict fuzzy scores move from
**72.0%/29.4%** to **100.0%/96.7%**. The full report at
`A:/Galaxy on Fire new version/_work/galaxy_getstation_full_20261017.json`
compares **4,323/4,524** originals in **4,723** rows; averages are
**93.76%/82.92%**, with **2,521 linked-exact / 1,076 byte-exact**.
Comparison against the preceding full report finds zero linked- or
byte-exact pair losses, zero skipped units and zero wrong-type missing
symbols. See [the lifetime evidence](GALAXY_GET_STATION_LIFETIME_ARM_2026-10-17.md).

## Asset-backed Status station transition (2026-10-17)

The `tools/run_weapon_positions_file_stack_smoke.py` host test now
executes production `Galaxy::Galaxy` and `Status::Status` /
`Status::setStation` over stations 91 and 58, including K'ontrr's
unsorted input order. Host `-O0/-O2` tests and **47/47** file
lifetimes pass. Native `gof2` and `gof2_host_file` build and 40
verifier tests pass (one existing skip). No production source changed;
the latest ARM corpus remains
`A:/Galaxy on Fire new version/_work/station_lifetime_full_final_20261017.json`.
See [the transition evidence](STATUS_SET_STATION_ASSET_RUNTIME_2026-10-17.md).

## Status startup catalogs (2026-10-17)

The host `tools/run_weapon_positions_file_stack_smoke.py` test now
includes `agents.bin` and `wanted.bin`. At `-O0/-O2`, all 27 agents
and 25 wanted entries parse, cross-catalog bounds and agent station
membership pass, and 44/44 file lifetimes balance. Native `gof2`
and 40 verifier tests (one existing skip) pass. No production source
changed, so the last full ARM corpus remains
`A:/Galaxy on Fire new version/_work/station_lifetime_full_final_20261017.json`.
See [the evidence note](STATUS_STARTUP_CATALOG_ASSETS_2026-10-17.md).

## System station order (2026-10-17)

The five-file host test now invokes the production
`loadStationsBinary(SolarSystem*)` body for all **34** systems at
`-O0/-O2`. Returned station IDs follow `stations.bin` scan order;
**42/42** file lifetimes balance. ARM REA inspection of
`_ZN8FileRead18loadStationsBinaryEP11SolarSystem` at `0x12014c`
confirms its increasing 0..134 scan and append index. System 20's
catalog order is nonascending. No production code changed; the last
full corpus is unchanged, and this overloaded loader remains
**93.7% source-shape / 56.2% strict fuzzy**, nonexact. See
[the evidence note](FILE_READ_SYSTEM_STATION_ORDER_ARM_2026-10-17.md).

## Systems catalog asset cross-check (2026-10-17)

`tools/run_weapon_positions_file_stack_smoke.py` now includes the
unchanged `loadSystemsBinary` and `SolarSystem` bodies. Against the
local Android 2.0.16 `systems.bin`, host `-O0/-O2` tests pass for all
34 systems and the mapping of all 135 stations to their systems.
Missing file and 42/42 stream lifetime checks pass. The native `gof2`
target and 40 verifier tests (one existing skip) pass. No production
source changed, so the latest ARM corpus is still
`A:/Galaxy on Fire new version/_work/station_lifetime_full_final_20261017.json`;
in it `loadSystemsBinary` is 97.1% source-shape, 63.4% strict fuzzy,
not exact. See [the systems evidence](SYSTEMS_CATALOG_ASSET_CROSSCHECK_2026-10-17.md).

## Native host asset bridge (2026-10-17)

`python tools/run_weapon_positions_file_stack_smoke.py --asset-dir
"A:/Galaxy on Fire new version/analysis/gof2_raw_bin" --out
"A:/Galaxy on Fire new version/_work/weapon-file-stack-smoke" --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at `-O0/-O2` with the new
`host/ReadOnlyAssetFileInterface` and unchanged production loaders.
Missing-asset and path checks pass; four real resource files produce
seven balanced opens/closes. The native `gof2` and `gof2_host_file`
targets build. No Android source or ARM comparison target changed;
the most recent full report remains
`A:/Galaxy on Fire new version/_work/station_lifetime_full_final_20261017.json`.
See [the host stage note](HOST_ASSET_BRIDGE_AND_STAGES_2026-10-17.md).

## Station lookup ownership recovery (2026-10-17)

Focused ARM `loadStation` rises **57.5%/14.3% → 100%/100%**
source-shape/strict fuzzy and becomes linked-exact, not byte-exact;
`Array<Station *>::~Array` remains linked-exact. The host file-stack
smoke passes `stations.bin` at `-O0/-O2`, including all 135 station
IDs and direct reads of DLC starting stations 70/91. Native `gof2`,
**208/208** ARM units and 40 verifier tests (one existing skip) pass.
Full report
`A:/Galaxy on Fire new version/_work/station_lifetime_full_final_20261017.json`
records **4,323/4,524** originals, **4,723** comparisons,
**93.76%/82.91%** averages, **2,521/1,076** linked-/byte-exact,
zero exact-pair losses, and one extra destructor alias. See
[the evidence note](FILE_READ_LOAD_STATION_OWNERSHIP_ARM_2026-10-17.md).

## Core catalog asset check (2026-10-17)

`tools/run_weapon_positions_file_stack_smoke.py` now exercises the
unchanged `items.bin` and `ships.bin` loader and constructor bodies
alongside the weapon-position asset at host `-O0/-O2`. All 233 item
and 64 ship IDs pass; missing-file and open/close paths pass. Native
`gof2`, **208/208** ARM units and 40 verifier tests (one existing
skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/core_catalog_file_stack_focus_20261017.json`
records **95.9%/59.9%** source-shape/strict fuzzy for items and
**78.4%/61.0%** for ships. No production code changed, so full-corpus
counts remain those in the DLC section below. See
[the asset evidence](CORE_CATALOG_ASSET_FILE_STACK_2026-10-17.md).

## Weapon-position native file-stack check (2026-10-17)

`tools/run_weapon_positions_file_stack_smoke.py` compiles the unchanged
production AEFile stack and extracted weapon loader with a host file
boundary. The local asset passes missing/present, first/later record,
coordinates and open/close checks at `-O0/-O2`. Incremental native
`gof2` passes. Focused report
`A:/Galaxy on Fire new version/_work/weapon_file_stack_focus_20261017.json`
compiles **208/208** ARM units and retains **97.7%/53.1%**
source-shape/strict fuzzy, nonexact. No production source changed;
full-corpus counts remain those in the DLC preset section below.
See [the evidence note](WEAPON_FILE_STACK_ASSET_MOUNT_2026-10-17.md).

## DLC start presets (2026-10-17)

`python tools/test_menutouch_dlc_presets.py` passes two source-mapping
tests. Native `gof2` links; focused ARM builds **208/208** units.
`startValkyrie` source-shape/strict fuzzy is **97.5%/70.8%** (from
89.1%/53.7%) and `startSupernova` is **96.4%/64.7%** (from
91.4%/45.8%); both remain nonexact. Full corpus report
`A:/Galaxy on Fire new version/_work/mtw_dlc_preset_full_20261017.json`
records **4,323/4,524** originals, **4,723** comparisons,
**93.75%/82.89%** averages and **2,520/1,076** linked-/byte-exact,
with zero exact-pair losses against the preceding full report.
The 40 verifier tests pass with one existing skip. See
[the evidence note](MENUTOUCH_DLC_PRESETS_ARM_2026-10-17.md).

## Weapon-position asset-backed loader check (2026-10-17)

`python tools/run_weapon_positions_asset_smoke.py --asset
"A:/Galaxy on Fire new version/analysis/gof2_raw_bin/weapons_hd.bin"
--cxx C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2`.
The test extracts the production loader body but uses a host file
shim; it checks absent-file return, records for ships 63 and 1,
slot counts, selected coordinates, string lifetime and close calls.
The incremental native `gof2` build passes. Focused ARM report
`A:/Galaxy on Fire new version/_work/weapon_positions_asset_smoke_focus_20261017.json`
compiles **208/208** units and retains **97.7% source-shape / 53.1%
strict fuzzy**, still nonexact. No C++ source changed; the latest
completed full-corpus metrics remain those in the Level cleanup
section below.
See [the asset check](WEAPON_POSITIONS_ASSET_SMOKE_2026-10-17.md).

## Level weapon-position cleanup (2026-10-17)

`python tools/run_level_weapon_cleanup_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for null,
populated and empty slots. Native `gof2`, **208/208** ARM units and
40 verifier tests (one existing skip) pass. Focused
`Level::createPlayer` rises from **86.2% source-shape / 37.2% strict
fuzzy** in `level_weapon_cleanup_before_20261017.json` to
**86.5% / 37.8%** in `level_weapon_cleanup_after_20261017.json`,
still nonexact. Full report
`A:/Galaxy on Fire new version/_work/level_weapon_cleanup_full_20261017.json`
compares **4,323/4,524** originals over **4,723** comparisons, with
**201 missing**, **123 extras**, **93.74%/82.88%** rounded averages,
**2,520 linked-exact / 1,076 byte-exact**. Against
`filread_weapon_positions_lifetime_full_20261017.json`, there are
zero exact-pair losses and two linked-exact release-helper gains.
See [the cleanup audit](LEVEL_WEAPON_POSITION_CLEANUP_ARM_2026-10-17.md).

## FileRead weapon path lifetime (2026-10-17)

Native `gof2`, **208/208** ARM units and 40 verifier tests (one
existing skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/filread_weapon_positions_lifetime_after_20261017.json`
scores `loadWeaponPositions` at **97.7% source-shape / 53.1% strict
fuzzy**, versus **83.1% / 36.0%** in
`filread_weapon_positions_baseline_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/filread_weapon_positions_lifetime_full_20261017.json`
has **4,323/4,524** originals over **4,721** comparisons, **201
missing**, **123 extras**, **93.74%/82.88%** rounded averages and
**2,518 linked-exact / 1,076 byte-exact**. Exact `(unit,symbol)`
sets are unchanged from `cutscene_filread_lifetime_full_20261017.json`.
See [the path-lifetime audit](FILE_READ_WEAPON_PATH_LIFETIME_ARM_2026-10-17.md).

## CutScene FileRead lifetime (2026-10-17)

`python tools/run_cutscene_filread_lifetime_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for
constructor, ship lookup, load and destructor ordering. Native
`gof2`, **208/208** ARM units, 40 verifier tests (one existing skip)
and preceding turret smokes pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_filread_lifetime_after_20261017.json`
scores `checkForTurret` at **96.0% source-shape / 65.0% strict fuzzy**,
up from **93.3% / 48.5%** in
`cutscene_turret_cleanup_after_20261017.json`, still nonexact. Full
report
`A:/Galaxy on Fire new version/_work/cutscene_filread_lifetime_full_20261017.json`
has **4,323/4,524** original functions over **4,721 comparisons**,
**201 missing**, **123 extras**, **93.74%/82.87%** rounded averages,
**2,518 linked-exact / 1,076 byte-exact**. Exact `(unit,symbol)`
sets are unchanged from `cutscene_turret_cleanup_full_20261017.json`.
See [the lifetime audit](CUTSCENE_FILREAD_LIFETIME_ARM_2026-10-17.md).

## CutScene turret position cleanup (2026-10-17)

`python tools/run_cutscene_turret_cleanup_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for two
populated and two empty position slots, plus the null-list return.
Native `gof2`, **208/208** ARM units, 40 verifier tests (one existing
skip), and previous turret motion/transform smokes pass. Focused
report
`A:/Galaxy on Fire new version/_work/cutscene_turret_cleanup_after_20261017.json`
scores `checkForTurret` at **93.3% source-shape / 48.5% strict fuzzy**,
versus **93.8% / 48.7%** in
`cutscene_check_turret_after_20261017.json`; the ARM cleanup call
takes precedence over this nonexact function's score. Full report
`A:/Galaxy on Fire new version/_work/cutscene_turret_cleanup_full_20261017.json`
has **4,323/4,524** originals compared in **4,721 comparisons**,
**201 missing**, **123 extras**, **93.74%/82.87%** rounded averages,
**2,518 linked-exact / 1,076 byte-exact**. Relative to
`cutscene_init_mode2_placement_full_20261017.json`, no exact
`(unit,symbol)` pair is lost and the outer `ArrayReleaseClasses`
specialization gains linked-exact. See
[the cleanup audit](CUTSCENE_TURRET_CLEANUP_ARM_2026-10-17.md).

## CutScene mode-2 ship placement (2026-10-17)

`python tools/run_cutscene_init_mode2_ships_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for both
positions, the positive-time gate, null and short enemy lists and
missing players. The prior mode-2 and mode-23 camera smokes, native
`gof2`, **208/208** ARM units and 40 verifier tests (one existing
skip) also pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode2_placement_after_20261017.json`
scores `initialize` at **95.8% source-shape / 61.9% strict fuzzy**,
up from **93.1% / 37.9%** in
`cutscene_init_mode2_placement_before_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode2_placement_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.74%/82.87%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_check_turret_full_20261017.json`. See
[the placement audit](CUTSCENE_INIT_MODE2_SHIPS_ARM_2026-10-17.md).

## CutScene turret geometry (2026-10-17)

`python tools/run_cutscene_turret_transform_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` and `python
tools/run_cutscene_turret_motion_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` pass at host `-O0/-O2` for seven
transform-ID calls, all known item-index translations, outer position
slot 2 and the rotation gate. Native `gof2`, **208/208** ARM units and
40 verifier tests (one existing skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_check_turret_after_20261017.json`
scores `checkForTurret` at **93.8% source-shape / 48.7% strict fuzzy**,
up from **89.6% / 45.8%** in
`cutscene_check_turret_before_20261017.json`, still nonexact. Full
report
`A:/Galaxy on Fire new version/_work/cutscene_check_turret_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.74%/82.86%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_replace_ship_final_full_20261017.json`. See
[the turret audit](CUTSCENE_TURRET_ARM_2026-10-17.md).

## CutScene ship replacement (2026-10-17)

`python tools/run_cutscene_replace_ship_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for both
`getShipGroup` arguments, offsets at indices 19 and 63, matrix copy,
main transform IDs, exhaust hiding, LOD removal, and null-list and
null-old-geometry paths. Native `gof2`, **208/208** ARM units, and
40 verifier tests (one existing skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_replace_ship_final_after_20261017.json`
scores `replacePlayerShip` at **98.0% source-shape / 89.5% strict
fuzzy**, up from **94.7% / 64.9%** in
`cutscene_replace_ship_before_20261017.json`, still nonexact. Full
report
`A:/Galaxy on Fire new version/_work/cutscene_replace_ship_final_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.86%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode2_ship_final_full_20261017.json`. See
[the replacement audit](CUTSCENE_REPLACE_SHIP_ARM_2026-10-17.md).

## CutScene mode-2 ship motion (2026-10-17)

`python tools/run_cutscene_process_mode2_ship_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for both
ships' three-axis motion, zero/negative playing time (no enemy-list
access), missing first geometry and a high-bit frame delta. Native
`gof2`, **208/208** ARM units and 40 verifier tests (one existing
skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode2_ship_after_20261017.json`
scores `process` at **94.0% source-shape / 63.5% strict fuzzy**,
up from **93.9% / 62.8%** in
`cutscene_process_mode4_race2_after_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode2_ship_final_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode4_race2_full_20261017.json`. See
[the mode-2 audit](CUTSCENE_PROCESS_MODE2_SHIP_ARM_2026-10-17.md).

## CutScene mode-4 race-2 process path (2026-10-17)

`python tools/run_cutscene_process_mode4_race2_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for the two
last enemies, four list reads, ordinary/high-bit frame deltas and
the non-race-2 return. Native `gof2`, **208/208** ARM units, 40
verifier tests (one existing skip), and preceding mode-4 smokes pass.
Focused report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race2_after_20261017.json`
scores `process` at **93.9% source-shape / 62.8% strict fuzzy**,
up from **92.1% / 59.7%** in
`cutscene_process_mode4_race3_after_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race2_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode4_race3_full_20261017.json`. See
[the race-2 audit](CUTSCENE_PROCESS_MODE4_RACE2_ARM_2026-10-17.md).

## CutScene mode-4 race-3 process path (2026-10-17)

`python tools/run_cutscene_process_mode4_race3_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for both
random thresholds, optional `geom28`, both timers and a high-bit
frame delta. Native `gof2`, **208/208** ARM units, 40 verifier tests
(one existing skip), and preceding mode-4 smokes pass. Focused
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race3_after_20261017.json`
scores `process` at **92.1% source-shape / 59.7% strict fuzzy**,
up from **90.7% / 56.1%** in
`cutscene_process_mode4_race1_after_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race3_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode4_race1_full_20261017.json`. See
[the race-3 audit](CUTSCENE_PROCESS_MODE4_RACE3_ARM_2026-10-17.md).

## CutScene mode-4 race-1 process path (2026-10-17)

`python tools/run_cutscene_process_mode4_race1_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for optional
geometry presence, both animation timers and a high-bit frame delta.
Native `gof2`, **208/208** ARM units, 40 verifier tests (one existing
skip), and the preceding mode-4 and mode-23 smokes pass. Focused
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race1_after_20261017.json`
scores `process` at **90.7% source-shape / 56.1% strict fuzzy**,
up from **90.2% / 53.2%** in
`cutscene_process_mode4_common_after_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode4_race1_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode4_common_full_20261017.json`. See
[the race-1 audit](CUTSCENE_PROCESS_MODE4_RACE1_ARM_2026-10-17.md).

## CutScene mode-4 common process transforms (2026-10-17)

`python tools/run_cutscene_process_mode4_common_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` for a null
middle enemy and ordinary/high-bit frame deltas. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), and the
preceding mode-23 process smokes pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode4_common_after_20261017.json`
scores `process` at **90.2% source-shape / 53.2% strict fuzzy**,
up from **90.1% / 50.8%** in
`cutscene_process_mode23_anim_after_20261017.json`, still nonexact.
Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode4_common_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode23_anim_full_20261017.json`. See
[the mode-4 common audit](CUTSCENE_PROCESS_MODE4_COMMON_ARM_2026-10-17.md).

## CutScene mode-23 process animation (2026-10-17)

`python tools/run_cutscene_process_mode23_animation_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2`, including
timer, random, running-state and high-bit frame-delta cases. Native
`gof2`, **208/208** ARM units, 40 verifier tests (one existing skip),
and the preceding mode-23 process smokes pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode23_anim_after_20261017.json`
scores `process` at **90.1% source-shape / 50.8% strict fuzzy**,
up from **89.7% / 49.3%** in
`cutscene_process_fog_after_20261017.json`, still nonexact. Full
report `A:/Galaxy on Fire new version/_work/cutscene_process_mode23_anim_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_fog_full_20261017.json`. See
[the animation audit](CUTSCENE_PROCESS_MODE23_ANIMATION_ARM_2026-10-17.md).

## CutScene process fog (2026-10-17)

`python tools/run_cutscene_process_fog_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` on extracted
production fog-call fragments. Native `gof2`, **208/208** ARM units,
40 verifier tests (one existing skip), and the earlier mode-23
transform and reset-camera smokes pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_process_fog_after_20261017.json`
scores `process` at **89.7% source-shape / 49.3% strict fuzzy**,
up from **89.1% / 47.8%** in
`cutscene_process_mode23_after_20261017.json`, still nonexact. Full
report `A:/Galaxy on Fire new version/_work/cutscene_process_fog_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_process_mode23_full_20261017.json`. See
[the process-fog audit](CUTSCENE_PROCESS_FOG_ARM_2026-10-17.md).

## CutScene mode-23 process transform (2026-10-17)

`python tools/run_cutscene_process_mode23_transform_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` on extracted
production statements, including a high-bit frame delta. Native
`gof2`, **208/208** ARM units, 40 verifier tests (one existing skip),
and earlier CutScene setup smokes pass. Focused reports
`A:/Galaxy on Fire new version/_work/cutscene_process_mode23_before_20261017.json`
and
`A:/Galaxy on Fire new version/_work/cutscene_process_mode23_after_20261017.json`
score `process` at **88.3%/43.7%** before and **89.1%/47.8%** after
for source-shape/strict fuzzy, still nonexact. Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_mode23_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_reset_camera_full_20261017.json`. See
[the transform audit](CUTSCENE_PROCESS_MODE23_TRANSFORM_ARM_2026-10-17.md).

## CutScene resetCamera (2026-10-17)

`python tools/run_cutscene_reset_camera_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` on the
extracted production body for modes 23, 4 and 2 with races 1 and 0.
Native `gof2`, **208/208** ARM units, 40 verifier tests (one existing
skip), and the earlier CutScene setup smokes pass. Focused reports
`A:/Galaxy on Fire new version/_work/cutscene_reset_camera_before_20261017.json`
and
`A:/Galaxy on Fire new version/_work/cutscene_reset_camera_after_20261017.json`
score `resetCamera` at **77.9%/34.3%** before and **91.6%/57.3%**
after for source-shape/strict fuzzy, still nonexact. Full report
`A:/Galaxy on Fire new version/_work/cutscene_reset_camera_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_init_mode4_full_20261017.json`. See
[the reset-camera audit](CUTSCENE_RESET_CAMERA_ARM_2026-10-17.md).

## CutScene mode-4 setup (2026-10-17)

`python tools/run_cutscene_mode4_setup_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2`, exercising
races 3, 0 and 1 with the extracted production setup. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), and the
earlier CutScene camera/time smokes pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode4_focused_20261017.json`
scores `initialize` at **93.1% source-shape / 37.9% strict fuzzy**,
up from **92.3% / 37.0%**, nonexact. Full report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode4_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_init_mode23_full_20261017.json`. See
[the mode-4 setup audit](CUTSCENE_INIT_MODE4_SETUP_ARM_2026-10-17.md).

## CutScene mode-23 camera initialization (2026-10-17)

`python tools/run_cutscene_mode23_camera_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` on the extracted
production branch. Native `gof2`, **208/208** ARM units, 40 verifier
tests (one existing skip), and the prior CutScene camera/time smokes
pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode23_focused_20261017.json`
scores `initialize` at **92.3% source-shape / 37.0% strict fuzzy**,
up from **88.9% / 36.2%**, nonexact. Full report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode23_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact / 1,076
byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_init_mode2_full_20261017.json`. See
[the mode-23 initialization audit](CUTSCENE_INIT_MODE23_CAMERA_ARM_2026-10-17.md).

## CutScene mode-2 camera initialization (2026-10-17)

`python tools/run_cutscene_init_mode2_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2` on the extracted
production setup calls. Native `gof2`, **208/208** ARM units, 40
verifier tests (one existing skip), and the prior CutScene camera/time
smokes pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode2_focused_20261017.json`
scores `initialize` at **88.9% source-shape / 36.2% strict fuzzy**,
up from **86.9% / 35.7%**, nonexact. Full report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode2_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact /
1,076 byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_mode2_camera_full_20261017.json`. See
[the initialization audit](CUTSCENE_INIT_MODE2_CAMERA_ARM_2026-10-17.md).

## CutScene mode-2 camera matrix (2026-10-17)

`python tools/run_cutscene_mode2_camera_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2`, exercising
the extracted production matrix copy/rotate/submit route. Native
`gof2`, **208/208** ARM units and 40 verifier tests (one existing skip)
also pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_mode2_camera_focused_20261017.json`
scores `process` at **88.3% source-shape / 43.7% strict fuzzy**,
up from **87.6% / 43.6%**. Full report
`A:/Galaxy on Fire new version/_work/cutscene_mode2_camera_full_20261017.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** rounded averages, **2,517 linked-exact /
1,076 byte-exact**. Exact `(unit,symbol)` sets are unchanged from
`cutscene_camera_step_full_20261016.json`. See
[the matrix audit](CUTSCENE_MODE2_CAMERA_MATRIX_ARM_2026-10-17.md).

## CutScene fixed camera step (2026-10-17)

Android ARM at `0x98eec` passes `30` to
`TargetFollowCamera::update(int)`; the recovered caller now agrees.
Focused report
`A:/Galaxy on Fire new version/_work/cutscene_camera_step_focused_20261016.json`
scores `CutScene::process` at **87.6% source-shape / 43.6% strict
fuzzy**, up from **87.5% / 42.1%**, nonexact. Native `gof2`, **208/208**
ARM units, 40 verifier tests (one existing skip), and
`run_cutscene_time_smoke.py` pass. Full report
`A:/Galaxy on Fire new version/_work/cutscene_camera_step_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.85%** averages, **2,517 linked-exact / 1,076 byte-exact**.
Both exact `(unit,symbol)` sets are unchanged from
`cutscene_time64_full_20261016.json`. See
[the fixed-step audit](CUTSCENE_CAMERA_FIXED_STEP_ARM_2026-10-17.md).

## CutScene render and time layout (2026-10-16)

`python tools/run_cutscene_time_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2`, exercising
extracted production time fragments across high-word and wrap cases.
Native `gof2`, **208/208** ARM units and 40 verifier tests (one
existing skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_time64_focused_20261016.json`
shows `render3D` at **100% linked-exact**, `isInitialized` at **100%
byte-exact**, `process` at **87.5%/42.1%** and `initialize` at
**86.9%/35.7%** source-shape/strict fuzzy. Full report
`A:/Galaxy on Fire new version/_work/cutscene_time64_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.73%/82.84%** averages, **2,517 linked-exact / 1,076 byte-exact**.
Compared with `cutscene_update_full_20261016.json`, exact
`(unit,symbol)` sets gain two linked and one byte pair, zero losses.
See [the render/time audit](CUTSCENE_RENDER_TIME_LAYOUT_ARM_2026-10-16.md).

## CutScene update dispatch (2026-10-16)

The Android ARM `update()` and `update(int)` symbols both tail-branch
through a veneer to `CutScene::process(int)@plt`; the rebuilt wrappers
now use that route. Focused report
`A:/Galaxy on Fire new version/_work/cutscene_update_focused_20261016.json`
shows **100% source-shape / 100% strict fuzzy** for both one-instruction
wrappers, up from **1.7% / 0.0%** for `update()`. Neither is linked-
or byte-exact. Native `gof2`, **208/208** ARM units and 40 verifier
tests pass (one existing skip). Full report
`A:/Galaxy on Fire new version/_work/cutscene_update_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.72%/82.82%** average source-shape/strict fuzzy, **2,515
linked-exact / 1,075 byte-exact**. Both exact `(unit,symbol)` sets
are unchanged against `player_ego_setposition_full_20261016.json`.
See [the dispatch audit](CUTSCENE_UPDATE_DISPATCH_ARM_2026-10-16.md).

## PlayerEgo position overload (2026-10-16)

`PlayerEgo::setPosition(Vector)` now forwards through its native
three-float overload. Focused report
`A:/Galaxy on Fire new version/_work/player_ego_setposition_focused_20261016.json`
measures **100% source-shape / 100% strict fuzzy** at one instruction,
up from **1.1% / 0.0%**; it is not linked- or byte-exact. Native
`gof2`, **208/208** ARM units and 40 verifier tests (one existing
skip) pass. Full report
`A:/Galaxy on Fire new version/_work/player_ego_setposition_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.70%/82.80%** average source-shape/strict fuzzy, **2,515
linked-exact / 1,075 byte-exact**. Compared against
`particle_sprite_spawn_full_20261016.json`, both exact `(unit,symbol)`
sets have zero gains and zero losses. See
[the overload audit](PLAYER_EGO_POSITION_OVERLOAD_ARM_2026-10-16.md).

## Particle emitter argument ABI (2026-10-16)

`python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes at host `-O0/-O2`; `python
tools/run_particle_control_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`
passes at `-O0`. Their `init` fixture overrides now match the `void`
virtual interface. The tests cover distinct lifetime/start/end sizes,
random draws and UV order in extracted production callers. Native
`gof2` and 40 verifier tests pass (one existing skip). No production
C++ changed; prior **208/208** ARM build and full report
`A:/Galaxy on Fire new version/_work/particle_sprite_spawn_full_20261016.json`
still apply. See [the ARM call-site audit](PARTICLE_EMIT_ARGUMENT_ABI_ARM_2026-10-16.md).

## Sprite spawn size argument (2026-10-16)

`python tools/run_particle_sprite_alpha_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` extracts the production sprite
`setParticle` body. The size assertion fails before the patch with
`p2=23.75`, `p9=99.25` and passes afterward at `-O0/-O2`; position,
UV, color mask and 16-bit id calls also pass. Native `gof2`, 208/208
ARM units and 40 verifier tests (one existing skip) pass. Focused
reports `A:/Galaxy on Fire new version/_work/particle_sprite_spawn_before_20261016.json`
and `particle_sprite_spawn_after_20261016.json` show source-shape/
strict fuzzy **90.8%/43.5% → 89.1%/38.4%**, nonexact. The decline
does not justify reverting the ARM-confirmed behavior. See
[the spawn audit](PARTICLE_SPRITE_SPAWN_SIZE_ARM_2026-10-16.md).
Full report
`A:/Galaxy on Fire new version/_work/particle_sprite_spawn_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
zero skipped/wrong signatures, **93.68%/82.78%** average source-shape/
strict fuzzy, **2,515 linked-exact / 1,075 byte-exact**. Both exact
`(unit,symbol)` sets are unchanged against
`particle_init_void_full_20261016.json`.

## Sprite particle area-exit behavior (2026-10-16)

`python tools/run_particle_sprite_alpha_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now extracts the production
`updateAreaExitParticle` and `setAlpha` bodies. Host `-O0/-O2`
recording-canvas checks cover the outer, middle and inner radius
bands, exact outer/middle boundaries, four-draw rejected/accepted
rebirth, position
and base-velocity effects. The random source is scripted and the
Vector/Matrix and canvas helpers are fixtures; live ARM32/GPU and
production RNG remain untested. Native `gof2` and 40 verifier tests
(one existing skip) pass. No production C++ changed. Focused report
`A:/Galaxy on Fire new version/_work/particle_sprite_area_exit_focused_20261016.json`
stays **94.6% source-shape / 66.7% strict fuzzy**, nonexact. The
unchanged full report `particle_init_void_full_20261016.json` has
**4,323/4,524** compared, **93.68%/82.78%** averages, **2,515
linked-exact / 1,075 byte-exact**. ARM build was not rerun for this
test-only change; prior **208/208** remains applicable. See
[the area-exit audit](PARTICLE_SPRITE_AREA_EXIT_ARM_2026-10-16.md).

## Sprite particle update output (2026-10-16)

`python tools/run_particle_sprite_alpha_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now extracts the production sprite
`updateSingle` body and records exact/expired lifetime behavior,
canvas call order, size/movement values, UV transition, and area-exit
dispatch at host `-O0/-O2`. The interpolator, rotation and area-exit
callee are fixture stubs; their internals are not checked here.
The fixture, native `gof2` and 40 verifier tests (one existing skip)
pass. No production C++ changed. Focused report
`A:/Galaxy on Fire new version/_work/particle_sprite_update_output_focused_20261016.json`
remains **92.9% source-shape / 49.0% strict fuzzy**, nonexact. The
last full ARM report `particle_init_void_full_20261016.json` still
applies: **4,323/4,524** compared, **93.68%/82.78%** averages,
**2,515 linked-exact / 1,075 byte-exact**. ARM compilation was not
rerun for this test-only change; the prior 208/208 result remains
applicable. See [the update audit](PARTICLE_SPRITE_UPDATE_OUTPUT_ARM_2026-10-16.md).

## Particle init return ABI and sprite alpha output (2026-10-16)

Android sprite and mesh `init` both tail-call virtual `reset` without
forming an integer return; the source virtual interface and overrides
now use `void`. Native `gof2`, **208/208** ARM translation units, 40
verifier tests (one existing skip), the mesh trail fixture and the
new `python tools/run_particle_sprite_alpha_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` fixture at `-O0/-O2` pass. The latter
records sprite init/reset and RGBA output for both alpha modes and
16-bit index wrap. Focused ARM reports
`A:/Galaxy on Fire new version/_work/particle_sprite_init_alpha_after_20261016.json`
and `particle_mesh_init_after_20261016.json` show sprite init
**57.0%/62.5% → 100%/100%** source-shape/strict fuzzy, now byte-exact;
mesh init **97.6%/97.9% → 100%/100%**, now linked-exact. Sprite
`setAlpha` remains **80.6%/46.9%**, nonexact. Full report
`A:/Galaxy on Fire new version/_work/particle_init_void_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**, no
skips/wrong signatures, **93.68%/82.78%** average source-shape/strict
fuzzy, **2,515 linked-exact / 1,075 byte-exact**. Exact `(unit,symbol)`
set comparison against `particle_interpolation_full_20261016.json`
shows two linked gains, one byte gain and no losses. See
[the init/alpha audit](PARTICLE_INIT_VOID_SPRITE_ALPHA_ARM_2026-10-16.md).

## Base particle color interpolation (2026-10-16)

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now extracts the real
`IParticleSystem::interpolateColor` in addition to the mesh methods.
The `-O0/-O2` fixture checks a half-life blend, over-life cap, both
fade modes, second set and trail vertex propagation. Native `gof2`
and 40 verifier tests (one existing skip) pass. No production C++
changed. Fresh focused `--no-build` report
`A:/Galaxy on Fire new version/_work/particle_interpolation_baseline_20261016.json`
records **91.4% source-shape / 42.8% strict fuzzy**, nonexact. The
fresh full `--no-build` report
`A:/Galaxy on Fire new version/_work/particle_interpolation_full_20261016.json`
has **4,323/4,524** compared, **201 missing**, **123 extras**,
zero skipped, **93.67%/82.77%** averages, **2,513 linked-exact /
1,074 byte-exact**. Both exact `(unit,symbol)` sets have zero
differences against `particle_trail_edge_sign_full_20261016.json`.
ARM compilation was not rerun; the previous 208/208 result remains
valid because production C++ is unchanged. Live ARM32/GPU
behavior remains unverified. See
[the interpolation audit](PARTICLE_INTERPOLATE_COLOR_ARM_2026-10-16.md).

## ParticleSystemMesh color and expiry output (2026-10-16)

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` extracts the production `updateSingleColor`
and `updateSingle` bodies and checks recorded canvas calls at host
`-O0/-O2`: start masks, adjacent/wrapped sections, expiry order and
`age == lifetime` boundary. The interpolator is a fixture stub, so
its arithmetic and live GPU output are outside this check. The smoke
test, native `gof2` and 40 verifier tests (one existing skip) pass.
No production C++ changed; the prior 208/208 ARM build and full report
still apply. Fresh focused `--no-build` report
`A:/Galaxy on Fire new version/_work/particle_color_expiry_focused_20261016.json`
shows 97.2%/70.5% `updateSingle` and 99.8%/74.6%
`updateSingleColor` source-shape/strict fuzzy, both nonexact. The
unchanged full report has 4,323/4,524 compared, 93.67%/82.77%
averages, 2,513 linked-exact and 1,074 byte-exact. See
[the output audit](PARTICLE_MESH_COLOR_EXPIRY_OUTPUT_ARM_2026-10-16.md).

## ParticleSystemMesh trail-edge Y sign (2026-10-16)

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` extracts the production
`updateTrailEdges` body and records both negative/positive
`MeshTranslatePoint` pairs at host `-O0/-O2`. The test failed before
the two Y-sign corrections and passes afterward. Native `gof2`,
208/208 ARM units and 40 verifier tests (one existing skip) pass.
Focused source-shape/strict fuzzy changes 96.1%/67.2% to
96.8%/73.4%, nonexact. Full report
`A:/Galaxy on Fire new version/_work/particle_trail_edge_sign_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 123 extras, zero
skipped or wrong signatures, 93.67%/82.77% average source-shape/
strict fuzzy, 2,513 linked-exact and 1,074 byte-exact. Both exact
`(unit,symbol)` sets have zero losses/gains against
`particle_mesh_update_sentinel_full_20261016.json`. See
[the edge audit](PARTICLE_MESH_TRAIL_EDGE_SIGN_ARM_2026-10-16.md).

## ParticleSystemMesh dormant trail sentinel (2026-10-16)

The extracted `ParticleSystemMesh::updateSingle` test failed on the
original C++ for age `-2` with `newSectionStarted == 0`, then passed
after restoring the ARM early exit at `0x1868c8`. It also checks
resumed and ongoing trail particles at host `-O0/-O2`. Native `gof2`,
208/208 ARM units and 40 verifier tests (one existing skip) pass. Focused
source-shape/strict fuzzy changes 97.2%/69.2% to 97.2%/70.5%,
nonexact. Full report
`A:/Galaxy on Fire new version/_work/particle_mesh_update_sentinel_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 123 extras, no skipped
or wrong signatures, 93.67%/82.77% average source-shape/strict fuzzy,
2,513 linked-exact and 1,074 byte-exact. Both exact `(unit,symbol)`
sets have zero losses/gains against
`particle_emit_direct_split_full_20261016.json`. See
[the sentinel audit](PARTICLE_MESH_SENTINEL_UPDATE_ARM_2026-10-16.md).

## Base particle repeated-frame rate and ring check (2026-10-16)

`python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` runs the extracted production `emit`
body at host `-O0/-O2`. The new 100/200/100/200 ms sequence checks
rate-mode timer carry and three-slot wrap against original ARM at
`0x181f62`–`0x181f92` and `0x1826c4`–`0x1826cc`. Native `gof2`
and 40 verifier tests (one existing skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/particle_emit_repeated_frame_focused_20261016.json`
is 88.8% source-shape / 19.9% strict fuzzy, nonexact. Production C++
is unchanged; the preceding full ARM corpus report
`particle_emit_direct_split_full_20261016.json` still applies with
4,323/4,524 originals, 201 missing, 123 extras, 93.67%/82.77%
average source-shape/strict fuzzy, 2,513 linked-exact and 1,074
byte-exact. See [the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## ParticleSystemMesh direct trail dispatch (2026-10-16)

The extracted particle trail fixture passes; native `gof2`, 208/208
ARM units and 40 verifier tests (one existing skip) pass. Original ARM
`emit` branches to `emitTrail` or `IParticleSystem::emit`; the rebuilt
object now has matching `R_ARM_THM_JUMP24` targets. Focused `emit` stays
100% source-shape/strict fuzzy and linked-exact. The full report
`A:/Galaxy on Fire new version/_work/particle_emit_direct_split_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 123 extras, zero skipped
or wrong signatures, 93.67%/82.77% average source-shape/strict fuzzy,
2,513 linked-exact and 1,074 byte-exact. Against
`objectgun_update_offsets_full_20261016.json`, both exact
`(unit,symbol)` sets have zero losses or gains. See
[the dispatch audit](PARTICLE_MESH_DIRECT_TRAIL_DISPATCH_ARM_2026-10-16.md).

## ObjectGun geometry offsets (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks extracted owner and turret
offset expressions under host `-O0/-O2`, including all Z choices
and a high-byte-only flag. Native `gof2`, 207/207 ARM units and
40 verifier tests (one existing skip) pass. Focused `update`
source-shape/strict fuzzy changes 87.9%/31.0% -> 90.8%/30.4%,
nonexact. Full report
`A:/Galaxy on Fire new version/_work/objectgun_update_offsets_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero
skipped or wrong signatures, 93.67%/82.77% average source-shape/
strict fuzzy, 2,513 linked-exact and 1,074 byte-exact. Against
`objectgun_render_semantics_full_20261016.json`, both exact
`(unit,symbol)` sets are unchanged. See
[the offset audit](OBJECTGUN_UPDATE_OFFSETS_ARM_2026-10-16.md).

## Per-function missing-symbol triage (2026-10-16)

The latest full report's 201 missing names were classified and
checked by exact name against `arm-linux-androideabi-nm -g
--defined-only -P` on the current ZIP, zlib, OpenSSL and NDK r18b
libc++abi ARM archives. They partition as 74/47/23/57 with zero
unclassified and zero absent from those archives. This is an
archive-availability audit, not a new per-function build or a
whole-library link. The report's 4,323/4,524 coverage, 2,513
linked-exact and 1,074 byte-exact values remain unchanged. See
[the triage note](MISSING_SYMBOLS_ARCHIVE_TRIAGE_2026-10-16.md).

## ObjectGun render gating and animation (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks extracted production fragments
for inactive sentinel, turret rotation arguments, both lifetime
scales, cross order and mine wobble at host `-O0/-O2`. Native `gof2`,
207/207 ARM units and 40 verifier tests (one existing skip) pass.
Focused `render` source-shape/strict fuzzy improves 89.6%/39.7% ->
89.7%/41.5%, nonexact. Full report
`A:/Galaxy on Fire new version/_work/objectgun_render_semantics_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero
skipped or wrong signatures, 93.67%/82.77% average source-shape/
strict fuzzy, 2,513 linked-exact and 1,074 byte-exact. Against
`objectgun_visible_axes_full_20261016.json`, both exact
`(unit,symbol)` sets are unchanged. See
[the render audit](OBJECTGUN_RENDER_SEMANTICS_ARM_2026-10-16.md).

## ObjectGun visible orientation axes (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks the production orientation
assignments at host `-O0/-O2` with camera `8,9` distinct from
`10,11`. Native `gof2`, 207/207 ARM units and 40 verifier tests
(one existing skip) pass. Focused `render` source-shape/strict fuzzy
remains 89.6%/39.7%, nonexact. Full report
`A:/Galaxy on Fire new version/_work/objectgun_visible_axes_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero
skipped or wrong signatures, 93.67%/82.77% average source-shape/
strict fuzzy, 2,513 linked-exact and 1,074 byte-exact. Against
`objectgun_dtor_full_20261016.json`, both exact `(unit,symbol)`
sets are unchanged. See
[the orientation audit](OBJECTGUN_VISIBLE_ORIENTATION_ARM_2026-10-16.md).

## ObjectGun scatter explosion flow (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now checks extracted `update` and
`render` explosion fragments at host `-O0/-O2`. Native `gof2` has
no pending rebuild; 40 verifier tests pass (one skip). Production
source is unchanged. Focused `update` source-shape/strict fuzzy
remains 87.9%/31.0% and `render` 89.6%/39.7%, both nonexact.
The last full report at this source revision,
`A:/Galaxy on Fire new version/_work/objectgun_dtor_full_20261016.json`,
remains 4,323/4,524 compared, 201 missing, 124 extras, zero skipped
or wrong signatures, 93.67%/82.77% average source-shape/strict fuzzy,
2,513 linked-exact and 1,074 byte-exact. See
[the explosion flow audit](OBJECTGUN_EXPLOSION_FLOW_ARM_2026-10-16.md).

## ObjectGun destructor ownership (2026-10-16)

Native `gof2`, 207/207 ARM units, the ObjectGun host fixture and 40
verifier tests (one skip) pass. Focused `D1`/`D2` source-shape/strict
fuzzy improves 89.2%/95.1% -> 100%/100%; both become linked-exact.
`setScaling` remains byte-exact after source-backed confirmation that
ARM ignores its third argument. Full report
`A:/Galaxy on Fire new version/_work/objectgun_dtor_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped
or wrong signatures, 93.67%/82.77% average source-shape/strict fuzzy,
2,513 linked-exact and 1,074 byte-exact. Against
`objectgun_setenemies_full_20261016.json`, exact `(unit,symbol)`
sets have two linked gains and zero losses. See
[the destructor audit](OBJECTGUN_DESTRUCTOR_ARM_2026-10-16.md).

## ObjectGun enemy delegation (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks the extracted `setEnemies`
body with nonnull and null arrays at host `-O0/-O2`. Native `gof2`,
207/207 ARM units and 40 verifier tests (one skip) pass. Focused
source-shape/strict fuzzy improves 90.0%/50.0% -> 100%/100% and
becomes linked-exact; `setEnemy` and `translate` remain byte-exact.
Full report
`A:/Galaxy on Fire new version/_work/objectgun_setenemies_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped
or wrong signatures, 93.67%/82.77% average source-shape/strict fuzzy,
2,511 linked-exact and 1,074 byte-exact. Against
`objectgun_update_math_full_20261016.json`, exact `(unit,symbol)`
sets have one linked gain and zero losses. See
[the enemy delegation audit](OBJECTGUN_ENEMY_DELEGATION_ARM_2026-10-16.md).

## ObjectGun update math (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks extracted player/camera
direction inputs and turret matrix order at host `-O0/-O2`.
Native `gof2`, 207/207 ARM units and 40 verifier tests (one skip)
pass. Focused update source-shape/strict fuzzy improves
87.8%/29.0% -> 87.9%/31.0%; it is still nonexact. Full report
`A:/Galaxy on Fire new version/_work/objectgun_update_math_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped
or wrong signatures, 93.66%/82.75% average source-shape/strict fuzzy,
2,510 linked-exact and 1,074 byte-exact. Against
`objectgun_render_math_full_20261016.json`, exact `(unit,symbol)`
sets have zero gains and zero losses. See
[the update math audit](OBJECTGUN_UPDATE_MATH_ARM_2026-10-16.md).

## ObjectGun render math (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks extracted iPad scaling and ego
rotation fragments at host `-O0/-O2`. Native `gof2`, 207/207 ARM units
and 40 verifier tests (one skip) pass. Focused render source-shape
changes 90.4% -> 89.6%, while strict fuzzy improves 34.6% -> 39.7%;
it is still nonexact. Full report
`A:/Galaxy on Fire new version/_work/objectgun_render_math_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped,
no wrong signatures, 93.66%/82.75% average source-shape/strict fuzzy,
2,510 linked-exact and 1,074 byte-exact. Against
`objectgun_render_bindings_full_20261016.json`, exact `(unit,symbol)`
sets have zero gains and zero losses. See
[the math audit](OBJECTGUN_RENDER_MATH_ARM_2026-10-16.md).

## ObjectGun render bindings (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks extracted iPad scaling
and set-local/draw fragments at host `-O0/-O2`.
Native `gof2`, 207/207 ARM units and 40 verifier tests
(one skip) pass. Focused render source-shape/strict fuzzy
improves 86.8%/30.9% to 90.4%/34.6%, without exact status.
The full report
`A:/Galaxy on Fire new version/_work/objectgun_render_bindings_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras,
zero skipped, 93.66%/82.75% average source-shape/strict
fuzzy, 2,510 linked-exact and 1,074 byte-exact. Against
`objectgun_update_bindings_enum_full_20261016.json`,
exact `(unit,symbol)` sets have zero gains and zero losses.
See [the render audit](OBJECTGUN_RENDER_BINDINGS_ARM_2026-10-16.md).

## ObjectGun update bindings (2026-10-16)

The production inactive-delay branch is exercised by
`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` at host `-O0/-O2`.
Native `gof2`, 207/207 ARM units and 40 verifier tests (one
skip) pass. Focused update source-shape/strict fuzzy improves
83.8%/20.5% to 87.8%/29.0%, without exact status. The full report
`A:/Galaxy on Fire new version/_work/objectgun_update_bindings_enum_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras,
zero skipped, 93.66%/82.75% average source-shape/strict fuzzy,
2,510 linked-exact and 1,074 byte-exact. Exact `(unit,symbol)`
comparison with `objectgun_ctor_full_20261016.json` shows zero
gains and zero losses. See
[the update audit](OBJECTGUN_UPDATE_BINDINGS_ARM_2026-10-16.md).

## ObjectGun constructor bindings (2026-10-16)

The constructor's canvas/iPad GOT bindings and its shared 232-entry
signed mesh-ID table are source-backed by Android ARM. All source
table entries were compared with `.rodata 0x203398-0x203737`.
Native `gof2`, 207/207 ARM units and 40 verifier tests (one skip)
pass. Focused C1/C2 source-shape/strict fuzzy improves from
80.2%/41.1% to 87.4%/65.5%; neither is exact. The full report
`A:/Galaxy on Fire new version/_work/objectgun_ctor_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero
skipped, 93.66%/82.75% average source-shape/strict fuzzy, 2,510
linked-exact and 1,074 byte-exact. Against the prior
`objectgun_replace_full_20261016.json`, exact `(unit,symbol)`
sets have zero gains and zero losses. See
[the constructor audit](OBJECTGUN_CONSTRUCTOR_BINDINGS_ARM_2026-10-16.md).

## ObjectGun mesh replacement (2026-10-16)

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks the extracted production
`replaceGun` body at host `-O0` and `-O2`, including 16-bit ID
arguments and re-reading `Globals::Canvas` between the removal and
addition calls. Native `gof2`, 207/207 ARM units and 40 verifier tests
(one skip) pass. Focused source-shape/strict fuzzy rises from
83.0%/41.2% to 100%/100%, linked-exact. The full report
`A:/Galaxy on Fire new version/_work/objectgun_replace_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped,
93.66%/82.74% average source-shape/strict fuzzy, 2,510 linked-exact
and 1,074 byte-exact. Against
`A:/Galaxy on Fire new version/_work/trail_canvas_full_20261016.json`,
exact `(unit,symbol)` sets have one linked gain and zero linked/byte
losses. See [the mesh replacement audit](OBJECTGUN_REPLACE_MESH_ARM_2026-10-16.md).

## Trail constructor behavior (2026-10-16)

`python tools/run_trail_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now also extracts `Trail::Trail(int,int)`
and runs under host `-O0` and `-O2`. Cases `(9,2)` and `(4,0)` check
mesh dimensions, ordered triangle/UV calls, both integer allocations,
transform/mesh attachment and the color chosen by `changeType`.
Native `gof2`, 207/207 ARM units and 40 verifier tests (one skip) pass.
No production C++ changed; focused report
`A:/Galaxy on Fire new version/_work/trail_ctor_semantics_focused_20261016.json`
remains 98.76% source-shape / 88.72% strict fuzzy, 7 linked-exact
and 1 byte-exact among 12 functions. The prior full report remains
4,323/4,524 compared, 93.66%/82.73% average source-shape/strict
fuzzy, 2,509 linked-exact and 1,074 byte-exact. See
[the constructor audit](TRAIL_CONSTRUCTOR_SEMANTICS_ARM_2026-10-16.md).

## Trail update and width behavior (2026-10-16)

`python tools/run_trail_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now compiles extracted production
`render`, `changeType`, `setWidth` and float `update` bodies with both
`-O0` and `-O2`. It validates the ARM-observed point-shift ordering,
relative coordinates, six mesh point calls, local translation, and
width adjustments in both directions. Native `gof2`, 207/207 ARM
units and 40 verifier tests (one skip) pass. No production C++ changed,
so the focused Trail report
`A:/Galaxy on Fire new version/_work/trail_semantics_focused_20261016.json`
remains 98.76% source-shape / 88.72% strict fuzzy with 7 linked-exact
and 1 byte-exact; the previous full report remains 4,323/4,524
compared, 93.66%/82.73% average source-shape/strict fuzzy, 2,509
linked-exact and 1,074 byte-exact. See
[the semantics audit](TRAIL_UPDATE_WIDTH_SEMANTICS_ARM_2026-10-16.md).

## Trail canvas routing (2026-10-16)

`python tools/run_trail_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` executes extracted production `render`
and `changeType` bodies with a recording canvas. It checks all 13
tested type/color branches, render arguments and a changed global
canvas pointer. Native `gof2`, 207/207 ARM units and 40 verifier tests
(one skip) pass. Focused 12-function source-shape/strict fuzzy rises
93.65%/83.60% -> 98.76%/88.72%; linked-exact rises 6 -> 7.
Full report `A:/Galaxy on Fire new version/_work/trail_canvas_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped,
93.66%/82.73% average source-shape/strict fuzzy, 2,509 linked-exact
and 1,074 byte-exact. Against
`A:/Galaxy on Fire new version/_work/aegeometry_render_full_20261016.json`,
exact `(unit,symbol)` sets have one linked gain (`Trail::render`) and
zero linked/byte losses. See [the Trail audit](TRAIL_CANVAS_ROUTING_ARM_2026-10-16.md).

## AEGeometry render routing (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now covers the production `render()`
body's low-byte visibility gate and typed `DrawTransform` arguments.
It failed to compile before removal of the undeclared hook and passes
afterward. The original ARM tail-branch destination was resolved
manually through a veneer to the named `DrawTransform` PLT entry;
the generic direct-call audit excludes `R_ARM_THM_JUMP24`. Focused
source-shape/strict fuzzy remains 100%/100%, linked-exact. Native
`gof2`, 207/207 ARM units and 40 verifier tests (one skip) pass.
Full report
`A:/Galaxy on Fire new version/_work/aegeometry_render_full_20261016.json`
has 4,323/4,524 compared, 93.64%/82.72% average source-shape/strict
fuzzy, 2,508 linked-exact and 1,074 byte-exact. Compared with
`A:/Galaxy on Fire new version/_work/aegeometry_set_position_split_full_20261016.json`,
there are no exact `(unit,symbol)` gains or losses. See
[the render audit](AEGEOMETRY_RENDER_ARM_2026-10-16.md).

## AEGeometry position setter (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` exercises both production `setPosition`
overloads against a recording canvas/math fixture. The `Vector`
overload and numeric setter each reach linked-exact in focused ARM
verification; numeric source-shape/strict fuzzy rises from 0.7%/0.0%
to 100%/100%. Native `gof2`, 207/207 ARM units and 40 verifier tests
(one skip) pass. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_set_position_split_full_20261016.json`
compares 4,323/4,524 originals, 201 missing, 124 extras, zero skipped,
93.64%/82.72% average source-shape/strict fuzzy, 2,508 linked-exact and
1,074 byte-exact. Relative to
`A:/Galaxy on Fire new version/_work/aerandom_bound_full_20261016.json`,
exact `(unit,symbol)` sets have one linked gain and zero linked/byte
losses. See [the position audit](AEGEOMETRY_SET_POSITION_ARM_2026-10-16.md).

## Bounded AERandom semantics (2026-10-16)

`tools/aerandom_bound_smoke.cpp` compares 2,560 production
`AERandom::nextInt(int)` results and seed states against an ARM-derived
48-bit model. It passes with optimized UCRT64 compilation and with
`-ftrapv`; the overflow-trap build terminated before the unsigned
power-of-two test was fixed. Native `gof2`, 206/206 ARM units and 40
verifier tests (one skip) pass. Focused `nextInt(int)` remains 96.4%
source-shape / 50.5% strict fuzzy, nonexact. Full report
`A:/Galaxy on Fire new version/_work/aerandom_bound_full_20261016.json`
compares 4,323/4,524 original unique functions at 93.62%/82.69%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact, with zero exact-pair losses. See
[the RNG audit](AERANDOM_BOUND_ARM_2026-10-16.md).

## Particle UV selector mapping (2026-10-16)

`python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks all four production `rotateUVs`
outputs against selectors derived from original ARM instructions for
256 signed seeds. Native `gof2`, 206/206 ARM units and 40 verifier
tests (one skip) pass. Focused report
`A:/Galaxy on Fire new version/_work/particle_rotate_uv_focused_20261016.json`
shows 95.2% source-shape / 72.0% strict fuzzy, nonexact. Production
C++ is unchanged, so the prior full corpus report and its 2,507
linked-exact / 1,074 byte-exact pairs still apply. See
[the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## Base particle UV dispatch and second-frame guard (2026-10-16)

`python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` checks production-body UV dispatch for
`0x02000000` and its absence for `0x80000000`, all four UV outputs,
and suppression of a second emission for flags `0x80` and `0x40`.
The positive-bit assertion failed before the source correction and
passes afterward. Native `gof2`, 206/206 ARM units and 40 verifier
tests (one skip) pass. Focused `emit` is 88.8% source-shape / 19.9%
strict fuzzy, nonexact; the unchanged `rotateUVs` helper is 95.2% /
72.0%, nonexact. Focused report:
`A:/Galaxy on Fire new version/_work/particle_emit_uv_focused_20261016.json`.
Full report `A:/Galaxy on Fire new version/_work/particle_emit_uv_full_20261016.json`
compares 4,323/4,524 original unique functions, with 201 missing,
124 extras, zero skipped, 93.62%/82.69% average source-shape/strict
fuzzy, 2,507 linked-exact and 1,074 byte-exact. Exact pair sets have
zero losses or gains against the preceding full report.
See [the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## Base particle random-emission checks (2026-10-16)

`python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now also checks both random-position
branches of the production `IParticleSystem::emit` body against the
ARM-observed RNG call order, final seed, position, velocity and
lifetime/size outputs. Native `gof2`, 206/206 ARM units and 40
verifier tests (one skip) pass. Production C++ is unchanged, so the
preceding full corpus report and its 2,507 linked-exact / 1,074
byte-exact pairs still apply. Fresh focused report:
`A:/Galaxy on Fire new version/_work/particle_emit_random_focused_20261016.json`
(88.5% source-shape, 16.7% strict fuzzy, nonexact). See
[the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## Base particle-emission position/timing (2026-10-16)

`python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` compiles the production
`IParticleSystem::emit` body and checks particle position and update
timing at 150 ms and 200 ms frame lengths, plus configured spacing.
Native `gof2`, 206/206 ARM units and
40 verifier tests (one skip) pass. Focused `emit` is 88.5%
source-shape, 16.7% strict fuzzy, nonexact. Full report
`A:/Galaxy on Fire new version/_work/particle_emit_step_full_20261016.json`
keeps 4,323/4,524 original unique comparisons, 93.62%/82.69%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact. Exact pair sets have zero losses or gains against the
prior full report. See [the timing audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## ParticleSystemMesh ordinary-emission dispatch (2026-10-16)

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now tests the production `emit` branch
against a recording base-emitter stub and the recovered trail method.
The native `gof2` build, 206/206 ARM units and 40 verifier tests (one
skip) pass. ARM `emit` remains 100%/100% strict fuzzy/source-shape and
linked-exact; its ordinary tail relocation now names
`IParticleSystem::emit`. Full report
`A:/Galaxy on Fire new version/_work/particle_emit_dispatch_full_20261016.json`
retains 4,323/4,524 original unique comparisons, 93.62%/82.69%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact. Exact `(unit,symbol)` pair sets have zero losses/gains
against the preceding full report. See
[the dispatch audit](PARTICLE_MESH_EMIT_DISPATCH_ARM_2026-10-16.md).

## ParticleSystemMesh trail emission (2026-10-16)

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` exercises the production `emitTrail` body
with recorded canvas calls and fake neighboring implementations. Native
`gof2`, 206/206 ARM units and 40 verifier tests (one skip) pass. The
function moves from a one-instruction stub at 0%/0% strict fuzzy/
source-shape to a 605-instruction reconstruction at 37.5%/94.9%; it is
not exact. Full report
`A:/Galaxy on Fire new version/_work/particle_emit_trail_full_20261016.json`
compares 4,323/4,524 original unique functions at 93.62%/82.69%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact. Exact `(unit,symbol)` pair comparison against the prior
full report found zero losses/gains. One extra forwarding symbol raises
extras to 124. The linked-call audit is unchanged at 2,392 matches,
53 mismatch candidates and 647 unresolved; it does not cover the
nonexact trail method. See
[the trail audit](PARTICLE_MESH_EMIT_TRAIL_ARM_2026-10-16.md).

## AEGeometry LOD selection and transfer (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now exercises twenty production
AEGeometry bodies. New cases cover alternate/fallback position
transforms, camera delta, culling, `0.33`/`0.66`/`NaN` factors, LOD
transitions, base fallback, matrix destinations and animation time.
Native `gof2`, 206/206 ARM units and 40 verifier tests (one skip)
pass. Focused `updateLod` strict fuzzy/source-shape improves from
82.8%/96.4% to 97.6%/100%; it is not exact. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_lod_full_20261016.json`
compares 4,323/4,524 unique original functions at 93.60%/82.69%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact. Exact `(unit,symbol)` pair comparison against the previous
full report shows zero losses/gains. The separate linked-function
call audit records 2,392 matches, 53 mismatch candidates and 647
unresolved; this nonexact method's 19 calls were checked manually.
See [the focused audit](AEGEOMETRY_UPDATE_LOD_ARM_2026-10-16.md).

## AEGeometry direction basis (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now exercises nineteen production
AEGeometry bodies. The new direction cases check nonunit and zero-up
inputs, three unequal scale values, preserved translation, and the
original column layout. `python tools/run_native_math_link_smoke.py
--cxx C:/msys64/ucrt64/bin/g++.exe` checks the actual vector
cross/normalize and matrix-scaling implementations for these inputs.
Native `gof2`, 206/206 ARM units and 40 verifier tests (one skip)
pass. Focused `setDirection` strict fuzzy/source-shape improves from
45.1%/95.6% to 85.4%/100%; it is not exact. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_direction_full_20261016.json`
compares 4,323/4,524 unique original functions at 93.60%/82.68%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact. Exact `(unit,symbol)` pair comparison against the previous
full report shows zero losses/gains. The separate linked-function
call audit records 2,392 matches, 53 mismatch candidates and 647
unresolved; this nonexact method's ten calls were checked manually.
See [the focused audit](AEGEOMETRY_DIRECTION_ARM_2026-10-16.md).

## AEGeometry translation axes (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now exercises eighteen production bodies.
New float and vector translation cases use unequal starting coordinates
to check all three matrix-axis increments and write-back. Native
`gof2`, 206/206 ARM units and 40 verifier tests (one skip) pass. The
float overload reaches 100%/100% source-shape/strict fuzzy and
linked-exact; focused AEGeometry totals are 27 linked-exact and 8
byte-exact across 45 functions. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_translate_full_20261016.json`
compares 4,323/4,524 unique original functions at 93.60%/82.67%
average source-shape/strict fuzzy, 2,507 linked-exact and 1,074
byte-exact. Exact `(unit,symbol)` set comparison against the previous
full report shows zero losses and one linked gain. The separate
call-target audit records 2,392 matches, 53 mismatch candidates and
647 unresolved. See
[the focused audit](AEGEOMETRY_TRANSLATE_ARM_2026-10-16.md).

## AEGeometry rotation and scaling calls (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now exercises sixteen production AEGeometry
bodies; new cases record matrix call order, object identity, arguments
and state changes in rotation/scaling. Native `gof2`, 206/206 ARM units
and 40 verifier tests (one skip) pass. The three changed methods are
100%/100% source-shape/strict fuzzy and linked-exact; focused totals
are 26 linked-exact and 8 byte-exact across 45 functions. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_rotation_full_20261016.json`
compares 4,323/4,524 unique original functions at 93.59%/82.65%
average source-shape/strict fuzzy, 2,506 linked-exact and 1,074
byte-exact. Compared with the preceding report, exact `(unit,symbol)`
sets show zero losses and three linked gains. The separate call audit
records 2,387 matches, 53 mismatch candidates and 647 unresolved.
See [the focused audit](AEGEOMETRY_ROTATION_SCALING_ARM_2026-10-16.md).

## AEGeometry construction and LOD calls (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now exercises 13 production method bodies,
including both constructors, `setMatrix` and two LOD-child setters. A
nonzero created transform index verifies the mesh constructor's reference
matrix lookup. Native `gof2`, 206/206 ARM units and 40 verifier tests (one
skip) pass. Focused `AEGeometry` verification compares 45 functions with
23 linked-exact and 8 byte-exact. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_ctor_lod_full_20261016.json`
compares 4,323/4,524 unique original functions at 93.59%/82.64% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074 byte-exact.
Exact `(unit,symbol)` pair comparison against the previous full report
shows zero losses/gains. The separate full call-target audit stays at
2,372 matches, 53 mismatch candidates and 647 unresolved. See
[the focused audit](AEGEOMETRY_CTOR_LOD_CALLS_ARM_2026-10-16.md).

## AEGeometry canvas call contracts (2026-10-16)

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`
compiles eight production transform
method bodies with a recording `PaintCanvas` fixture. It checks
transform indices, returned matrix/vector values, both parent-index
branches, reference-matrix copy and child-link bookkeeping. Native
`gof2`, 206/206 ARM units and 40 verifier tests (one skip) pass. All
ten changed methods remain 100%/100% source-shape/strict fuzzy and
linked-exact, not byte-exact; all 12 corrected direct-call targets
match Android ARM. Full report
`A:/Galaxy on Fire new version/_work/aegeometry_canvas_calls_full_20261016.json`
compares 4,323/4,524 unique functions at 93.59%/82.64% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074
byte-exact and zero exact `(unit,symbol)` losses/gains. A separate
call-target audit records 2,372 matches, 53 mismatch candidates and
647 unresolved sites. The LOD bodies are not exercised by the host
fixture. See [the focused audit](AEGEOMETRY_CANVAS_CALLS_ARM_2026-10-16.md).

## PaintCanvas text-line call contracts (2026-10-16)

`python tools/run_paintcanvas_draw_text_lines_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` compiles the production bodies of the
two alignment overloads with instrumented text-method fixtures. It
checks centered/right-aligned offsets, no-alignment paths, line
spacing, method arguments and an empty array. Native `gof2`, 206/206
ARM units and 40 verifier tests (one skip) pass. Both changed
overloads stay 100%/100% source-shape/strict fuzzy and linked-exact,
not byte-exact; all six direct-call targets now match Android ARM.
Full report `A:/Galaxy on Fire new version/_work/draw_text_lines_full_20261016.json`
compares 4,323/4,524 unique functions at 93.59%/82.64% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074
byte-exact and zero exact `(unit,symbol)` losses/gains. A separate
call-target audit records 2,360 matches, 65 mismatch candidates and
647 unresolved calls. See [the focused audit](PAINTCANVAS_TEXT_LINES_CALLS_ARM_2026-10-16.md).

## Camera view-frustum flag (2026-10-16)

Android ARM point and sphere frustum checks load `Engine::vfc`.
Both recovered bodies now read that byte instead of a never-assigned
pointer. Native `gof2`, 206/206 ARM units, 40 verifier tests (one
skip), and the existing canvas-wrapper smoke pass. Focused point
source-shape/strict fuzzy changes 81.1%/53.8% to 82.3%/54.0%; sphere
changes 82.5%/34.9% to 82.4%/46.3%. Neither is exact. Full report
`A:/Galaxy on Fire new version/_work/camera_vfc_full_20261016.json`
compares 4,323/4,524 unique functions at 93.59%/82.64% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074
byte-exact. Comparison against the prior full report shows zero
exact `(unit,symbol)` losses or gains. The wrapper smoke does not
exercise the production core frustum computations. See
[the focused audit](CAMERA_VFC_FLAG_ARM_2026-10-16.md).

## MeshIntersect shader flag (2026-10-16)

`python tools/run_mesh_intersect_flag_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`
compiles the production `MeshIntersect` body and tests hit/miss,
an empty index list, and both `Engine::enableShader` V-coordinate
branches. Native `gof2`, 206/206 ARM units, 40 verifier tests
(one skip), and the fixture pass. Focused source-shape/strict fuzzy
improves 84.7%/43.0% to 86.2%/43.7%; no exact match is claimed.
Full report `A:/Galaxy on Fire new version/_work/mesh_intersect_flag_full_20261016.json`
compares 4,323/4,524 unique functions at 93.59%/82.63% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074
byte-exact and no exact-pair losses or gains. See
[the focused audit](MESH_INTERSECT_SHADER_FLAG_ARM_2026-10-16.md).

## SpriteSystemCreate shader flag (2026-10-16)

The original Android ARM body reads `Engine::enableShader` before
conditionally filling mesh tangent/binormal arrays. The recovered
body now reads that byte rather than an uninitialized pointer.
Native `gof2`, 206/206 ARM units and 40 verifier tests (one skip)
pass. Focused strict fuzzy changes 66.9% to 67.1% and source-shape
95.2% to 95.0%; no exact match is claimed. Full report
`A:/Galaxy on Fire new version/_work/sprite_create_flag_full_20261016.json`
compares 4,323/4,524 unique functions at 93.59%/82.63% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074
byte-exact and no exact-pair losses/gains. See
[the focused audit](SPRITE_CREATE_SHADER_FLAG_ARM_2026-10-16.md).

## Sprite UV/RGBA data helpers (2026-10-16)

`python tools/run_sprite_uv_rgba_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`
compiles the production `SpriteSystemSetAllUv`, `SetUv`, and `SetRGBA`
bodies in a small host fixture. It checks null/bounds behavior, the
two `Engine::enableShader` UV paths, and per-vertex RGBA output. The
corrected `SetUv` reads the ARM-observed static flag rather than an
uninitialized pointer. Native `gof2`, 206/206 ARM units, 40 verifier
tests (one skip), and the fixture pass. Focused `SetUv`
source-shape/strict fuzzy improves 87.9%/46.6% to 88.9%/52.8%.
Full report `A:/Galaxy on Fire new version/_work/sprite_uv_flag_full_20261016.json`
compares 4,323/4,524 unique functions at 93.59%/82.63% average
source-shape/strict fuzzy, 2,503 linked-exact and 1,074 byte-exact,
with no exact-pair losses or gains. See
[the focused audit](SPRITE_UV_RGBA_FLAG_ARM_2026-10-16.md).

## PaintCanvas sprite-system call contracts (2026-10-16)

`SpriteSystemSetAllUv`, `SpriteSystemSetUv` and `SpriteSystemSetRGBA`
now call the original-named `AbyssEngine::SpriteSystemSet*` functions
instead of undefined transition shims. Focused source-shape/strict
fuzzy stays 100%/100% for all three; each remains linked-exact, not
byte-exact, and its direct call target now matches. Native `gof2`,
206/206 ARM units and 40 verifier tests (one skip) pass. Full report
`A:/Galaxy on Fire new version/_work/paintcanvas_sprite_calls_full_20261015.json`
compares 4,323/4,524 unique functions at 93.59%/82.63% average
source-shape/strict fuzzy, 2,503 linked-exact and 1,074 byte-exact,
with zero exact-pair losses or gains. Direct-call-target matches rise
to 2,354. See [the focused audit](PAINTCANVAS_SPRITE_CALLS_ARM_2026-10-16.md).

## PaintCanvas StopDraw2FBO call contract (2026-10-15)

The four undefined `paintcanvas_ext_fbo_*` calls now invoke the original
ARM-observed engine/canvas methods. Three direct-call targets match by
symbol; the fourth is a tail `R_ARM_THM_JUMP24` to
`Engine::ActivateViewBuffer`, identified separately with REA/IDA.
Focused source-shape/strict fuzzy stays 100%/100%; the function remains
linked-exact, not byte-exact. Native `gof2` and 206/206 ARM units pass.
Full report `A:/Galaxy on Fire new version/_work/paintcanvas_stop_fbo_full_20261015.json`
compares 4,323/4,524 unique functions at 93.59%/82.63% average
source-shape/strict fuzzy, 2,503 linked-exact and 1,074 byte-exact,
with no exact-pair losses/gains. Direct-call matches rise to 2,351.
See [the focused audit](PAINTCANVAS_STOP_FBO_CALL_ARM_2026-10-15.md).

## ImageFactory RNG call contract (2026-10-15)

All three `ImageFactory::createChar` RNG calls now target the ARM-observed
`AERandom::nextInt(int)` member instead of an undefined free function.
Focused source-shape/strict fuzzy remains 100%/100% for both overloads;
both remain linked-exact, not byte-exact. Native `gof2`, the portrait smoke
and 206/206 ARM units pass. Full report
`A:/Galaxy on Fire new version/_work/image_factory_rng_full_20261015.json`
compares 4,323/4,524 unique functions at 93.59%/82.63% average
source-shape/strict fuzzy, with 2,503 linked-exact and 1,074 byte-exact;
the exact-pair sets have zero losses/gains. Direct-call-target matches rise
by three to 2,348. See [the focused audit](IMAGE_FACTORY_RNG_CALL_ARM_2026-10-15.md).

## Direct ARM call-target audit (2026-10-15)

`linked-exact` means equality of bytes **outside** locally recorded relocation
fields; it does not by itself verify the resolved call/data targets. A separate
audit of direct `R_ARM_THM_CALL`/`R_ARM_CALL` sites in the prior 2,503
linked-equal function-pairs found 2,345 matching call symbols, 80 mismatch
candidates and 647 unresolved sites among 3,072 calls. These counts do not
replace or change the corpus metrics. See [the call-target audit](ARM_CALL_TARGET_AUDIT_2026-10-15.md).

## String numeric constructors (2026-10-15)

`String(char)`, `String(int)` and `String(long long)` now initialize only
the null data pointer before calling `Set(long long)`; the native `char`
path converts through `unsigned char`. Native `gof2`, the ABI/lifetime
smoke, standalone String reverse check, **206/206** ARM units and **37**
verifier tests (one skip) pass. Focused C2 source-shape/strict fuzzy is
**100%/100%** for all three versus **97.6%/97.0%**, **97.6%/97.0%** and
**97.3%/96.6%** before; all gain linked-exact, not byte-exact equality.
The full report `_work/string_numeric_ctors_full_20261015.json` compares
**4,323/4,524** original functions, **201 missing**, **123 extras**, zero
skipped and **93.59%/82.63%** average source-shape/strict fuzzy.
Against `_work/string_append_int_full_20261015.json`, the linked-exact
set gains only these three pairs (**2,500 → 2,503**) and loses none;
byte-exact remains **1,074** with zero losses. See
[the constructor and append audit](STRING_NUMERIC_CTORS_ARM_2026-10-15.md).

## String signed-int append (2026-10-15)

`String::operator+=(int const&)` now sign-extends and delegates directly to
`operator+=(long long const&)`, as observed at Android ARM `0x72928–0x72950`.
Native `gof2`, ABI/lifetime and standalone String checks, **206/206** ARM
units and **37** verifier tests (one skip) pass. Focused source-shape/
strict fuzzy rises **53.0%/14.3% → 100%/100%**, with linked-exact but
not byte-exact equality. Full report
`_work/string_append_int_full_20261015.json` compares **4,323/4,524**
original functions, **201 missing**, **123 extras**, zero skipped,
**93.59%/82.63%** average source-shape/strict fuzzy. Against
`_work/string_set_i64_full_20261015.json`, linked-exact gains only this
symbol (**2,499 → 2,500**) with zero losses; byte-exact remains **1,074**
with zero losses. See
[the signed-int append audit](STRING_APPEND_INT_ARM_2026-10-15.md).

## String 64-bit integer conversion (2026-10-15)

`String::Set(long long)` now uses unsigned magnitude conversion and direct
UTF-16 output, avoiding signed overflow at `INT64_MIN`; it deliberately
allocates 42 rather than the original ARM's 40 bytes for safe negative
19-digit output. Native `gof2`, the ABI/lifetime smoke and standalone
String reverse check, **206/206** ARM units and **37** verifier tests
(one skip) pass. Focused source-shape/strict fuzzy rises
**65.1%/18.9% → 89.4%/21.9%**, without an exact match. The full report
`_work/string_set_i64_full_20261015.json` compares **4,323/4,524**
original functions, **201 missing**, **123 extras**, zero skipped,
**93.58%/82.61%** average source-shape/strict fuzzy, and zero exact-pair
losses or gains versus `_work/float_set_object_full_20261015.json`:
**2,499 linked-exact / 1,074 byte-exact**. See
[the integer-conversion audit](STRING_SET_I64_ARM_2026-10-15.md).

## String float object composition (2026-10-15)

`String::Set(float)` now follows the ARM-observed local `String`
composition, substring insertion and temporary destruction sequence.
Native `gof2`, ABI/lifetime and standalone String checks, **206/206** ARM
units and **37** verifier tests (one skip) pass. Focused source-shape/
strict fuzzy rises **43.4%/12.3% → 96.6%/65.6%**, without an exact
match. Full report `_work/float_set_object_full_20261015.json` compares
**4,323/4,524** unique original functions, **201 missing**, **123
extras**, zero skipped and no losses in the **2,499 linked-exact /
1,074 byte-exact** symbol sets. See
[the object-composition audit](STRING_FLOAT_SET_OBJECT_ARM_2026-10-15.md).

## String float digit helper and call contract (2026-10-15)

`AbyssEngine::computeFloatString` now returns the ARM-observed owned UTF-16
buffer. At this historical gate, the local `String::Set(float)` called its
five-argument symbol with mode 0; later raw ARM verification established
mode 1 and the 2026-10-19 String float pass corrected that caller.
The production-body ABI smoke checks positive, negative, zero and fractional
values. Native `gof2`, standalone String reverse check, **206/206** ARM units
and **37** verifier tests (one skip) pass. Focused helper source-shape/strict
fuzzy rises **70.5%/20.5% → 86.8%/26.0%**; `Set(float)` rises
**42.3%/11.7% → 43.4%/12.3%**, without an exact match. Full report
`_work/float_helper_full_20261015.json` compares **4,323/4,524** original
functions, **201 missing**, **123 extras**, zero skipped and no losses in
the **2,499 linked-exact / 1,074 byte-exact** symbol sets. See
[the helper audit](STRING_FLOAT_HELPER_ARM_2026-10-15.md).

## PaintCanvas camera creation and ABI (2026-10-15)

`CameraCreate` now uses the real `Camera` constructor and
`ArrayAdd<Camera*>`, replacing two undefined helper calls. Run the
production-body smoke:

```powershell
python tools/run_canvas_camera_create_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected: `PaintCanvas camera creation production smoke: OK`. Native
`gof2` and 206/206 ARM units pass. Focused
`_work/camera_create_focused_20261015.json` improves source-shape/
strict fuzzy **76.8%/69.6% → 84.7%/75.7%**; the function remains
non-exact. See [the camera ABI audit](PAINTCANVAS_CAMERA_CREATE_ARM_2026-10-15.md).
Full report `_work/camera_create_full_20261015.json` compares
**4,323/4,524** at **93.46%** average source-shape and **82.36%** strict
fuzzy. Both exact-symbol sets are unchanged with no losses:
**2,486 linked-exact**, **1,065 byte-exact**. Missing/extra/skipped are
**201/123/0**.

## ParticleSystemSprite camera-matrix render (2026-10-15)

The two-argument static render overload now copies both 0x3c-byte camera
matrices directly. All 206 ARM translation units and native `gof2` build pass.
Focused ARM report `_work/particle_render_focused_20261015.json` records
**71.0%/29.8% → 100%/100%** source-shape/strict fuzzy, 107/107
instructions and one new linked-exact (not byte-exact) match. See
[the render audit](PARTICLE_SPRITE_RENDER_ARM_2026-10-15.md).
Full report `_work/particle_render_full_20261015.json` compares
**4,323/4,524** at **93.46%** average source-shape and **82.36%** strict
fuzzy; linked-exact rises to **2,486** with exactly the render symbol gained
and no losses, byte-exact remains **1,065** with no losses. Missing/extra/
skipped remain **201/123/0**.

## ParticleSystemSprite ordinary update (2026-10-15)

The area-exit host runner now also extracts the production `updateSingle`
body and its source constants, checking expiry, size, UV and millisecond
movement corrections alongside area-exit behavior:

```powershell
python tools/run_particle_area_exit_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected: `Particle sprite update production smoke: OK`. Native `gof2`,
206/206 ARM units and 37 verifier tests (one skip) pass. Focused
`updateSingle` source-shape/strict fuzzy changes **87.3%/39.8% →
92.9%/49.0%**, with no exact claim. See
[the update audit](PARTICLE_SPRITE_UPDATE_ARM_2026-10-15.md).
Full report `_work/particle_sprite_update_full_20261015.json` compares
**4,323/4,524** at **93.45%** average source-shape and **82.35%** strict
fuzzy; both exact-symbol sets remain unchanged at **2,485 linked-exact**
and **1,065 byte-exact**, with zero losses, **201 missing**, **123 extras**
and zero skipped.

## ParticleSystemSprite area-exit (2026-10-15)

The former undefined-helper call in `updateAreaExitParticle` is replaced
by the Android ARM-backed body. Run its extracted-production host check:

```powershell
python tools/run_particle_area_exit_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected: `Particle sprite update production smoke: OK`. Native `gof2`, all 206
ARM units and 37 verifier tests (one skip) pass. Focused source-shape/strict
fuzzy changes **1.8%/0.9% → 94.6%/66.7%** after the `colorFlag` field
correction; no exact result is claimed. Historical full report
`_work/particle_area_exit_full_20261015.json` (before that correction) compares
**4,323/4,524** at **93.45%** average source-shape and **82.34%**
strict fuzzy, with **2,485 linked-exact**, **1,065 byte-exact** and
zero exact-symbol losses versus the preceding report.
See [the area-exit audit](PARTICLE_SPRITE_AREA_EXIT_ARM_2026-10-15.md).

## PlayerEgo endExplosion (2026-10-15)

REA/IDA Android Thumb `0xa15c6–0xa15d2` supports the conditional
`Explosion::reset()` call. Extracted-production/adjacent smokes,
native `gof2`, 206/206 ARM units and 37 verifier tests (one skipped)
pass. Focused source-shape/strict fuzzy improves **59.2%/36.4% →
100%/100%** and gains linked-exact, not byte-exact. Full report
`_work/player_end_explosion_full_20261015.json` compares
**4,323/4,524** at **93.43%** average source-shape and **82.33%**
strict fuzzy, with **2,485 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Linked-exact gains
only `endExplosion`; both exact sets have zero losses versus
`_work/player_explode_full_20261015.json`. Live Android/iOS reset
behavior is unverified. See
[the explosion audit](PLAYER_EGO_EXPLODE_ARM_2026-10-15.md).

## PlayerEgo explode (2026-10-15)

REA/IDA Android Thumb `0xa13e0–0xa1488` supports the particle,
camera/player, Explosion, FMOD stop and exhaust sequence recovered in
`explode`. Native `gof2`, 206/206 ARM units, extracted-production and
adjacent revive smokes, and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy changes **61.7%/49.5% →
100%/100%**; the function remains non-exact. Full report
`_work/player_explode_full_20261015.json` compares **4,323/4,524**
at **93.42%** average source-shape and **82.32%** strict fuzzy, with
**2,484 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras** and zero skipped. Both exact symbol sets are unchanged
versus `_work/player_revive_final_full_20261015.json`. Live Android
ARM32 and iOS death flow remain unverified. See
[the explode audit](PLAYER_EGO_EXPLODE_ARM_2026-10-15.md).

## PlayerEgo revive (2026-10-15)

REA/IDA Android Thumb `0xa14a4–0xa1586` supports the particle, explosion,
sound, health, position and direction sequence restored in `revive`.
Native `gof2`, 206/206 ARM units, extracted-production/adjacent smokes,
the `Player` recovery check and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy changes **88.4%/64.9% →
93.0%/70.3%**; it remains non-exact. Final full report
`_work/player_revive_final_full_20261015.json` compares **4,323/4,524**
at **93.41%** average source-shape and **82.31%** strict fuzzy, with
**2,484 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras** and zero skipped. Both exact symbol sets are unchanged
versus `_work/player_autopilot_full_20261015.json`. Live Android ARM32
and iOS revive behavior remain unverified. See
[the revive audit](PLAYER_EGO_REVIVE_ARM_2026-10-15.md).

## PlayerEgo autopilot waypoint distance (2026-10-15)

REA/IDA pseudocode and Android Thumb `0x9e344–0x9e3e8` confirm the
target Vector at `+0xec`, position subtraction, `VectorLength`, signed
integer conversion and inclusive 19999 threshold. Native `gof2`, all
206 ARM units, extracted-production and adjacent smokes, and 37
verifier tests (one skipped) pass. Focused `update` source-shape/
strict fuzzy changes **80.1%/30.8% → 80.6%/30.8%**; it remains
non-exact. Full report `_work/player_autopilot_full_20261015.json`
compares **4,323/4,524** functions at **93.41%** average source-shape
and **82.31%** strict fuzzy, with **2,484 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras**, zero skipped. Both exact
symbol sets are unchanged versus `_work/player_drive_charge_full_20261015.json`.
Live ARM32 waypoint and iOS analogue behavior are not yet validated.
See [the autopilot audit](PLAYER_EGO_AUTOPILOT_DISTANCE_ARM_2026-10-15.md).

## PlayerEgo jump-drive charge in update (2026-10-15)

REA/IDA inspection of Android Thumb `0x9d8a6–0x9d8f6` supports the
charge, threshold-event and volatile-goods branch restored in `update`.
Native `gof2`, all 206 ARM units, extracted-production and adjacent
smokes, and 37 verifier tests (one skipped) pass. Focused source-shape/
strict fuzzy changes **79.5%/32.3% → 80.1%/30.8%**; `update` stays
non-exact. Full report `_work/player_drive_charge_full_20261015.json`
compares **4,323/4,524** functions at **93.41%** average source-shape
and **82.31%** strict fuzzy, with **2,484 linked-exact** and **1,065
byte-exact**, **201 missing**, **123 extras**, zero skipped. Both exact
symbol sets are unchanged versus `_work/player_jump_full_20261015.json`.
The local iOS 1.1.4 ARMv7 IPA and SuperDuperHyperHle's guest GDB server
could supply runtime behavior for an analogous path, but no such trace
was taken for this package. See
[the charge audit](PLAYER_EGO_DRIVE_CHARGE_ARM_2026-10-15.md).

## PlayerEgo jump-drive initialization and activation (2026-10-15)

Targeted REA/IDA reads of Android Thumb `0x99a30–0x99a38`,
`0x99d56–0x99dcc` and `0x9a6c4–0x9a700` confirm jump-drive state,
equipment and activation order. Native `gof2`, 206/206 ARM units,
extracted-production and adjacent smokes, and 37 verifier tests (one
skipped) pass. Focused constructor C1/C2 source-shape/strict fuzzy
changes **73.9%/41.6% → 77.8%/47.3%**; `startJumpDrive` changes
**91.9%/85.1% → 100%/96.2%**. The full report
`_work/player_jump_full_20261015.json` compares **4,323/4,524** at
**93.41%** average source-shape and **82.31%** strict fuzzy, with
**2,484 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses versus
`_work/player_boost_full_20261015.json`. None of the three targeted
symbols is exact. Live ARM32 jump/audio remains unverified. See
[the jump-drive audit](PLAYER_EGO_JUMP_DRIVE_ARM_2026-10-15.md).

## PlayerEgo boost setup and activation (2026-10-15)

Android Thumb `0x99b20–0x99bbc` and `0x9aea4–0x9aefa` confirms
boost equipment, availability, sound ID and play gain. Native
`gof2`, 206/206 ARM units, the extracted-production and adjacent
smokes, and 37 verifier tests (one skipped) pass. Focused
constructor source-shape/strict fuzzy changes **68.2%/41.3% →
73.9%/41.6%** for C1/C2; neither is exact. Focused `boost()`
changes **90.1%/67.8% → 100%/100%**, becoming linked-exact but
not byte-exact. Full report `_work/player_boost_full_20261015.json`
compares **4,323/4,524** at **93.41%** average source-shape and
**82.30%** strict fuzzy, with **2,484 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras** and zero skipped.
The linked-exact set gains only `boost()` and loses none; the
byte-exact set changes by zero versus
`_work/player_cloak_ctor_full_20261014.json`. Live ARM32
boost/audio remains unverified. See
[the boost audit](PLAYER_EGO_BOOST_ARM_2026-10-15.md).

## PlayerEgo cloak-state constructor (2026-10-15)

Android Thumb `0x99a12`, `0x99cdc–0x99d4e` and
`0x99f56–0x99f94` confirm the cloak item, timers, sound resource
and difficulty thresholds. Native `gof2`, 206/206 ARM units,
the extracted-fragment and adjacent smokes, and 37 verifier tests
(one skipped) pass. Focused C1/C2 source-shape/strict fuzzy changes
**61.0%/35.2% → 68.2%/41.3%**; neither is exact. Full report
`_work/player_cloak_ctor_full_20261014.json` compares
**4,323/4,524** at **93.40%** average source-shape and **82.29%**
strict fuzzy, with **2,483 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Neither exact
pair set gains or loses a member against
`_work/player_supernova_ctor_full_20261014.json`. Full live ARM32
cloak behavior remains unverified. See
[the cloak-constructor audit](PLAYER_EGO_CLOAK_CONSTRUCTOR_ARM_2026-10-15.md).

## PlayerEgo supernova-protection constructor (2026-10-14)

Android Thumb `0x99a02–0x99a0a` and `0x99f4a–0x9a048` confirm
the sound ID, effect root/flag and ordered resource children.
Native `gof2`, 206/206 ARM units, the extracted-tail and adjacent
smokes, and 37 verifier tests (one skipped) pass. Focused C1/C2
constructor source-shape/strict fuzzy changes **45.1%/28.3% →
61.0%/35.2%**; neither is exact. Full report
`_work/player_supernova_ctor_full_20261014.json` compares
**4,323/4,524** at **93.40%** average source-shape and **82.29%**
strict fuzzy, with **2,483 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Both exact pair
sets gain and lose no members against
`_work/player_engine_sound_full_20261014.json`. Live ARM32
render/audio remains unverified. See
[the constructor audit](PLAYER_EGO_SUPERNOVA_CONSTRUCTOR_ARM_2026-10-14.md).

## PlayerEgo engine-sound dispatch (2026-10-14)

Android Thumb `0x9a550–0x9a5d0` and `0x9a5ec–0x9a676` confirms
the sound receiver, gain, gates and ship resource passed to
`Player::PlayEngineSound`. Native `gof2`, 206/206 ARM units, the
extracted-method and adjacent smokes, and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy changes
**94.0%/75.5% → 98.5%/87.4%** for `PlayEngineSound` and
**94.7%/76.9% → 98.2%/86.2%** for `StopEngineSound`; neither is exact.
Full report `_work/player_engine_sound_full_20261014.json`
compares **4,323/4,524** at **93.39%** average source-shape and
**82.29%** strict fuzzy, with **2,483 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras** and zero skipped.
Both exact pair sets gain and lose no members against
`_work/player_emergency_start_full_20261014.json`. Live ARM32
audio remains unverified. See
[the engine-sound audit](PLAYER_EGO_ENGINE_SOUND_ARM_2026-10-14.md).

## PlayerEgo emergency-system activation (2026-10-14)

Android Thumb `0xa106c–0xa10f0` and IDA lines 86377–86403 confirm
the activation gates, equipment sort 27 removal, and sound 1115
through `Globals::sound` at gain zero. Native `gof2`, 206/206 ARM
units, the extracted-method and related smokes, and 37 verifier
tests (one skipped) pass. Focused source-shape/strict fuzzy changes
**90.4%/79.2% → 100.0%/100.0%**; it becomes linked-exact, not
byte-exact. Full report `_work/player_emergency_start_full_20261014.json`
compares **4,323/4,524** at **93.39%** average source-shape and
**82.28%** strict fuzzy, with **2,483 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras** and zero skipped. The
linked-exact set gains this function and loses none; the byte-exact
set changes by zero versus
`_work/player_emergency_ctor_full_20261014.json`. Live ARM32 execution
and inactive emergency-geometry ownership remain unverified. See
[the activation audit](PLAYER_EGO_EMERGENCY_START_ARM_2026-10-14.md).

## PlayerEgo emergency-state constructor (2026-10-14)

Android Thumb `0x99946–0x9994c` and `0x99a82–0x99a86` confirms
the six emergency-state zero stores. Native `gof2`, 206/206 ARM
units, extracted-constructor and related smokes, and 37 verifier
tests (one skipped) pass. Focused constructor source-shape/strict
fuzzy changes **44.8%/28.2% → 45.1%/28.3%** for both aliases;
neither is exact. Full report
`_work/player_emergency_ctor_full_20261014.json` compares
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Both exact pair
sets lose and gain zero members against
`_work/player_emergency_full_20261014.json`. Live ARM32 execution
and unused-geometry ownership are unverified. See
[the constructor audit](PLAYER_EGO_EMERGENCY_CONSTRUCTOR_ARM_2026-10-14.md).

## PlayerEgo emergency visual lifecycle (2026-10-14)

Android Thumb `0x9e60c–0x9e748` confirms the late emergency timer,
effect update and expiry teardown. Native `gof2`, 206/206 ARM units,
the production-fragment smoke, cloak/setShip/render smokes and 37
verifier tests (one skipped) pass. Focused `update` source-shape/strict
fuzzy changes **77.0%/31.1% → 79.5%/32.3%**; it is not exact.
Full report `_work/player_emergency_full_20261014.json` compares
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Neither exact pair
set loses or gains a member against
`_work/player_cloak_lifecycle_full_20261014.json`. Live ARM32
rendering and unused-geometry ownership are unverified. See
[the emergency-lifecycle audit](PLAYER_EGO_EMERGENCY_LIFECYCLE_ARM_2026-10-14.md).

## PlayerEgo active cloak lifecycle (2026-10-14)

Android Thumb `0x9cfe2–0x9d002` and `0x9d310–0x9d8a6` confirm
the cloak recharge countdown, fade, visibility, Status duration,
material restoration and expiration. The old emergency-timer
interpretation of `+0x20c` was wrong and is corrected. Native
`gof2`, 206/206 ARM units, two production-fragment smokes and 37
verifier tests (one skipped) pass. Focused `update` source-shape/
strict fuzzy changes **68.2%/20.4% → 77.0%/31.1%**; it is not
exact. Full report `_work/player_cloak_lifecycle_full_20261014.json`
compares **4,323/4,524** at **93.39%** average source-shape and
**82.28%** strict fuzzy, with **2,482 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras**, zero skipped and no
exact-pair losses versus the preceding full report. Live ARM32
rendering and the late emergency visual teardown remain unverified.
See [the cloak-lifecycle audit](PLAYER_EGO_CLOAK_LIFECYCLE_ARM_2026-10-14.md).

## PlayerEgo original material mode (2026-10-14)

Android Thumb `0x9a29a–0x9a2a6` confirms `setShip` stores the
original `Material::materialMode`, not a material-list pointer, at
`PlayerEgo+0x394`. The production-fragment smoke, native `gof2`,
206/206 ARM units and 37 verifier tests (one skipped) pass.
Focused `setShip` source-shape/strict fuzzy changes
**98.2%/91.3% → 97.8%/79.2%**; no exact match is claimed.
Full report `_work/player_original_material_mode_full_20261014.json`
compares **4,323/4,524** at **93.39%** average source-shape and
**82.28%** strict fuzzy, with **2,482 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras**, zero skipped and no
exact-pair losses against `_work/player_cloak_charge_full_20261014.json`.
Live ARM32 rendering is unverified. See
[the material-mode audit](PLAYER_EGO_ORIGINAL_MATERIAL_MODE_ARM_2026-10-14.md).

## PlayerEgo cloak-charge transition (2026-10-14)

Android Thumb `0x9d492–0x9d4c8` confirms frame-time addition to
`cloakCharge`, the strict greater-than threshold, HUD event 29,
`toggleCloaking` and subsequent clearing of `chargingCloak`.
The extended production-fragment smoke, native `gof2`, 206/206
ARM units and 37 verifier tests (one skipped) pass. Focused
`update` source-shape/strict fuzzy changes **67.5%/20.5% →
68.2%/20.4%**; it remains non-exact. Full report
`_work/player_cloak_charge_full_20261014.json` compares
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Both exact pair
sets lose and gain zero members against
`_work/player_update_beams_full_20261014.json`. The smoke does
not cover the whole `update` body or live ARM32 behavior. See
[the cloak-charge audit](PLAYER_EGO_CLOAK_CHARGE_ARM_2026-10-14.md).

## PlayerEgo beam updates (2026-10-14)

Android Thumb `0x9d8fe–0x9d93c` and IDA lines 83966–83981 confirm
the tractor-then-repair dispatch and its frame-time/radar/level/HUD
arguments. The production-fragment smoke, native `gof2`, 206/206
ARM units and 37 verifier tests (one skipped) pass. Focused
`update` source-shape changes **66.1% → 67.5%**, strict fuzzy
**28.6% → 20.5%**, and the body remains non-exact. Full report
`_work/player_update_beams_full_20261014.json` compares
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, and zero skipped. Both exact pair
sets lose and gain zero members against
`_work/player_set_ship_full_20261014.json`. The smoke covers the
changed fragment, not full-method or live ARM32 behavior. See
[the beam-update audit](PLAYER_EGO_BEAM_UPDATE_ARM_2026-10-14.md).

## PlayerEgo setShip emergency and cloak tail (2026-10-14)

Android Thumb `0x9a3f6–0x9a498` and IDA lines 80837–80875 confirm
emergency sort 27 attribute 41 and hull material cloning into
`PlayerEgo+0x388`. The extracted production-fragment smoke, native
`gof2`, 206/206 ARM units and 37 verifier tests (one skipped) pass.
Focused `setShip` source-shape/strict fuzzy changes
**97.4%/90.6% → 98.2%/91.3%**; both instruction counts are 195,
but it is non-exact. Full report
`_work/player_set_ship_full_20261014.json` compares
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras** and zero skipped. Both exact pair
sets lose none against `_work/player_cloak_full_20261014.json`.
The test does not cover the whole ship replacement lifecycle or
live ARM32 behavior. See
[the setShip-tail audit](PLAYER_EGO_SET_SHIP_TAIL_ARM_2026-10-14.md).

## PlayerEgo cloak activation (2026-10-14)

Android Thumb `0x9a708–0x9a9b0` and IDA lines 81068–81226 confirm
cloak attribute 38, amount-limited cargo removal, HUD event 30's
amount, sound through `Globals::sound` and shader channels 1/2.
The extracted production-method smoke, native `gof2`, 206/206 ARM
units and 37 verifier tests (one skipped) pass. Focused
`toggleCloaking` source-shape changes **92.5% → 94.2%** and strict
fuzzy **56.5% → 23.0%**; call-count match reaches **100%**, but
the function is not exact. Full report
`_work/player_cloak_full_20261014.json` compares **4,323/4,524**
at **93.39%** average source-shape and **82.28%** strict fuzzy,
with **2,482 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, and zero skipped. Both exact pair sets lose none
against `_work/player_render_full_20261014.json`. Live ARM32 cloak
behavior remains unverified. See
[the cloak audit](PLAYER_EGO_CLOAK_ARM_2026-10-14.md).

## PlayerEgo render dispatch (2026-10-14)

Android Thumb `0xa1b78–0xa1c56` and IDA lines 87170–87237 confirm
the dead/live branch split, `+0x309` visibility gate, emergency/turret
geometry and moving-stars predicate. The extracted production-method
smoke, native `gof2`, 206/206 ARM translation units and 37 verifier
tests (one skipped) pass. Focused `render` source-shape/strict fuzzy
changes **86.8%/41.4% → 97.2%/60.4%**; no exact claim is made.
Full report `_work/player_render_full_20261014.json` compares
**4,323/4,524**, with **201 missing**, **123 extras**, zero skipped,
**93.39%** average source-shape, **82.29%** strict fuzzy,
**2,482 linked-exact** and **1,065 byte-exact**. Both exact pair
sets lose none against `_work/player_check_turret_full_20261014.json`.
Live ARM32 rendering is unverified. See
[the render audit](PLAYER_EGO_RENDER_ARM_2026-10-14.md).

## PlayerEgo turret construction and stream return (2026-10-14)

Android Thumb and IDA confirm turret geometry at `+0xdc/+0x28/+0x2c/+0x30`,
sort 2, attribute 17, the mesh/offset table, transform-handle parenting,
and cloak materials at `+0x38c/+0x390`. Both `addGun` wrappers tail-call
`checkForTurret`; `dockToStream` reads `+0xe0/+0xe4/+0xe8` and clears
autopilot. The production-method smoke, native `gof2`, 206/206 ARM
units and 37 verifier tests (one skipped) pass. Focused source-shape/
strict fuzzy is **95.2%/57.5%** for `checkForTurret`, **100%/95.2%**
for `dockToStream`, and **100%/100%** for both `addGun` overloads;
none is exact. Full report `_work/player_check_turret_full_20261014.json`
covers **4,323/4,524** at **93.38%** average source-shape and
**82.28%** strict fuzzy, with **2,482 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras** and zero skipped. Exact
pair sets lose none against `_work/player_axis_ramp_full_20261014.json`.
Live ARM32 behavior is unverified. See
[the turret audit](PLAYER_EGO_TURRET_CONSTRUCTION_ARM_2026-10-14.md).

## PlayerEgo pitch/yaw ramps (2026-10-14)

IDA and Android Thumb confirm the cursor-mode yaw cap of 280, option
float selection through `Globals::options+0x11`, direction-specific
pitch sensitivity factors and base-handling `yawRamp` store on right
input. The production-helper smoke, native `gof2`, 206/206 ARM units
and 37 verifier tests (one skipped) pass. Focused average
source-shape/strict fuzzy for `right`, `left`, `down` and `up` changes
**80.3%/58.9% → 88.0%/69.8%**; none is exact. Full report
`_work/player_axis_ramp_full_20261014.json` covers **4,323/4,524**
at **93.37%** average source-shape and **82.26%** strict fuzzy,
with **2,482 linked-exact**, **1,065 byte-exact**, **123 extras**,
**201 missing** and zero skipped. Linked- and byte-exact pair sets
have no losses against
`_work/player_approach_asteroid_stage0_full_20261014.json`. Live
input feel and ARM32 runtime remain unverified. See
[the axis-ramp audit](PLAYER_EGO_AXIS_RAMP_ARM_2026-10-14.md).

## PlayerEgo asteroid approach stage zero (2026-10-14)

IDA/Ghidra and Android Thumb `0x9f5ac–0x9f8de` confirm the stage-zero
distance, movement, camera entry and alignment behavior. The
production-body asteroid smoke, native `gof2`, 206/206 ARM units and
37 verifier tests (one skipped) pass. Focused `approachAsteroid`
source-shape/strict fuzzy changes **40.8%/21.6% → 91.9%/47.3%**;
it is not exact. Full report
`_work/player_approach_asteroid_stage0_full_20261014.json` covers
**4,323/4,524** at **93.37%** average source-shape and **82.26%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, **201 missing** and zero skipped. Linked- and
byte-exact pair sets have no losses against
`_work/player_dock_point_full_20261014.json`. Live asteroid approach
and ARM32 runtime remain unverified. See
[the asteroid-approach audit](PLAYER_EGO_ASTEROID_APPROACH_STAGE0_ARM_2026-10-14.md).

## PlayerEgo docking-point ease (2026-10-14)

IDA/Ghidra and Android Thumb `0x9fa6a–0x9fb5e` confirm the canvas and
level arguments, look-at camera mode, translated destination matrix,
and 3,000 ms ease construction. The production-body dock-point smoke,
native `gof2`, 206/206 ARM translation units and 37 verifier tests
(one skipped) pass. Focused `dockToDockingPoint` source-shape/strict
fuzzy changes **77.7%/54.3% → 97.7%/66.4%**; it is not exact.
Full report `_work/player_dock_point_full_20261014.json` covers
**4,323/4,524** at **93.36%** average source-shape and **82.25%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, **201 missing** and zero skipped. Linked- and byte-exact
pair sets have no losses against
`_work/player_update_maneuver_full_20261014.json`. Live docking and
ARM32 runtime remain unverified. See
[the docking-point ease audit](PLAYER_EGO_DOCK_POINT_EASE_ARM_2026-10-14.md).

## PlayerEgo maneuver update (2026-10-14)

IDA/Ghidra and Android Thumb `0x9c1f8–0x9c684` confirm both dodge
directions, the 1,200 ms fade/reset and the 3,000 ms strafe/camera
path. Production-body maneuver smokes, native `gof2`, 206/206 ARM
translation units and 37 verifier tests (one skipped) pass. Focused
`updateManeuver` source-shape/strict fuzzy changes **30.1%/20.0% →
93.0%/51.7%**; `initManeuver` is **99.4%/89.4%**. Neither is exact.
Full report `_work/player_update_maneuver_full_20261014.json` covers
**4,323/4,524** at **93.35%** average source-shape and **82.25%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, **201 missing** and zero skipped. Linked- and byte-exact
pair sets have no losses against
`_work/player_move_to_position_full_20261014.json`. Live maneuver
animation and ARM32 runtime remain unverified. See
[the maneuver audit](PLAYER_EGO_UPDATE_MANEUVER_ARM_2026-10-14.md).

## PlayerEgo move-to-position (2026-10-14)

IDA/Ghidra and Android Thumb `0x9c6d4–0x9ca52` confirm heading blend,
signed steering/history, forward and rotated strafe movement, and the
soft-float return value. The constructor and `setAutoPilot` use the
same history offsets. The production-body navigation smoke, native
`gof2`, 206/206 ARM translation units and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy for `moveToPosition`
changes **43.8%/22.2% → 92.9%/48.0%**; it is not exact. Full report
`_work/player_move_to_position_full_20261014.json` covers
**4,323/4,524** original functions at **93.34%** average source-shape
and **82.24%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped. The linked- and
byte-exact pair sets have no losses against
`_work/player_handle_ship_full_20261014.json`. Live flight and ARM32
runtime remain unverified. See
[the move-to-position audit](PLAYER_EGO_MOVE_TO_POSITION_ARM_2026-10-14.md).

## PlayerEgo ship control (2026-10-14)

IDA/Ghidra and Android Thumb `0x9bdf0–0x9c12c` confirm the two sound
parameters, rotation and roll, forward and strafe movement, camera
translation, transform composition and accumulator resets. The
production-body `handleShip` and turret-view smokes, native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped)
pass. Focused `handleShip` source-shape/strict fuzzy changes
**40.8%/23.7% → 92.1%/60.4%**; the corrected `handleTurretView` reaches
**97.9%/80.8%**. Neither is exact. Full report
`_work/player_handle_ship_full_20261014.json` covers **4,323/4,524**
original functions at **93.33%** average source-shape and **82.23%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras** and zero skipped. The linked- and byte-exact pair sets
have no losses against `_work/player_roll_full_20261014.json`. Live
ship movement, sound and ARM32 runtime remain unverified. See
[the ship-control audit](PLAYER_EGO_HANDLE_SHIP_ARM_2026-10-14.md).

## PlayerEgo roll (2026-10-14)

IDA/Ghidra and Android Thumb `0x9bbf8–0x9bd94` confirm the roll
matrix inputs, rate literals, direction and autolevel flags, and the
`MatrixSetRotation` call contract. The production-body host smoke,
native `gof2`, 206/206 ARM translation units and 37 verifier tests
(one skipped) pass. Focused source-shape/strict fuzzy changes
**60.8%/38.6% → 92.2%/67.5%**. The method is not exact. Live
animation and ARM32 runtime remain unverified. Full report
`_work/player_roll_full_20261014.json` covers **4,323/4,524**
original functions at **93.32%** average source-shape and **82.23%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras** and zero skipped. Exact-pair sets have no losses or
gains against `_work/player_turret_view_full_20261014.json`. See
[the roll audit](PLAYER_EGO_ROLL_ARM_2026-10-14.md).

## PlayerEgo turret-view camera (2026-10-14)

IDA/Ghidra and Android Thumb `0x9b96c–0x9bbb0` confirm the movement
gates, composed camera matrix, randomized shake, camera handle and
Player transform assignment. The production-body host smoke, native
`gof2`, 206/206 ARM translation units and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy changes
**80.8%/59.3% → 97.8%/80.4%**. The method is not exact. Live camera
behavior and ARM32 runtime remain unverified. Full report
`_work/player_turret_view_full_20261014.json` covers **4,323/4,524**
original functions at **93.31%** average source-shape and **82.22%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras** and zero skipped. Exact-pair sets have no losses or
gains against `_work/player_auto_turret_aim_full_20261014.json`. See
[the turret-view audit](PLAYER_EGO_TURRET_VIEW_ARM_2026-10-14.md).

## PlayerEgo auto-turret aim/fire (2026-10-14)

Android IDA/Ghidra and Thumb `0x9cb70–0x9cde4` establish the
post-selection target lead, geometry rotation, weapon-2 firing and
timeout. Thumb `0x9caf6` reads `noTargetFlag` at `+0x74`, now matched
by the 32-bit header layout. The production-body smoke, native
`gof2`, 206/206 ARM translation units and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy rises
**45.7%/24.0% → 93.8%/54.1%**. Full report
`_work/player_auto_turret_aim_full_20261014.json` covers
**4,323/4,524** original functions at **93.31%** average
source-shape and **82.22%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped. Exact-pair
sets have no losses against the selection baseline report
`_work/player_auto_turret_full_20261014.json`. Live aiming and ARM32
runtime remain unverified. See
[the aim/fire audit](PLAYER_EGO_AUTO_TURRET_AIM_ARM_2026-10-14.md).

## PlayerEgo auto-turret selection (2026-10-14)

Android Thumb `0x9cac8–0x9cb32` confirms the virtual enemy-position
call, subtraction from PlayerEgo `+0x148`, `VectorLength`, integer
distance and nearest-target comparison. Thumb `0x9cde0–0x9cde4`
confirms `stopShooting(2)`. The production-body host smoke checks
target filters, nearest target, distance boundary and stop argument;
native `gof2`, 206/206 ARM units and 37 verifier tests (one skipped)
pass. Focused source-shape/strict fuzzy changes **45.7%/23.5% →
45.7%/24.0%**. Full report
`_work/player_auto_turret_full_20261014.json` covers **4,323/4,524**
functions at **93.30%** average source-shape and **82.21%** strict
fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**, **123
extras** and zero skipped. Exact-pair and extra-symbol sets have no
losses or gains against `_work/player_dock_vector_full_20261014.json`.
The aim/fire tail and ARM32 runtime remain unverified. See
[the selection audit](PLAYER_EGO_AUTO_TURRET_SELECTION_ARM_2026-10-14.md).

## PlayerEgo dock-vector reset (2026-10-14)

Raw Android Thumb `0x9f4c8–0x9f4e2` and `0x9fc0e–0x9fc2c` shows
both undock paths assigning a zero `Vector` to PlayerEgo `+0x1c8`.
The header overlays that vector with the three existing integer
fields, with ARM offset assertions. The production-method docking
smoke, native `gof2`, 206/206 ARM units and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy remains
**99.4%/88.2%** for asteroid docking and **77.7%/54.3%** for
docking-point docking. Full report
`_work/player_dock_vector_full_20261014.json` covers **4,323/4,524**
functions at **93.30%** average source-shape and **82.21%** strict
fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**, **123
extras** and zero skipped. Exact-pair and extra-symbol sets have no
losses or gains against `_work/player_maneuver_full_20261014.json`.
Live docking and ARM32 runtime remain unverified. See
[the vector audit](PLAYER_EGO_DOCK_VECTOR_ARM_2026-10-14.md).

## PlayerEgo maneuver target (2026-10-14)

Android `initManeuver` at `0xa1abc` loads the `10000.0f` literal at
`0xa1b70` for the type-3 strafe target. Its recovered body uses real
`AEMath` operators in the original call order, avoiding declared-only
`Vec_*` wrappers. The production-body host smoke, native `gof2`,
206/206 ARM units and 37 verifier tests (one skipped) pass. Focused
source-shape/strict fuzzy improves **93.1%/78.5% → 99.4%/89.4%**,
without an exact match. Full report
`_work/player_maneuver_full_20261014.json` covers **4,323/4,524**
functions at **93.30%** average source-shape and **82.21%** strict
fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**, **123
extras** and zero skipped. Exact-pair and extra-symbol sets are
unchanged from `_work/starsystem_flare_full_20261014.json`. The full
run used one worker after an eight-worker attempt hit Windows page-file
error 1455. Live maneuver control and ARM32 runtime remain unverified.
See [the maneuver audit](PLAYER_EGO_MANEUVER_ARM_2026-10-14.md).

## StarSystem flare handoff (2026-10-14)

Android `StarSystem::render2D` at `0x1354e8` reads camera state from
`Globals::Canvas` and gates `LensFlare::render2D` on successful screen
projection. A production-body host smoke checks both canvas roles,
the failed-projection no-draw path and a successful projected draw.
Native `gof2`, 206/206 ARM units and 37 verifier tests (one skipped)
pass. Focused source-shape/strict fuzzy changes **95.6%/67.2% →
98.6%/64.4%**; no exact match. Full report
`_work/starsystem_flare_full_20261014.json` covers **4,323/4,524**
functions at **93.29%** average source-shape and **82.21%** strict
fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**, **123
extras** and zero skipped. Exact-pair and extra-symbol sets have no
losses or gains against `_work/lens_flare_render_full_20261014.json`.
Real GL, assets and ARM32 runtime remain unverified. See
[the handoff audit](STARSYSTEM_FLARE_HANDOFF_ARM_2026-10-14.md).

## LensFlare render2D recovery (2026-10-14)

Android `render2D` at `0x11d088` establishes seven image draws,
per-pass canvas/color/size arguments and a final input-canvas
color-preserving fill. The extracted production-body host smoke
checks center/displaced output and no-draw guards. Native `gof2`,
206/206 ARM units and 37 verifier tests (one skipped) pass. Focused
source-shape/strict fuzzy changes **64.2%/15.5% → 94.8%/48.7%**;
the function is not exact. Full report
`_work/lens_flare_render_full_20261014.json` covers **4,323/4,524**
functions at **93.29%** average source-shape and **82.21%** strict
fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**, **123
extras** and zero skipped. Exact-pair and extra-symbol sets have no
losses or gains against `_work/lens_flare_ctor_full_20261014.json`.
Real GL, camera integration and ARM32 runtime remain unverified. See
[the render audit](LENS_FLARE_RENDER_ARM_2026-10-14.md).

## LensFlare constructor canvas binding (2026-10-14)

Android constructor `0x11d00c` uses `Globals::Canvas` for resource
IDs 1288–1290 and its input canvas for width/height. The extracted
production-body host smoke checks both canvas roles, all handles and
the retained pointer. Native `gof2`, 206/206 ARM units and 37 verifier
tests (one skipped) pass. Both C1/C2 aliases improve from **91.0%/
46.2%** source-shape/strict fuzzy to **100%/100%** and become
linked-exact, not byte-exact. Full report
`_work/lens_flare_ctor_full_20261014.json` covers **4,323/4,524**
functions at **93.29%** average source-shape and **82.20%** strict
fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**, **123
extras** and zero skipped. Compared with
`_work/system_pathfinder_full_20261014.json`, no exact pair is lost;
the C1/C2 constructor pairs are newly linked-exact. `render2D`, real
GL and ARM32 runtime are unverified. See
[the constructor audit](LENS_FLARE_CONSTRUCTOR_ARM_2026-10-14.md).

## SystemPathFinder route state and node release (2026-10-14)

Android `getSystemPath` at `0x11b7a8` reads `Globals::status` and, on
success, releases its allocated node array with
`ArrayReleaseClasses<Node *>`. The production-body host smoke checks
route construction, jump distance and visibility filtering. Native
`gof2`, 206/206 ARM units and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy changes **95.4%/67.6% →
96.0%/59.1%**; it is not exact. Full report
`_work/system_pathfinder_full_20261014.json` covers **4,323/4,524**
functions at **93.28%** average source-shape and **82.18%** strict
fuzzy, with **2,480 linked-exact**, **1,065 byte-exact**, **123 extras**
and zero skipped. Exact-pair comparison with
`_work/menutouch_preview_level_full_20261014.json` finds zero losses,
one new linked-exact `ArrayReleaseClasses<Node *>` pair and no
byte-exact or extra-set changes. Repeated-query memory behavior and
ARM32 runtime are unverified. See
[the pathfinder audit](SYSTEM_PATH_FINDER_ROUTE_ARM_2026-10-14.md).

## Save-preview level label (2026-10-14)

Android `MenuTouchWindow::createRecordButtons` at `0x125454`, IDA lines
162752–162766 and Thumb `0x125730–0x12576e`, establish GameText ID 321,
`: ` and `String(int)` from `GameRecord+0x20`. The production-expression
smoke checks 0, 7 and 123; native `gof2`, 206/206 ARM units and verifier
tests (37 run, one skipped) pass. Focused source-shape/strict fuzzy
improves **89.5%/43.6% → 89.9%/44.4%**. Full report
`_work/menutouch_preview_level_full_20261014.json` covers
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**, **1,065
byte-exact**, **123 extras** and zero skipped. Exact-pair and extra
sets have no losses or gains against
`_work/layout_save_menu_metrics_full_20261014.json`. Complete save/load
UI interaction and ARM32 runtime remain unverified. See
[the label audit](MENUTOUCH_PREVIEW_LEVEL_ARM_2026-10-14.md).

## Layout save-menu metrics (2026-10-14)

Android `Layout::Layout()` at `0xd1028`, IDA lines 128010–128013,
128456–128459 and 128536–128556, plus Thumb stores at
`0xd117e–0xd118c`, `0xd1a8c–0xd1a94` and `0xd1be4–0xd1c44`,
establish seven `+0x294..+0x2ac` words copied by `MenuTouchWindow`.
The production constructor smoke checks seven profiles; native
`gof2`, 206/206 ARM units, three Layout/TouchButton smokes and 37
verifier tests (one skipped) pass. Focused C1/C2 source-shape rises
**41.0% → 43.2%**; strict fuzzy falls **9.5% → 9.0%**. Full report
`_work/layout_save_menu_metrics_full_20261014.json` covers
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact
pair sets have no losses or gains against
`_work/layout_wanted_width_metrics_full_20261014.json`. Linked
ARM32 UI behavior and asset-backed visuals are unverified. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout Wanted-window width metrics (2026-10-14)

Android `Layout::Layout()` at `0xd1028`, IDA lines 128000–128005,
128414–128453 and 128473–128554, plus Thumb stores at `0xd1a46`,
`0xd1b9e` and `0xd1c34`, establish five `+0xa0..+0xac/+0xcc`
words. The production constructor smoke checks seven profiles;
native `gof2`, 206/206 ARM units, three Layout/TouchButton smokes and
37 verifier tests (one skipped) pass. Focused C1/C2 source-shape/
strict fuzzy rises **39.9%/9.1% → 41.0%/9.5%**. Full report
`_work/layout_wanted_width_metrics_full_20261014.json` covers
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact
pair sets have no losses or gains against
`_work/layout_late_window_metrics_full_20261014.json`. Linked
ARM32 UI behavior and asset-backed visuals are unverified. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout late-window metrics (2026-10-14)

Android `Layout::Layout()` at `0xd1028`, IDA lines 127985–127993,
128065–128069 and 128254–128355, plus Thumb stores at
`0xd1122–0xd1128` and `0xd1786–0xd18b6`, establish six
`+0x2c0/+0x2c4/+0x2cc..+0x2d8` words. The production constructor
smoke checks seven platform profiles. Native `gof2`, 206/206 ARM
units, three Layout/TouchButton smokes and 37 verifier tests (one
skipped) pass. Focused C1/C2 source-shape rises **38.0% → 39.9%**;
strict fuzzy falls **9.7% → 9.1%**. Full report
`_work/layout_late_window_metrics_full_20261014.json` covers
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact
pair sets have no losses or gains against
`_work/layout_hangar_choice_metrics_full_20261014.json`. Linked
ARM32 UI behavior and asset-backed visuals are unverified. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout Hangar/Choice metrics (2026-10-14)

Android `Layout::Layout()` at `0xd1028`, IDA lines 129089–129103,
129126–129130 and 129193–129197, plus Thumb stores at
`0xd23cc–0xd23fe`, establish ten `+0x254..+0x278` words consumed by
`HangarWindow` and `ChoiceWindow`. The production constructor smoke
checks six platform profiles. Native `gof2`, 206/206 ARM units, three
Layout/TouchButton smokes and 37 verifier tests (one skipped) pass.
Focused C1/C2 source-shape rises **35.8% → 38.0%**; strict fuzzy
moves **9.8% → 9.7%**. Full report
`_work/layout_hangar_choice_metrics_full_20261014.json` covers
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact
pair sets have no losses or gains against
`_work/layout_save_metrics_full_20261014.json`. Linked ARM32
UI behavior and asset-backed visuals are unverified. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout save-list metrics (2026-10-14)

Android `Layout::Layout()` at `0xd1028`, IDA lines 128014–128017,
128460–128463 and 128473–128581, plus raw Thumb's `+0x108/+0x118`
vector stores, establish eight previously zero-filled slots through
`+0x128`. `MenuTouchWindow` consumes the first four for save-list
placement. The production constructor smoke checks six platform
profiles; native `gof2`, 206/206 ARM units, three Layout/TouchButton
smokes and 37 verifier tests (one skipped) pass. Focused C1/C2
source-shape/strict fuzzy improves **34.3%/9.5% → 35.8%/9.8%**;
neither is exact. Full report `_work/layout_save_metrics_full_20261014.json`
covers **4,323/4,524** functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets have no losses or gains against
`_work/layout_window_touch_metrics_full_20261014.json`. Live
save/load UI, asset-backed visuals and ARM32 runtime are unverified.
See [the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout window and touch metrics (2026-10-14)

Android `Layout::Layout()` at `0xd1028`, IDA lines 127981–128083
and 128174–128381, and raw Thumb's packed stores restore eleven
previously zero-filled `+0x5c..+0x88` slots across six platform
profiles. The extracted production constructor smoke checks every
profile; native `gof2`, 206/206 ARM units, three Layout/TouchButton
smokes and 37 verifier tests (one skipped) pass. Focused C1/C2
source-shape/strict fuzzy improves **30.9%/7.6% → 34.3%/9.5%**;
neither is exact. Full report
`_work/layout_window_touch_metrics_full_20261014.json` covers
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact
pair sets have no losses or gains against
`_work/player_dock_glide_full_20261014.json`. Visual behavior with
assets and ARM32 runtime is not verified. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## PlayerEgo docking glide and departure (2026-10-14)

Raw Android Thumb `0xa0050–0xa039c` establishes state-2 docking-point
distance, state-3 navigation-point distance, integer truncation before
the 200-unit/radius thresholds, and the common EaseInOutMatrix and
geometry update. Literal rates are `0.0006f` for approach and
`0.0012f` for departure. The extracted whole-method smoke checks the
thresholds, follow rates, frame-time advance and matrix propagation.
Native `gof2`, 206/206 ARM translation units, 37 verifier tests (one
skipped), and four PlayerEgo smokes pass. Focused
`approachDockingPoint` source-shape/strict fuzzy rises
**52.2%/27.1% → 69.9%/30.7%**; it remains non-exact. Full report
`_work/player_dock_glide_full_20261014.json` covers **4,323/4,524**
functions at **93.28%** average source-shape and **82.18%** strict
fuzzy, with **2,479 linked-exact**, **1,065 byte-exact**, **123 extras**,
and zero skipped units. Pairwise exact sets have no losses or gains
against `_work/player_dock_ease_entry_full_20261014.json`. Linked ARM
gameplay remains unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking ease-matrix entry (2026-10-14)

Raw Android Thumb `0x9fdf4–0xa001a`, IDA and Ghidra establish the
old ease-object destruction, nearest docking-point query, rotated
basis, 2000 ms EaseInOutMatrix constructor, sound `0x8de` 3D update,
and engine-sound stop. The extracted whole-method smoke checks the
constructor range/basis, ownership and audio. Native `gof2`, 206/206
ARM units, 37 verifier tests (one skipped), and four PlayerEgo smokes
pass. Focused `approachDockingPoint` source-shape/strict fuzzy rises
**31.2%/16.1% → 52.2%/27.1%**; it is not exact. Full report
`_work/player_dock_ease_entry_full_20261014.json` covers
**4,323/4,524** original functions at **93.27%** average
source-shape and **82.17%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets have no losses or gains against
`_work/player_dock_nav_full_20261014.json`. The glide/easing update
shims and linked ARM gameplay remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking navigation and approach speed (2026-10-14)

Raw Android Thumb `0x9fd56–0x9fdc4` confirms the local-to-world
SpacePoint calculation and 12-byte store at PlayerEgo `+0xec`.
IDA/Ghidra confirm integer distance, steering-enabled
`moveToPosition`, handling-plus-2.7 speed capped at 4, and near-target
threshold 19,999. Extracted whole-method smoke covers the point,
speed cap, and distance boundaries. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `approachDockingPoint` source-shape/strict fuzzy rises
**22.2%/15.3% → 31.2%/16.1%**. Full-corpus exact comparison is
in `_work/player_dock_nav_full_20261014.json`:
**4,323/4,524** original functions, **93.27%** average source-shape,
**82.17%** strict fuzzy, **2,479 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped units. Exact pair comparison against
`_work/player_approach_docking_full_20261014.json` finds no losses
or gains. At that checkpoint the ease-matrix constructor and glide
shims were not verified; the following pass recovers the constructor.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking approach and exit transitions (2026-10-14)

Android Thumb `0x9fcb8` and IDA/Ghidra cross-checks establish the
state-0 `+0x1d8` approach threshold, state-2 immediate true return
and `+0xb2` clear on arrival, and state-3 target/flag cleanup with
a false return. The extracted whole-method smoke checks threshold
boundaries, near-target and camera flags, return values, Radar and
SpacePoint cleanup, and HackingGame destruction. Native `gof2`,
206/206 ARM translation units, 37 verifier tests (one skipped), and
four PlayerEgo smokes pass. Focused source-shape/strict fuzzy for
`approachDockingPoint` rises **20.5%/12.1% → 22.2%/15.3%**.
Full report `_work/player_approach_docking_full_20261014.json`:
**4,323/4,524** original functions, **93.27%** average source-shape,
**82.17%** strict fuzzy, **2,479 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped units. Exact pair comparison against
`_work/player_delivery_full_20261014.json` has zero losses and
gains. At this checkpoint the approach function was not exact and
its movement/easing shims remained unresolved; linked ARM gameplay
is still unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking delivery and transfer ABI (2026-10-14)

Android `PlayerEgo::update` at `0x9dda8` onward and
`approachDockingPoint` at `0x9fcb8` establish the docking transfer
state and side effects. ARM data at `0x9ded4/0x9ded8` gives 1000/1500
ms intervals. The extracted production-fragment smoke tests target and
state gates, transfer initialization, goods/passenger/campaign
branches, event boundaries, and hacking-game selection. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and four PlayerEgo
smokes pass. Focused source-shape/strict fuzzy changes:
`update` **57.1%/27.6% → 66.1%/30.3%**,
`approachDockingPoint` **20.4%/10.2% → 20.5%/12.1%**. Neither
function is exact. Full report `_work/player_delivery_full_20261014.json`:
**4,323/4,524** original functions covered, **93.27%** average
source-shape, **82.17%** strict fuzzy, **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped units. Exact-pair
comparison against `_work/player_subsystems_full_20261014.json` finds
zero losses and gains. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cloak recharge timer and hacking update (2026-10-14)

Android Thumb `0x9cfe2–0x9d19e` confirms the cloak recharge timer and
the win/reward/cleanup paths after `HackingGame::update`. The five
reward IDs come from Android `.rodata` at `0x1fba34`. Extracted
production-fragment smoke covers the timer crossing, normal hacking,
win without reward, first flagged enemy cleanup, and a null enemy
list. This checkpoint used the wrong C++ field for that countdown;
the later cloak-lifecycle package corrects it. Native `gof2`,
206/206 ARM units, 37 verifier tests (one
skipped), and four PlayerEgo smokes pass. Focused `update`
source-shape/strict fuzzy changes **55.0%/19.0% → 57.1%/27.6%**.
Full report `_work/player_subsystems_full_20261014.json` covers
**4,323/4,524** original functions at **93.26%** average
source-shape and **82.17%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair comparison with `_work/player_boost_full_20261014.json` finds
zero losses and one linked-exact gain (`Array<int>::~Array()`).
The function remains non-exact; linked ARM gameplay and the later
delivery/docking completion branch remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo boost update (2026-10-14)

Raw Android Thumb `0x9ce92–0x9ceba` establishes the timer's negative
cooldown crossing; `0x9d1d6–0x9d310` establishes the volatile-cargo
shake rates, per-nozzle particle life, strict end threshold, post-effect
disable, and conditional boost-parameter reset. The production-fragment
smoke tests these branches. Native `gof2`, 206/206 ARM translation units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass. Focused
`update` source-shape/strict fuzzy changes **52.3%/19.0% →
55.0%/19.0%**. Full report `_work/player_boost_full_20261014.json`:
**4,323/4,524** original functions covered, **93.26%** average
source-shape, **82.16%** strict fuzzy, **2,478 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped units, and no
linked-/byte-exact pair losses or gains against
`_work/player_update_tail_full_20261014.json`. No whole-function
exact or linked ARM gameplay is claimed; at this checkpoint
`PE_upd_subsystems` and `PE_upd_docksFinishDelivery` remained
unresolved. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo death/explosion update tail (2026-10-14)

Android Thumb `0x9edf0–0x9ef92` and IDA lines 84683–84765 show the
remaining `PlayerEgo::update` path after cursor projection. The copy
of static `crosshairPos` to PlayerEgo `+0x94` follows the camera
handoff. When `isDead()` and explosion at `+0x8c` are both true, the
path calls `Player::stopShooting(0)`, conditionally calls
`FModSound::stop(0)` for nonzero `+0x1b4/+0x1b8`, and rotates the
geometry at `+0x4` by 0.03 on each axis unless docked at point 1.
The primary explosion starts when `explosionTimer + frameTime` first
reaches 3000, is not updated at exactly 3000, and is updated after
that value. The secondary explosion at `+0x90` updates whenever
present; the timer at `+0x2f8` then advances by `+0x134`. The start
also disables emission/render for `currentSystem` at `+0x2fc` and
emits the level `+0x3c` particle using the previously cached camera
position/direction. Finally, turret mode at `+0x170` composes local
matrices for geometry `+0x8` then `+0x4` and writes geometry `+0x2c`.
The old unresolved `PE_upd_post` call is removed; only the ARM
stack-protector epilogue follows this path.

The extracted production-fragment smoke tests alive/null-explosion
skips, death sound and rotation gates, the 3000 ms start/update
boundary, secondary update, particle calls, and turret matrix order.
Native `gof2`, 206/206 ARM units, 37 verifier tests (one skipped),
and four PlayerEgo smokes pass. Focused `update` source-shape/strict
fuzzy changes **48.2%/24.8% → 52.3%/19.0%**. Full report
`_work/player_update_tail_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**, zero
skipped units. Pairwise comparison with
`_work/player_crosshair_active_full_20261014.json` finds zero
linked-/byte-exact losses or gains. The drop in strict fuzzy and the
absence of linked ARM gameplay are explicit limits; earlier unresolved
`update` helpers remain. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo active-cursor aim and steering (2026-10-14)

Raw Android Thumb at `0x9eb84–0x9eda6` and IDA lines 84609–84682
establish the active-cursor path: center is `w >> 1, h >> 1`, the
per-axis limit is center × the binary's double literal 0.7 then
converted to float, and each mouse delta is clamped to its signed
limit before integer conversion. The clamped integers replace
`Globals::mouseDeltaX/Y`; the displayed crosshair is bounded to
center ± limit. Positive/negative horizontal deltas call
`PlayerEgo::right/left`, and positive/negative vertical deltas call
`down/up`, with magnitude divided by its axis limit and the current
frame time passed through. The path projects the same normalized
camera direction × 22000 as the passive path, but stores the result
in static `crosshairShootPos`. A nonpositive projected depth blends
that result 0.2/0.8 with PlayerEgo `+0xa0`, which receives the final
shoot position. Constructor IDA at 0x998cc, lines 80099 onward,
zeros the two 12-byte previous-position vectors at `+0x94/+0xa0`.
The iOS dump did not yield a reliable counterpart for this block.

The extracted production-fragment smoke covers both steering signs,
limits, shoot projection, negative-depth smoothing, passive gate
overrides, and turret matrix order. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape/strict fuzzy changes
**42.4%/10.9% → 48.2%/24.8%**; constructor C1/C2 aliases change
**40.3%/24.8% → 41.9%/25.2%**. Full report
`_work/player_crosshair_active_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.17%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**, zero
skipped units. Exact pair comparison against
`_work/player_crosshair_passive_full_20261014.json` finds zero
losses and gains. No whole-function exact result or linked ARM
gameplay is claimed. At this checkpoint the previous-position store
was still early and the update tail incomplete; the following pass
corrected both.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo passive crosshair projection (2026-10-14)

Android ARM `PlayerEgo::update` at `0x9ea32–0x9eda6` gets position and
direction from `Player::transformMatrix` at Player `+0x4`. For an active
turret, it obtains the geometry matrices at PlayerEgo `+0x2c` then
`+0x28`, multiplies them in that order, and replaces the camera
position/direction. The passive-path predicate is
`!mouseCursorActivated || arg6 || arg7 == 3 || (arg7 != 1 && autoPilot)`;
autopilot is at PlayerEgo `+0x158`. The normalized direction is scaled
by the ARM literal 22000.0f, added to the position, and projected by
`PaintCanvas::GetScreenPosition` into static `crosshairPos`. If depth
is nonpositive, the projected position is blended with the previous
position at `+0x94` using ARM literals 0.2f and 0.8f. IDA lines
84563–84605, Ghidra lines 64543–64580, and raw Thumb agree on these
calls, offsets, literals, and gates. The extracted production-fragment
smoke tests ordinary aim, negative-depth smoothing, predicate gates,
and turret multiplication order. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.

Focused `update` source-shape/strict fuzzy changes
**37.9%/12.7% → 42.4%/10.9%**. Full report
`_work/player_crosshair_passive_full_20261014.json` covers
**4,323/4,524** original functions at **93.26%** average source-shape
and **82.16%** strict fuzzy, with **2,478 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets have zero losses and gains versus
`_work/player_cursor_camera_full_20261014.json`. The current source
stores the previous position immediately after the passive projection;
the original stores it after the subsequent cursor/camera work. That
intervening active-mouse steering, shoot-position path, and linked
ARM gameplay remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cursor/camera handoff (2026-10-14)

Original ARM `update` at `0x9edaa–0x9edf0` tests the byte at
`PlayerEgo+0x84`, calls `TargetFollowCamera::isInFastForwardMode` via
the pointer at `+0x88`, and only when fast-forward is clear calls
`setShipHandling` with the float at `+0x154` for an active cursor or
`resetShipHandling` otherwise. It then clears `+0x84`. Regardless of
the fast-forward gate, it stores Boolean `Globals::mouseCursorActivated`
to the byte at `+0x378`. Android IDA lines 84683–84697, independent
Ghidra lines 64668 onward, and raw Thumb instructions corroborate
the sequence and offsets. The extracted production-fragment smoke
checks no-change, deferred fast-forward, apply/reset, and Boolean
normalization. Native `gof2`, 206/206 ARM units, 37 verifier tests
(one skipped), and four PlayerEgo smokes pass. Focused `update`
source-shape/strict fuzzy changes **37.0%/21.5% → 37.9%/12.7%**;
the strict-fuzzy regression is recorded rather than optimized away.
Full report `_work/player_cursor_camera_full_20261014.json`:
**4,323/4,524** covered, **93.26%** source-shape, **82.16%** strict
fuzzy, **2,478 linked-exact**, **1,065 byte-exact**, **123 extras**.
Pair-by-pair comparison with `_work/player_hit_projection_full_20261014.json`
finds zero linked- or byte-exact losses and gains. The camera/crosshair
calculations between hit projection and this handoff are not yet
recovered; linked ARM gameplay is unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo hit-direction projection (2026-10-14)

Android ARM `update` at `0x9e94c–0x9ea32` calls
`Player::getHitVector`, `VectorNormalize`, vector × 5000, vector +
`PlayerEgo+0x148`, and `PaintCanvas::GetScreenPosition` in that
order. The resulting `Vector.x` is compared with zero, screen width,
and ±15,000/50,000; `Vector.z` selects the top/bottom flag path.
IDA lines 84518–84562, independent Ghidra lines 64494–64543, and
the raw Thumb instructions agree. `Hud::draw` consumes the resulting
bitfield at `PlayerEgo+0x20`. The extracted production-fragment smoke
tests no-damage suppression, the normalized world point, off-screen
thresholds, and negative depth. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape/strict fuzzy is **37.0%/21.5%** versus
**34.0%/10.5%**. Full report
`_work/player_hit_projection_full_20261014.json`:
**4,323/4,524** covered, **93.26%** source-shape, **82.16%** strict
fuzzy, **2,478 linked-exact**, **1,065 byte-exact**, **123 extras**.
Compared pair-by-pair with
`_work/player_hit_cursor_layout_full_20261014.json`, there are zero
linked- or byte-exact losses and gains. This does not validate the
remaining camera/crosshair branch or linked ARM gameplay. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cursor/damage state and tail ABI (2026-10-14)

Android ARM `PlayerEgo::update` at `0x9e86a–0x9e94c` resets the hit
direction word at `+0x20` and shield flash byte at `+0x30a`, compares
the cursor byte at `+0x378` against `Globals::mouseCursorActivated`,
then checks combined HP and the computer/freeze gates before calling
`Hud::playerHit` and `hitCamera`. Its shield/armor/hull flags at Player
`+0x64/+0x65/+0x66` drive sounds 25/23/24 with one branch cleared per
hit. Android IDA lines 84488 onward and Ghidra lines 64459 onward
corroborate the sequence. Independent Android ARM docking, strafe, and
render accessors confirm `SpacePoint` at `+0x36c`, acceleration/factor
at `+0x37c/+0x380`, and cloak materials at `+0x388/+0x38c/+0x390`.
The extracted production-fragment smoke exercises cursor mismatch,
combined-HP reduction, computer/freeze suppression, sound priority,
and flag clearing. Native `gof2`, all 206 ARM translation units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape/strict fuzzy is **34.0%/10.5%** versus
**31.4%/10.8%**. Full report
`_work/player_hit_cursor_layout_full_20261014.json`:
**4,323/4,524** covered, **93.26%** source-shape, **82.16%** strict
fuzzy, **2,478 linked-exact**, **1,065 byte-exact**, **123 extras**;
against `_work/player_visual_prelude_full_20261014.json`, no linked
or byte exact pairs are lost or gained. Neither the remaining
hit-vector projection nor linked ARM gameplay has been validated. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo matrix/particle prelude (2026-10-14)

Android ARM `update` at `0x9e7ac–0x9e82c` calls
`PaintCanvas::TransformGetLocal` on the ship geometry, passes its
matrix to `MatrixGetUp`, then assigns `PlayerEgo::vec_up`. The
`boostingFlag` byte at `+0x13c` chooses `ParticleSettings::Interpolate`
with sets **4, 6, 4** and `getBoostPercentage`, or copies initial set
4 into the live table while preserving the live `color0` at slot
offset `+0x34`. Android IDA lines 84443–84468 and Ghidra lines
64427–64443 independently corroborate the branches. The extracted
production-fragment smoke checks both, and the real
ParticleSettingsRef ABI/runtime smoke passes. Native `gof2`, all 206
ARM translation units, 37 verifier tests (one skipped), and four
PlayerEgo smokes pass. Focused `update` source-shape/strict fuzzy is
**31.4%/10.8%** versus **30.0%/10.2%**. Full report
`_work/player_visual_prelude_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**. Against
`_work/player_route_event_full_20261014.json`, linked and byte
exact-pair losses and gains are zero. The full update tail and
linked ARM gameplay are not established; the unlabelled iOS dump
could not independently confirm this exact sequence. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo route/HUD transition (2026-10-14)

Original ARM `update` at `0x9ce86–0x9ce8c` saves a pointer to
`PlayerEgo+0x148`; `0x9e82c–0x9e866` passes that pointer to
`Route::update(const Vector&)`, detects a changed current index, and
dispatches HUD event 23 or 24 according to the loop flag and new
index. Android IDA `setRoute`/`getRoute` at `0x9b150/0x9b156` and
secondary-index accessors at `0x9b7b0/0x9b7b8` establish ABI offsets
`+0xfc/+0x10c`. The production-fragment host smoke checks the event
branches and passed vector. Native `gof2`, all 206 ARM translation
units, 37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape/strict fuzzy is **30.0%/10.2%** versus
**28.7%/10.6%**. Full report
`_work/player_route_event_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**. Relative
to `_work/player_update_sound_ctor_full_20261014.json`, no exact
pairs are lost; four linked and three byte pairs are gained. The
whole `update`, intervening matrix/particle prelude and linked ARM
gameplay are not established. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo player/sound handoff (2026-10-14)

Android IDA `update` lines 84434–84453 and independent Ghidra
lines 64417–64425 confirm the direct player-update and positional
sound calls. The production-fragment host smoke checks alive/event
sound gating, options byte `+0x0f`, event ID, and the exact low-byte
test at `PlayerEgo+0x38`; an initial wrong field alias failed ARM32
assertions and was corrected before validation. Android IDA
constructor lines 80295–80339 and Ghidra lines 59471–59516
confirm the seven tested `+0x1c` sound-resource selections and
registration. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and four
PlayerEgo smokes pass. Focused `update` source-shape/strict fuzzy
is **28.7%/10.6%** versus **27.1%/10.5%**; constructor C1/C2
is **40.3%/24.8%** versus **33.4%/19.8%**. Full report
`_work/player_update_sound_ctor_full_20261014.json`:
**4,323/4,524** covered, **93.25%** source-shape,
**82.12%** strict fuzzy, **2,474 linked-exact**,
**1,062 byte-exact**, **123 extras**. Against
`_work/player_repair_timers_full_20261014.json`, linked and
byte exact-pair losses and gains are all zero. Full `update`
behavior and linked ARM gameplay are not proven. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo passive shield and repair timers (2026-10-14)

Android IDA constructor lines 80135–80144/80349–80353 and
`update` lines 84384–84436, cross-checked with independent Ghidra,
establish the three `int64_t` timers and the float recovery amount.
The original ARM literal pool at `0x9f09c..0x9f0ab` confirms the
four strict repair thresholds. ARM32 assertions pin the offsets;
the production-fragment host smoke covers the amount formula,
both repair types at and beyond threshold, alive/dead gating and
armor-after-hull ordering. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped) and four PlayerEgo smokes pass.
Focused `update` source-shape/strict fuzzy is **27.1%/10.5%**
versus **22.0%/8.3%**; constructor C1/C2 is **33.4%/19.8%**
versus **30.4%/18.7%**. Full report
`_work/player_repair_timers_full_20261014.json`:
**4,323/4,524** covered, **93.24%** source-shape,
**82.12%** strict fuzzy, **2,474 linked-exact**,
**1,062 byte-exact**, **123 extras**. Against
`_work/player_shield_update_full_20261014.json`, linked and
byte exact-pair losses and gains are all zero. Neither entire
function nor linked ARM gameplay is proven. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo shield recharge branch (2026-10-14)

Android IDA lines 83509–83545/84977–85005 and independent Ghidra
lines 63331–63397 establish the start, charging, sound and completion
branches. The host smoke extracts the production `update` fragment
and checks missing equipment/cargo, zero-HP activation, same-frame
minimum healing, scaled healing, continuation and completion. Its
fixture also repairs the stale throttle-image member name; all four
PlayerEgo smokes pass. Native `gof2`, 206/206 ARM units and 37
verifier tests (one skipped) pass. Focused `update` source-shape
is **22.0%** versus **14.3%** before, strict fuzzy **8.3%** versus
**4.1%**. Full report `_work/player_shield_update_full_20261014.json`:
**4,323/4,524** covered, **93.24%** source-shape, **82.12%**
strict fuzzy, **2,474 linked-exact**, **1,062 byte-exact**,
**123 extras**. Relative to
`_work/player_terminal_flags_full_20261014.json`, exact-pair
losses and gains are both zero. Full `update` behavior and linked
ARM gameplay are not proven. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo shield/mining flag split (2026-10-14)

Original Android constructor, `update`, `lostMiningGame`, and
`approachAsteroid` distinguish `+0x399/+0x39a` shield bytes from
mining-loss byte `+0x39b`. ARM32 assertions pin all three; focused
verification makes `lostMiningGame()` linked- and byte-exact.
Native `gof2`, 206/206 ARM translation units, 37 verifier tests
(one skipped), and three PlayerEgo smokes pass. Full report
`_work/player_terminal_flags_full_20261014.json`: **4,323/4,524**
covered, **93.24%** source-shape, **82.12%** strict fuzzy,
**2,474 linked-exact**, **1,062 byte-exact**, **123 extras**.
Relative to `_work/player_cargo_refresh_full_20261014.json`,
exact-pair losses are zero and gains are one linked/byte match.
The shield branch in `update` and linked ARM gameplay remain untested.
See [the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cargo-state refresh (2026-10-14)

Raw Android ARM at `0x9d1a6–0x9d1d2` confirms the strict
free-space-decrease guard, repeated free-space read and volatile
flag write at `+0x250/+0x398`. The extended docking smoke extracts
and tests this production fragment across decrease, increase and
equality cases. Native `gof2`, 206/206 ARM units, 37 verifier
tests (one skipped), and three PlayerEgo smokes pass. Full report
`_work/player_cargo_refresh_full_20261014.json`:
**4,323/4,524** covered, **93.24%** source-shape, **82.11%**
strict fuzzy, **2,473 linked-exact**, **1,061 byte-exact**,
**123 extras**. Against
`_work/player_turret_position_full_20261014.json`, exact-pair
sets have zero losses and zero gains. The rest of `update` and
linked ARM32 behavior are not verified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo turret position repair (2026-10-14)

Raw ARM `0x9b644–0x9b656` passes `PlayerEgo+0x224` to both
`AEGeometry::setPosition` calls in `checkForTurret`;
the new ARM32 assertion confirms this is `turretOffsetVec`.
Native `gof2`, 206/206 ARM units and 37 verifier tests
(one skipped) pass. Full report
`_work/player_turret_position_full_20261014.json`:
**4,323/4,524** covered, **93.24%** source-shape,
**82.11%** strict fuzzy, **2,473 linked-exact**,
**1,061 byte-exact**, **123 extras**. Relative to
`_work/player_hud_ctor_full_20261014.json`, exact-pair sets
have zero losses and zero gains. The full function and linked
ARM gameplay remain unverified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo HUD/constructor group (2026-10-14)

IDA constructor lines 80339–80349 and 80415–80439 establish the
agility/handling calculation, hard-core and cargo fields, and
five Image2D resource IDs. `draw` and `drawThrottle` at lines
87313–87410 confirm their consumers. ARM32 offset assertions
cover the reconstructed members. Native `gof2`, 206/206 ARM
units, 37 verifier tests (one skipped), and three PlayerEgo smokes
pass. Full report `_work/player_hud_ctor_full_20261014.json`:
**4,323/4,524** covered, **93.24%** source-shape, **82.11%**
strict fuzzy, **2,473 linked-exact**, **1,061 byte-exact**,
**123 extras**. Against
`_work/player_abi_late_final_full_20261014.json`, exact-pair
losses are zero and `collidesWithStation` adds one linked/byte
match. Runtime HUD visuals and cargo-state refresh are not yet
verified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo late movement/visibility ABI repair (2026-10-14)

Original IDA `resetMovement`, `roll`, `rotate`, `setRotation`,
`explosionEnded`, `setVisible`, dock-total accessors, `setShip`
and `hasVolatileGoods` establish offsets through `+0x399`.
ARM32 offset and size assertions match them. The omitted
`setExhaustVisible` call has been restored; focused `setVisible`
source-shape is 100%, though it is not exact. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and three
PlayerEgo smokes pass. Full report
`_work/player_abi_late_final_full_20261014.json`:
**4,323/4,524** covered, **93.23%** source-shape, **82.09%**
strict fuzzy, **2,472 linked-exact**, **1,060 byte-exact**,
**123 extras**. Compared with
`_work/player_abi_hud_full_20261014.json`, exact-pair losses
are zero; gains are 11 linked and 9 byte. Constructor semantics
and linked ARM32 gameplay remain unverified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo HUD/turret ABI repair (2026-10-14)

Original Android ARM `getHUD` reads `+0x220` (IDA line 87490),
`checkForTurret` reads `+0x1a0` and `+0x1b0` (IDA lines 82047 and
82067), and `approachDockingPoint` reads `+0x1a0` (IDA line 85693).
ARM32 offset assertions and the NDK r18b record dump now confirm
those source members. Native `gof2`, 206/206 ARM units, 37 verifier
tests (one skipped), and three PlayerEgo smokes pass. Full report
`_work/player_abi_hud_full_20261014.json`: **4,323/4,524**
covered, **93.20%** source-shape, **81.98%** strict fuzzy,
**2,461 linked-exact**, **1,051 byte-exact**, **123 extras**.
Compared with `_work/player_abi_handling_full_20261014.json`,
exact-pair losses are zero; linked gains are two and byte gains
one. This is not a linked ARM gameplay test, and later member
offsets remain unverified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo camera-handling semantic repair (2026-10-14)

Original ARM `setTargetFollowCamera` at `0xa21f2–0xa21fe` loads
`PlayerEgo+0x154`, stores the camera at `+0x88`, then branches to
`TargetFollowCamera::setShipHandling(float)`. The source now uses
a float handling member and this call, rather than converting the
float to int in the constructor and resetting camera handling.
Focused verification gives 100% source-shape and linked-exact for
the setter, but not byte-exact. Native
`gof2`, all 206 ARM units, 37 verifier tests (one skipped) and
three PlayerEgo smokes pass. Full report
`_work/player_abi_handling_full_20261014.json`: **4,323/4,524**
covered, **93.20%** source-shape, **81.97%** strict fuzzy,
**2,459** linked-exact, **1,050** byte-exact and **123** extras.
Against `_work/player_abi_mining_full_20261014.json`, exact-pair
sets have zero losses and one linked gain. HUD/later ABI and linked
ARM gameplay remain unverified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo mining/docking ARM32 layout repair (2026-10-14)

Original `dockToAsteroid` at IDA lines 85200–85206 writes the
scaling value to `PlayerEgo+0x1d8` and its timer to `+0x1dc`;
`approachAsteroid` accesses `MiningGame` at `+0x1e4`, and
`isHacking` reads `+0x1e8`. The NDK r18b Clang record dump now
places these fields and the intervening station at their ARM
offsets. Native `gof2`, all 206 ARM units, 37 verifier tests
(one skipped) and three PlayerEgo smokes pass. Full report
`_work/player_abi_mining_full_20261014.json`: **4,323/4,524**
covered, **93.20%** source-shape, **81.95%** strict fuzzy,
**2,458** linked-exact, **1,050** byte-exact, **123** extras.
Compared with `_work/player_abi_frame_full_20261014.json`, there
are 19 linked/12 byte gains and zero exact-pair losses. The class
size equals the original allocation (`0x3a0`), but HUD remains
8 bytes early and runtime ABI is not proved. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo frame/docking ARM32 layout repair (2026-10-14)

Original `update` ARM instructions at `0x9ce86–0x9ce96` assign
the geometry position to `PlayerEgo+0x148` and frame time to
`+0x134`. Other original accessors/constructor writes confirm
`lastHP +0x130`, boost timer `+0x138`, boost flag `+0x13c`, collide
flag `+0x144`, handling `+0x154`, auto-pilot target `+0x15c`,
asteroid target `+0x1bc`, and docking state `+0x1c0`. NDK r18b
Clang record layout now matches these addresses. Native `gof2`,
all 206 ARM units, 37 verifier tests (one skipped) and three
PlayerEgo smokes pass. Full report
`_work/player_abi_frame_full_20261014.json`: **4,323/4,524**
covered, **93.17%** source-shape, **81.85%** strict fuzzy,
**2,439** linked-exact, **1,038** byte-exact, **123** extras.
Compared with `_work/player_abi_controls_final_full_20261014.json`,
there are 25 linked/19 byte gains and zero exact-pair losses.
`update` source-shape rises 12.8% to 13.4%, but remains non-exact;
runtime behavior is not validated. The class is `0x398` against
the original `0x3a0` allocation; later member offsets differ.
See [the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo control-field ARM32 layout repair (2026-10-14)

Original accessors at IDA lines 86918–86948 and 87496–87577 place
camera-switch flags at `+0xb0`/`+0xb1`, speed at `+0xb8`, thrust at
`+0xbc`, and free-look mode at `+0xc4`. Original constructor lines
80248–80257 initialize speed/thrust and boost values at `+0xc8`
through `+0xd0`. The NDK r18b Clang record dump now matches these offsets.
Focused verify gives nine byte-exact control methods; full corpus
adds `changeThrust` as a tenth. Native `gof2`, all 206 ARM units and
37 verifier tests pass (one skipped). Full report
`_work/player_abi_controls_final_full_20261014.json`: **4,323/4,524**
covered, **93.13%** source-shape, **81.64%** strict fuzzy,
**2,414** linked-exact, **1,019** byte-exact and **123** extras.
The exact-pair comparison with
`_work/player_abi_matrix_full_20261014.json` has zero losses.
Remaining class size `0x378` vs original `0x3a0` leaves handling,
docking and later member accesses unverified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo early ARM32 layout repair (2026-10-14)

NDK r18b Clang's record dump now places the Matrix storage at
`+0x40`, maneuver parameters at `+0x7c`/`+0x80`, and camera pointer
at `+0x88`, matching the original constructor, maneuver writes and
camera getter. The constructor now initializes that Matrix storage;
its focused source-shape rises 18.5% to 18.9% and it remains
non-exact. The getter verifies byte-exact. Native `gof2` and
all 206 ARM units compile. The complete corpus report
`_work/player_abi_matrix_full_20261014.json` gives 4,323/4,524
covered, 93.11% source-shape, 81.54% strict fuzzy, 2,404 linked-
exact and 1,009 byte-exact, with no lost exact pairs against
`_work/approach_asteroid_guard_full_20261014.json`. The class is
still **0x360** versus the original **0x3a0** allocation, so later
field accesses and full gameplay remain unverified. See
[the offset evidence](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo asteroid mining phase (2026-10-14)

ARM `approachAsteroid` loads its target from `PlayerEgo+0x1bc`
before both dying/dead checks, preserves the incoming HUD in `r5`,
and routes mining flags, sound and status through known globals.
The C++ no longer uses unassigned holders or writes the wrong
asteroid slot. Focused source-shape rises 23.8% to 39.6%, strict
fuzzy 13.5% to 21.6%; no exact match. A production-method host
smoke checks guard, creation, loss and target death. Native `gof2`
static-library build, all 206 ARM units, the docking and stopMining
smokes and 37 verifier tests pass (one skipped). Full corpus stays
4,323/4,524, 93.10% average source-shape, 81.52% strict fuzzy,
2,403 linked-exact, 1,008 byte-exact and 123 extras; exact pairs
are unchanged. Stage-zero approach still has an undefined helper;
full link/gameplay and ARM32 runtime are not proved. See
[the evidence](PLAYER_EGO_APPROACH_ASTEROID_ARM_2026-10-14.md).

## PlayerEgo update docking teardown (2026-10-14)

Raw Android ARM instructions at `0x9dd18–0x9dd44` check the target
at `PlayerEgo+0x1bc`, test `KIPlayer::isDying` and `isDead`, pass
the saved radar register as the third `dockToAsteroid` argument,
then branch to the function epilogue. The docking-point path at
`0x9dd7a–0x9dd9e` checks the same target states and also returns
directly. The C++ now follows those paths. The host smoke extracts
and compiles the production docking-state fragment. Native `gof2`,
all 206 ARM units, the `stopMining` smoke and 37 verifier tests
pass (one skipped). Focused `update` source-shape rises 12.2% to
12.7%, strict fuzzy falls 4.8% to 4.1%; no exact match. Full
corpus remains 4,323/4,524 covered, 93.10% average source-shape,
81.51% strict fuzzy, 2,403 linked-exact, 1,008 byte-exact and
123 extras; the exact-pair sets are unchanged. Full gameplay and
the missing-target path remain unverified. See
[the evidence](PLAYER_EGO_UPDATE_DOCKING_ARM_2026-10-14.md).

## PlayerEgo asteroid-docking target (2026-10-14)

Android ARM `dockToAsteroid` tests the first argument (`KIPlayer*`)
on entry, stores it at offset `0x1bc`, and calls
`PlayerAsteroid::getScaling` on it. The previous C++ tested and
stored the second argument (`Radar*`). Focused source-shape rises
90.8% to 94.7%, strict fuzzy 53.7% to 72.1%, without exact match.
The production-method host smoke covers distinct target/radar
objects, no-target behavior and teardown; native `gof2`, all 206
ARM units, the mining handoff smoke and 37 verifier tests pass
(one skipped). Full corpus and exact-pair sets remain 4,323/4,524,
93.10% average source-shape, 81.51% strict fuzzy, 2,403 linked-exact,
1,008 byte-exact and 123 extras, with zero pair losses. This does
not prove ARM32 runtime. The null-radar call from `update` is
corrected in the follow-up audit above.
See [the evidence](PLAYER_EGO_DOCK_ASTEROID_ARM_2026-10-14.md).

## Ship cargo-add handoff (2026-10-14)

Focused `Ship::addCargo(Item*)` stays at 100.0% source-shape and
strict fuzzy before and after its ARM-backed call-target correction;
it remains non-exact. The production cargo-method smoke verifies that
core and ore coexist and duplicate ore merges. Native `gof2`, all
206 ARM units, the combined mining handoff smoke and 37 verifier
tests pass (one skipped). Full corpus remains 4,323/4,524 covered
at 93.10% average source-shape and 81.51% strict fuzzy; the 2,403
linked-exact, 1,008 byte-exact and 123 extra sets are unchanged.
This does not validate full gameplay, HUD/docking/audio or cargo
memory ownership. See
[the evidence](SHIP_ADD_CARGO_HANDOFF_ARM_2026-10-14.md).

## Status hardcore difficulty route (2026-10-14)

Focused `Status::hardCoreMode` source-shape rises 83.6% to 93.7%,
strict fuzzy 66.7% to 88.0%, without exact match. The original ELF
maps its float to `Globals::options + 0x2c`; the combined host smoke
executes this production method with `PlayerEgo::stopMining` for both
normal and hardcore outcomes. Native `gof2`, all 206 ARM units and
37 verifier tests pass (one skipped). Full corpus stays 4,323/4,524
covered; average source-shape rises 93.09% to 93.10%, strict fuzzy
stays 81.51%, and the 2,403 linked-exact, 1,008 byte-exact and
123 extra sets are unchanged. End-to-end native/ARM32 mining
and persisted options loading remain untested. See
[the evidence](STATUS_HARDCORE_OPTIONS_ARM_2026-10-14.md).

## PlayerEgo mining-result handoff (2026-10-14)

Focused `stopMining` source-shape rises 9.3% to 91.0%, strict fuzzy
4.4% to 39.7%, without exact match. Six production-method host
scenarios cover cargo, capacity, hardcore loss, discovery flags, HUD
calls and terminal cleanup. Native `gof2`, all 206 ARM units, the
three MiningGame smokes and 37 verifier tests pass (one skipped).
Full corpus coverage remains 4,323/4,524; average source-shape rises
93.08% to 93.09% and strict fuzzy 81.50% to 81.51%. The 2,403
linked-exact and 1,008 byte-exact sets, 123 extras and 201 missing
are unchanged.
At that checkpoint this was not an end-to-end native run because
`Status::hardCoreMode` had an unassigned service pointer; the direct
route is recovered above. See
[the evidence](PLAYER_EGO_STOP_MINING_ARM_2026-10-14.md).

## MiningGame outcomes and teardown audit (2026-10-14)

The production-method layer/update smoke now covers the 2500/2501 ms
loss boundary, ore/status reset, medal HUD thresholds and byte-only
completion flag. The constructor smoke executes the production
destructor and confirms it destroys the drill sprite but not the
marquees, as in Android ARM. The render smoke, native `gof2`, all 206
ARM units and 37 verifier tests pass (one skipped). No production
code changed: corpus coverage and shape stay 4,323/4,524,
93.08% average source-shape and 81.50% strict fuzzy; all 2,403
linked-exact, 1,008 byte-exact pairs and 123 extra symbols are
unchanged. `PlayerEgo::stopMining` still omits the ARM cargo/status/
ship/audio handoff; full mining flow and ARM32 runtime remain
untested. See [the audit](MINING_OUTCOMES_TEARDOWN_ARM_2026-10-14.md).

## MiningGame render2D service and call recovery (2026-10-14)

Focused `render2D` source-shape rises 25.6% to 97.4% and strict
fuzzy 14.5% to 78.9% (454 generated versus 464 target instructions),
without an exact match. Its production-method host smoke checks
ring/image flags, warning colors, ore and prompt text and positions,
and the no-prompt path. The constructor/layer/update smokes, native
`gof2`, all 206 ARM units and 37 verifier tests pass (one skipped).
Full corpus: 4,323/4,524 covered, 93.08% average source-shape,
81.50% strict fuzzy, 2,403 linked-exact and 1,008 byte-exact; exact
and 123-symbol extra sets are unchanged.
Game-asset visuals, status-loss/medal paths and ARM32 runtime remain
untested. See [the evidence](MINING_RENDER_ARM_2026-10-14.md).

## MiningGame constructor service and image bindings (2026-10-14)

Focused C1/C2 source-shape rises 1.3% to 99.1%, strict fuzzy 0.6%
to 89.1% (313 generated versus 311 target instructions for each);
neither alias is exact. The production-constructor host smoke, the
existing layer/update smoke, native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). Full corpus: 4,323/4,524 covered,
93.06% average source-shape, 81.48% strict fuzzy, 2,403 linked-exact
and 1,008 byte-exact; exact and 123-symbol extra sets are unchanged.
The no-equipment branch and full ARM32 mining runtime remain untested. See
[the evidence](MINING_CONSTRUCTOR_ARM_2026-10-14.md).

## MiningGame update and layer speeds (2026-10-14)

Focused `update` source-shape rises 14.1% to 95.1%, strict fuzzy
14.1% to 82.1% (357 generated versus 376 target instructions), with
no exact match. At this pre-constructor snapshot, C1/C2 remain 1.3%
source-shape and 0.6% strict fuzzy. The production-method host smoke,
native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Full corpus:
4,323/4,524 covered, 93.02% average source-shape, 81.45% strict
fuzzy, 2,403 linked-exact and 1,008 byte-exact. Exact and extra-symbol
sets are unchanged. Medal/loss branches and ARM32 runtime remain
untested. See [the evidence](MINING_UPDATE_ARM_2026-10-14.md).

## MiningGame layer membership (2026-10-14)

The Android-backed `isInCurrentLayer()` reaches 99.1% focused
source-shape and 81.8% strict fuzzy (44 generated and target
instructions), but is neither linked-exact nor byte-exact. The
production-method host smoke, native `gof2`, all 206 ARM units and
37 verifier tests pass (one skipped). Full corpus: 4,323/4,524
covered, 93.00% average source-shape, 81.43% strict fuzzy, 2,403
linked-exact and 1,008 byte-exact. Exact-pair and 123-symbol extra
sets are unchanged; ARM32 runtime remains untested. See
[the evidence](MINING_LAYER_MEMBERSHIP_ARM_2026-10-14.md).

## Layout constructor MiningGame parameters (2026-10-14)

Focused `Layout` C1/C2 source-shape rises 29.3% to 30.9%, strict
fuzzy stays 7.6%, and neither alias is exact. The `MiningGame`
constructor's focused shape remains 1.3% source and 0.6% strict.
The six-profile host smoke, native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). Full corpus remains 4,323/4,524
covered at 92.98% average source-shape and 81.41% strict fuzzy;
2,403 linked-exact and 1,008 byte-exact pairs are preserved with
zero exact-set or extra-symbol changes. The layout-supplied drift divisor
is nonzero for every covered platform branch, but unassigned
`MiningGame` service holders and untested ARM32 runtime remain. See
[the evidence](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout constructor HUD initialization fields (2026-10-14)

Focused C1/C2 source-shape rises 27.3% to 29.3% and strict fuzzy
5.7% to 7.6%; neither alias is exact. The six-profile production
constructor smoke, native `gof2`, all 206 ARM units, and 37 verifier
tests pass (one skipped). Full corpus remains 4,323/4,524 covered at
92.98% average source-shape, 81.41% strict fuzzy, 2,403 linked-exact,
and 1,008 byte-exact; exact and extra-symbol sets have zero change.
`Hud::init` copies these eight fields into touch, radar and event
state; ARM32 runtime and visual/interaction behavior remain untested. See
[the evidence](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout constructor consumer-backed middle fields (2026-10-14)

The six-profile host smoke, native `gof2`, all 206 ARM translation
units, and 37 verifier tests pass (one skipped). Focused C1/C2
source-shape rises 23.9% to 27.3%; strict fuzzy moves 5.8% to 5.7%;
neither alias is exact. Full corpus remains 4,323/4,524 covered at
92.98% average source-shape, 81.41% strict fuzzy, 2,403 linked-exact,
and 1,008 byte-exact, with no exact-set or extra-symbol change. These
fields are read by `MenuTouchWindow`, `StarMap`, and `Radio`, but
original ARM32 runtime and visual layout remain untested. See
[the evidence](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout constructor platform metrics (2026-10-14)

Both Android `Layout::Layout()` C1/C2 aliases rise from 8.7% to 23.9%
focused source-shape and from 2.9% to 5.8% strict fuzzy; neither is
exact. The six-profile production-method host smoke, native `gof2`,
all 206 ARM units, and 37 verifier tests pass (one skipped). Full
corpus: 4,323/4,524 covered at 92.98% average source-shape and 81.41%
strict fuzzy; 2,403 linked-exact and 1,008 byte-exact with zero
exact-set or extra-symbol changes. Original ARM32 runtime, visual
layout, and many constructor fields remain unverified. See
[the evidence](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout mission reward (2026-10-14)

The Android-backed reward display reaches 100.0% focused
source-shape for `showMissionRewardMessage` and 97.3% for
`drawMissionRewardMessage`; neither is exact. The production-method
host smoke checks sound, text, icon, boxes and fade boundaries.
Native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Full corpus: 4,323/4,524 covered, 2,403 linked-exact,
1,008 byte-exact, no exact-set or extra-symbol changes. Original
ARM32 runtime and visual rendering remain untested. See
[the evidence](LAYOUT_MISSION_REWARD_ARM_2026-10-14.md).

## Layout help-window dispatch (2026-10-14)

The Android-backed `Layout::initHelpWindow(String)` reaches 100.0%
focused source-shape and 97.9% strict fuzzy, but remains non-exact;
`drawHelpWindow()` reaches 100.0% focused source-shape and strict
fuzzy, adding one linked-exact (not byte-exact) result. The
production-method host smoke checks the help-window path. Native
`gof2`, all 206 ARM units and 37 verifier tests pass (one skipped).
Full corpus: 4,323/4,524 covered, 2,403 linked-exact and 1,008
byte-exact; complete exact/extra sets show one linked-exact gain
and no losses. Original ARM32 runtime and visual rendering remain
untested. See
[the evidence](LAYOUT_HELP_WINDOW_ARM_2026-10-14.md).

## Layout color-tag string (2026-10-14)

The Android-backed `Layout::tagString(String)` now reaches 100.0%
focused source-shape and strict fuzzy, but remains non-exact. Its
instance ABI and literal sequence are restored; the host smoke
checks the produced tag. Native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). Full corpus exact and
extra-symbol sets are unchanged. Original ARM32 runtime and visual
rendering remain untested. See
[the evidence](LAYOUT_TAG_STRING_ARM_2026-10-14.md).

## Layout number formatting (2026-10-13)

The Android-backed `Layout::formatNumber(int)` reaches 98.9%
source-shape and 80.1% strict fuzzy; it remains non-exact. The
production-method host smoke covers grouped and negative values,
language-specific separators and `INT_MIN`. Native `gof2`, all 206
ARM units and 37 verifier tests pass (one skipped). The `INT_MIN`
output is a host observation, not an ARM32 runtime test. See
[the evidence](LAYOUT_FORMAT_NUMBER_ARM_2026-10-13.md).

## Layout drawBox default-flags wrapper (2026-10-13)

The Android-backed six-argument `Layout::drawBox` wrapper reaches
100.0% source-shape and strict fuzzy, adding a linked-exact (not
byte-exact) result. Native `gof2`, all 206 ARM units, the
production-method UI smoke and 37 verifier tests pass (one skipped).
The smoke cannot observe flag bit 0, which the current renderer does
not test; the focused ARM comparison verifies the call shape. See
[the evidence](LAYOUT_DRAWBOX_WRAPPER_ARM_2026-10-13.md).

## Layout window-dimension reset (2026-10-13)

The Android-backed `Layout::resetWindowDimensions()` reaches 99.8%
source-shape and 89.4% strict fuzzy; it remains non-exact. The
production-method UI smoke checks button positions and published
coordinates. Native `gof2`, all 206 ARM units and 37 verifier tests
pass (one skipped). See
[the evidence](LAYOUT_RESET_WINDOW_DIMENSIONS_ARM_2026-10-13.md).

## Layout one-argument window wrapper (2026-10-13)

The Android-backed `Layout::drawWindow(String)` forwards
`drawBG=false`, correcting a behavior difference. Focused
source-shape and strict fuzzy reach 100.0%, adding a linked-exact
(not byte-exact) result. The production-method UI smoke checks the
background-free frame, and native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). See
[the evidence](LAYOUT_ONE_ARG_WINDOW_ARM_2026-10-13.md).

## Layout full-screen window wrapper (2026-10-13)

The Android-backed `Layout::drawWindow(String,bool)` reaches 100.0%
source-shape and 100.0% strict fuzzy, adding a linked-exact (not
byte-exact) result. The production-method UI smoke checks both
background paths and the original double top-inset adjustment. Native
`gof2`, all 206 ARM units and 37 verifier tests pass (one skipped).
This is not an ARM32 runtime or visual rendering test. See
[the evidence](LAYOUT_WINDOW_WRAPPER_ARM_2026-10-13.md).

## Layout header (2026-10-13)

Android-backed `Layout::drawHeader(String,bool)` reaches 99.8%
source-shape and 97.7% strict fuzzy, but remains non-exact. The
expanded production-method UI smoke checks image IDs, cap placement,
title text and help-button branches. Native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). See
[the evidence](LAYOUT_DRAW_HEADER_ARM_2026-10-13.md).

## Layout framed window (2026-10-13)

The Android-backed six-argument `Layout::drawWindow` reaches 100.0%
source-shape and strict fuzzy, but remains non-exact. The expanded
TouchButton/Layout smoke checks background on/off, outline and image
placement, and color restoration. Native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). The original title
condition remains unusual; see [the audit](LAYOUT_DRAW_WINDOW_ARM_2026-10-13.md).

## Layout empty footer (2026-10-13)

Android-backed `Layout::drawEmptyFooter(bool)` reaches 100.0%
source-shape and strict fuzzy, with a linked-exact (not byte-exact)
result. The TouchButton/Layout smoke exercises both `showBack` paths,
and native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). This is not an ARM32 visual rendering test. See
[the evidence](LAYOUT_EMPTY_FOOTER_ARM_2026-10-13.md).

## Layout tip initialization (2026-10-13)

The Android-backed `Layout::initTip()` recovery reaches 100.0%
source-shape and 97.0% strict fuzzy but remains non-exact. Its
production-method host smoke checks two calls, the text ID, font,
wrap width and old-array release. Native `gof2`, all 206 ARM units and
37 verifier tests pass (one skipped). See
[the evidence](LAYOUT_INIT_TIP_ARM_2026-10-13.md).

## Layout tip (2026-10-13)

The Android-backed `Layout::drawTip()` recovery is 100.0% source-shape
and 100.0% strict fuzzy, with a linked-exact (not byte-exact) result.
All 206 ARM units, native `gof2`, the TouchButton/Layout host smoke and
37 verifier tests pass (one skipped). This verifies the recovered ARM
form, not rendering on an ARM device. See [the evidence](LAYOUT_TIP_ARM_2026-10-13.md).

Continuously check whether our recovered C++ still assembles to the **same
instructions** as the original Android binary, function by function, and tune
compiler flags toward byte-exactness — without disturbing normal macOS
development.

## TL;DR

```bash
# one-time on macOS/Linux with OrbStack: provision the ubuntu machine
bash tools/verify/setup.sh

# Windows/MSYS fallback: use local Android NDK r18b and populate _work from the
# original Android .so.
export GOF2_VERIFY_LOCAL_NDK=1
python3 tools/verify/prepare_work_from_so.py \
  --so ../references/Apk/lib/armeabi-v7a/libgof2hdaa.so \
  --work _work

# configure the matching build and run the report
cmake --preset match
cmake --build cmake-build-match --target verify

# inspect one function (side-by-side disassembly diff)
FN=_Z8ArrayAddIP14AEPakFileEntryEvT_R5ArrayIS2_E cmake --build cmake-build-match --target verify-fn

# or directly:
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build \
  --unit game/weapons/Radar --only '^_ZN5Radar'
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build --show _ZN5Radar10hasScannerEv
# On Windows/MSYS, add the known translation unit to avoid scanning every object:
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build \
  --unit game/menu/MGame --show _ZN5MGame10OnTouchEndEiiPv
# Focused Radar inspection also avoids a whole-corpus scan:
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build \
  --unit game/weapons/Radar --show _ZN5Radar4drawEP6PlayerP3Hudi
```

Normal development is unchanged: `cmake --preset debug` still uses local Apple
clang and never touches OrbStack/local NDK verify tooling.

## Radar Snapshot (2026-09-15)

The unchanged 19-symbol family averages **99.0% source-shape**, with 13
linked-exact and eight raw-byte-exact symbols. Large draw itself measures
**90.8% source-shape / 41.7% strict fuzzy**, not byte-exact. Constructor and
destructor aliases and short accessors affect the unweighted mean; never
describe it as 99% exact draw or game recovery. See
[the evidence and remaining work](RADAR_DRAW_SOURCE_SHAPE_2026-09-15.md) and
[the per-function snapshot](validation/radar_family_2026-09-15.json).

`--unit` now selects a single object for family reports too, before any corpus
traversal or comparison. Add `--no-build` to reuse current objects. Such reports
contain `scoped_unit`, not global coverage claims, and reject
`--fail-on-wrong-type`: that gate still requires the full unfiltered corpus.
Run `python3 -m unittest discover -s tools/verify -p 'test_*.py'` for the scoped
runner and ASM normalization regressions. Never run simultaneous reports or
`--show` operations that rewrite the same target object.

## ParticleSettingsRef Native Smoke

The focused ABI/runtime smoke target does not link the whole recovered host
library: unrelated render shim symbols still prevent that library from forming
a standalone executable. It compiles only the two audited table translation
units and exercises their actual initialization/scaling/interpolation path.

```powershell
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja `
  -DGOF2_BUILD_PARTICLE_SETTINGS_SMOKE=ON
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt `
  --target gof2_particle_settings_smoke
.\cmake-build-ucrt\gof2_particle_settings_smoke.exe
```

Expected output: `ParticleSettingsRef ABI/runtime smoke: OK`.

## Focused ABI/Lifetime Smoke

```powershell
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja `
  -DGOF2_BUILD_ABI_RUNTIME_SMOKE=ON
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt `
  --target gof2_abi_runtime_smoke
.\cmake-build-ucrt\gof2_abi_runtime_smoke.exe
```

Expected output: `GOF2 native ABI/lifetime smoke: OK`. This exercises real
String/Array paths and probe-module vtable dispatch on the host. A fresh
`armeabi-v7a` build of the same isolated target ran 5/5 times on an
Android 12 M2007J20CG on 2026-10-15; see
[the ABI gate](ABI_RUNTIME_GATE_2026-09-27.md) for build identity,
original vtable relocations, and remaining ownership risks. Neither
the host nor the device smoke proves a playable native game.

## ImageFactory Portrait Host Smoke

For the Android-backed portrait table and dispatch path:

```powershell
python tools/run_image_factory_portrait_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `ImageFactory portrait load/draw smoke: OK`. The runner
executes extracted production factory/part bodies with fixture canvas,
random, sprite, and reload services; it removes its temporary executable.
It does not validate original assets or OpenGL. See
[the portrait audit](IMAGE_FACTORY_PORTRAITS_ARM_2026-10-13.md).

For the adjacent production Sprite/reload path:

```powershell
python tools/run_sprite_reload_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Sprite reload/draw host smoke: OK`. It distinguishes
the original `Globals::Canvas` from `PaintCanvas::gCanvas`, checks
asymmetric scaling and frame-array destruction calls, and removes its temporary
executable. It does not render real assets; see
[the Sprite audit](SPRITE_CANVAS_SCALE_ARM_2026-10-13.md).

## TouchButton/Layout Host Smoke

```powershell
python tools/run_touchbutton_touch_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `TouchButton/Layout host smoke: OK`. The runner extracts the
unchanged production touch, draw, pattern, fade, footer, box, border, metric, and mask
method bodies, executes them with fixture canvas/sound/status services,
and removes its temporary executable.
This checks selected geometry and call behavior, not a full game UI or ARM
runtime. See [the UI host-smoke audit](TOUCHBUTTON_UI_HOST_SMOKE_ARM_2026-10-12.md).

```powershell
python tools/run_layout_scroll_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Layout scrollbar host smoke: OK`. This extracts the
production scrollbar method and checks track/cap geometry, colors and
both thumb branches with fixture canvas services. It does not render
original textures or run on ARM32. See
[the scrollbar audit](LAYOUT_SCROLLBAR_ARM_2026-10-13.md).

```powershell
python tools/run_player_ego_throttle_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `PlayerEgo throttle host smoke: OK`. This extracts the
production throttle method and checks its color, region/text calls, and
inactive branch with fixture services; it does not run original assets or
the full game. See [the region-call audit](REGION_CALL_THROTTLE_ARM_2026-10-12.md).

```powershell
python tools/run_status_standing_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow standing host smoke: OK`. This extracts
only the production standing-bar block, checking geometry and region/image
calls with fixtures; it does not execute the whole window. See
[the standing-bar audit](STATUS_STANDING_REGION_ARM_2026-10-12.md).

```powershell
python tools/run_status_draw_prefix_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow draw-prefix host smoke: OK`. This extracts
the production draw setup and checks the shared canvas/layout, scroll-bar,
and iPad width behavior with fixtures; it does not render the full window.
See [the draw-globals audit](STATUSWINDOW_DRAW_GLOBALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_draw_info_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow info-card host smoke: OK`. This extracts the
production credits, level, time and ship-text block and checks strings and
positions with fixtures; it does not load original assets or draw the
statistics table. See [the info-card audit](STATUSWINDOW_DRAW_INFO_CARD_ARM_2026-10-13.md).

```powershell
python tools/run_status_draw_stats_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow stats-table host smoke: OK`. This extracts
the production standing-section boxes and statistics table, checking the
two-column labels, values, positions and language-9 separator with
fixtures; it does not load original assets or render a complete UI frame. See
[the statistics-table audit](STATUSWINDOW_DRAW_STATS_TABLE_ARM_2026-10-13.md).

```powershell
python tools/run_status_draw_medals_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow medal-grid host smoke: OK`. This extracts
the production medal grid and detail pane, checking iPad/phone placement,
visibility, header and detail-text coordinates with fixture services. It
does not execute the full window. See
[the medal-grid audit](STATUSWINDOW_DRAW_MEDALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_ctor_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow constructor host smoke: OK`. This runs
the production constructor with fixture services for phone and iPad tabs,
medals, images and dimensions; it does not load original assets or run
on ARM32. See [the constructor audit](STATUSWINDOW_CTOR_GLOBALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_touch_end_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow touch-end host smoke: OK`. This runs the
production touch begin, move and end methods with fixture services for
scroll, tabs, medals and help; it does not use the original assets or
full `Array` ABI. See [the touch-end audit](STATUSWINDOW_TOUCH_END_ARM_2026-10-13.md)
and [the touch-globals follow-up](STATUSWINDOW_TOUCH_GLOBALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_dtor_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow destructor host smoke: OK`. This runs
the production destructor with tracked fixture objects and arrays; it
does not test the original ARM32 allocator. See
[the destructor audit](STATUSWINDOW_DTOR_ARM_2026-10-13.md).

```powershell
python tools/run_status_medal_hint_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow medal-hint host smoke: OK`. This extracts the
production hint method and checks branch-specific separators and text IDs
with fixtures; it does not use original text assets or execute touch input.
See [the medal-hint audit](STATUSWINDOW_MEDAL_HINT_ARM_2026-10-13.md).

```powershell
python tools/run_status_reinit_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow reInit host smoke: OK`. This extracts the
production method and checks standing queries, rank-image selection and
size reads with fixture services. It does not load original image assets
or execute an ARM32 UI. See [the reInit audit](STATUSWINDOW_REINIT_ARM_2026-10-13.md).

```powershell
python tools/run_paintcanvas_region_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `PaintCanvas region host smoke: OK`. This extracts the
production ten-argument region method, checking its crop geometry,
rotation/pivot matrix, and draw-call order with fixture services. It
does not execute OpenGL or the full game. See
[the transform audit](PAINTCANVAS_REGION_TRANSFORM_ARM_2026-10-12.md).

```powershell
python tools/run_native_math_link_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Matrix constructor/product host smoke: OK`. The runner
compiles the production math units, checks the native default matrix,
matrix product, and pre-existing assignment/operators, and removes its
temporary executable. This does not establish ARM runtime or the full
matrix owner layout. See [the matrix audit](MATRIX_CTOR_PRODUCT_ARM_2026-10-12.md).

```powershell
python tools/run_material_draw_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `MaterialDraw host smoke: OK`. The runner executes the
production batch-draw method with fixture math/render services and
removes its temporary executable. It checks routing, colors, ambient
restoration and queue reset, not real OpenGL or ARM runtime. See
[the batch-draw audit](MATERIAL_DRAW_MATRIX_LIGHT_ARM_2026-10-13.md).

```powershell
python tools/run_sprite_system_draw_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `SpriteSystemDraw host smoke: OK`. The runner checks
the production sprite quad and matrix-queue path, including the real
cached-array template's first capacity growth, with fixture math/render
services. It removes its temporary executable. It is not a full GL,
sanitizer, or ARM32 runtime test. See
[the sprite queue audit](SPRITE_SYSTEM_DRAW_MATRIX_QUEUE_ARM_2026-10-13.md).

```powershell
python tools/run_material_lifecycle_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Material lifecycle host smoke: OK`. This compiles
the real `Material` and math units, checks the default ambient sentinel,
constructor fields, and two rounds of Array reuse, then removes its
temporary executable. It does not execute a full GL frame or ARM32
runtime. See [the ambient audit](MATERIAL_DEFAULT_AMBIENT_ARM_2026-10-13.md).

```powershell
python tools/run_mesh_clone_material_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `MeshCloneMaterial host smoke: OK`. This compiles
the production clone method against a fixture canvas and real
`Material`/`Mesh` objects, then removes the temporary executable. It
checks two clones and the invalid-index path, not a full render frame
or ARM32 runtime. See [the clone audit](MESH_CLONE_MATERIAL_ARM_2026-10-13.md).

```powershell
python tools/run_material_create_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `MaterialCreate host smoke: OK`. This extracts both
production overloads with real `Material` and Array code, fixtures
resource/texture services, and removes the temporary executable. It
does not run the real texture loader or ARM32 game. See
[the material-create audit](MATERIAL_CREATE_ARM_2026-10-13.md).

Windows note: the wrapper names are still `orbcc`, `orbnm`, `orbas`, and
`orbobjdump`, but they now also support a local NDK r18b fallback. Set
`GOF2_VERIFY_LOCAL_NDK=1` and keep the NDK at `.cache/ndk/android-ndk-r18b`, or
point `GOF2_NDK_ROOT` / `NDK` at another NDK r18b checkout.

## Why this exists / how it works

The original `libgof2hdaa.so` is ARMv7-A Thumb-2 + VFP/NEON, built with **NDK
r18b clang 7.0.2** for `armeabi-v7a`. We validate against it like this:

1. **Compile** each `src/*.cpp` with the *matching* toolchain. That toolchain only
   runs on Linux, so it lives in the **OrbStack `ubuntu`** machine and is invoked
   through `tools/verify/orbcc` (OrbStack mirrors the macOS filesystem 1:1, so
   objects land back under the repo). The per-TU build is **resilient** — a TU that
   doesn't compile yet is skipped and reported, never aborting the run.
2. **Delink** the matching originals out of the stripped, fully-linked `.so` into a
   relocatable ARM `.o` carrying the real mangled symbol names (`delink.py`).
3. **Diff**: disassemble both sides with `arm-linux-gnueabihf-objdump`, truncate to
   each symbol's real size, normalize away post-link absolute addresses, and
   compute strict fuzzy, source-shape, linked-exact and byte-exact evidence
   (`asmdiff.py`).
4. **Auto-discovery**: any mangled symbol present in *both* our object and the
   `.so` symbol table is compared — coverage grows automatically as more TUs
   compile. No function list to maintain.

> **Why not [objdiff](https://github.com/encounter/objdiff)?** Its 32-bit ARM
> disassembler caps at ARMv6K (GBA/DS/3DS). This game is ARMv7-A Thumb-2 with
> VFP/NEON, which objdiff rejects outright, so we disassemble with GNU objdump
> (full ARMv7 support) and do our own normalize + match.

## Reading the report

```
 match   source  link  unit                               symbol
  100.0%  100.0%    ==  engine/math/AEMath                 _ZN11AbyssEngine6AEMath...mlERKNS0_6VectorES3_
   67.5%   96.4%        engine/math/AEMath                 _ZN11AbyssEngine6AEMath6MatrixmLERKS1_
...
N comparisons   avg fuzzy 67.5%   avg source 96.4%   100%-fuzzy 30   byte-exact 21
coverage: compared 2311/4524 original functions   missing 2213 (-> cmake-build-match/verify/missing.txt)
report -> cmake-build-match/verify/report.json
```

- **`L` (linked-exact)** means identical machine-code bytes *outside* the relocation-covered
  fields of our unlinked `.o`. The delinked original contains post-link values in those fields.
  `asmdiff.linked_equal` masks them but does **not** compare the resolved targets. It is useful
  code-shape evidence, not a proof of byte identity after linking or semantic equivalence.
- **`==` (byte-exact)** means all raw bytes compare equal, including any relocation fields.
  It is stricter than `L` but does not by itself prove that a coincidentally equal placeholder
  resolves to the same target. Direct call destinations need the separate audit above; data
  relocations, indirect calls and runtime behavior remain additional checks.
- **`match`** is a fuzzy instruction-level score (0–100). Useful for "how close" and
  for comparing flag settings. It can read below 100% even for a byte-exact function
  when the original contains a literal pool (clang marks pools as data on our side;
  the delinked original has no such marker so objdump decodes the pool as code) — so
  trust `bytes ==` for "matched", `match` for "getting warmer".
- **`source` (`source_match`)** is an allocation-insensitive source-shape score,
  not a byte-match claim. It is the equal-weight mean of five separately recorded
  signals: duplicate-preserving instruction-shape inventory, opcode inventory,
  control-flow shape, call-count agreement, and instruction-count agreement. Control
  flow accepts either the ordered branch sequence or its duplicate-preserving inventory
  so compiler-only basic-block placement is not treated as missing behavior. It ignores
  which general-purpose register or local stack slot Clang selected and tolerates
  non-control instruction scheduling. It still penalizes changed calls, branch kinds,
  operation counts, constants, argument order and object-field offsets.
  Use it to audit recovered behavior across compiler allocation differences; use `L`
  or `==` for exact machine-code claims. Per-function components are written as
  `instruction_inventory_match`, `opcode_inventory_match`, `control_flow_match`,
  `ordered_control_flow_match`, `control_inventory_match`, `call_count_match`, and
  `size_match` in `report.json`.
- **`coverage` / `missing`** is the progress metric: how many of the original's
  `.text` functions we compared at all. *Missing* = original functions with no
  counterpart in our build — either not decompiled/compiled yet, or whose signature
  mangles differently. The original also contains statically linked C++ runtime,
  zip, compression and crypto functions; their missing names are not necessarily
  missing game code.
  The full list is written to `cmake-build-match/verify/missing.txt`.
- **`wrong type` (`missing_wrong_type`)** splits that *missing* pile into the two cases that
  look identical to an exact-name match but need very different fixes. We demangle every missing
  original and every symbol our build defines, strip the parameter list to a *qualified name*
  (`AEFile::Open`), and group by it. A missing original whose qualified name we **do** define is
  reported as *implemented under a different signature* — the body exists, only a parameter or
  qualifier type is off (`String` by-value vs `String&`, a global enum that should be nested,
  `void*` vs `char const*`), so the mangled name differs and the exact-name match skipped it.
  Inspect these before retyping: constructor/destructor C1/C2 and D1/D2 aliases
  can live in separate `.text.*` COMDAT sections. The verifier keys aliases by
  section and address. The rest are absent from the current object set, not
  necessarily absent game implementations. Both sides'
  full signatures, grouped by name, are written to
  `cmake-build-match/verify/missing_wrong_type.txt`; `report.json` carries the counts
  (`missing_wrong_type`, `missing_absent`) and the structured `wrong_type` list. Demangling uses
  `c++filt -n` (Itanium-aware on macOS — no OrbStack needed for this step).
- `report.json` has the full per-function data, `avg_match`, `avg_source_match`,
  the source-shape component scores, plus the coverage counts
  (`count`, `compared_unique`, `original_functions`, `missing`, `missing_wrong_type`,
  `missing_absent`) and the structured `wrong_type` pairings for scripting/CI.

As of 2026-09-23, `source_match` combines five equally weighted evidence
signals: allocation-normalized instruction inventory, opcode inventory,
control-flow shape, call-count agreement, and instruction-count agreement.
The report keeps both ordered control flow and duplicate-preserving control
inventory; `control_flow_match` uses the stronger of the two so compiler-only
basic-block placement does not dominate the source score. This affects only
the evidence score. Strict fuzzy, linked-exact, and byte-exact comparisons are
unchanged and remain the authority for binary matching.

## Tuning compiler flags

The canonical flag set is `tools/verify/match_flags.sh` (the *one* source of truth,
read by both `build_objs.sh` and the report driver). The biggest knob is the
optimization level:

```bash
# A/B test opt levels — re-run picks up changed flags and rebuilds:
cmake --preset match -DGOF2_MATCH_OPT=-O2
cmake --build cmake-build-match --target verify
```

`GOF2_MATCH_OPT` flows through to the build via the `match` preset. Other flags
(fpu, stack protector, API level, exceptions/rtti) live in `match_flags.sh`; edit
there and re-run `verify`. Known facts already baked in: `-mfpu=neon` (the original
uses NEON), `-D__ANDROID_API__=21` (needed for libc++ `<cmath>` to compile),
`-mthumb`, `-frtti`, `-fstack-protector-strong`, **`-DGOF2_MATCH=1`** (selects the real `Array<T>` over the dev-build
`std::vector` alias, see common.h).

> **`-Oz` vs `-O2` — settled: `-Oz`.** A clean A/B on equal coverage (2326 functions compared in
> both) gave `-Oz` 468 byte-exact / 66.1% avg vs `-O2` 444 / 54.9%; 24 functions match only at
> `-Oz` and none only at `-O2`. The original was built `-Oz`. (`match_flags.sh` defaults to it.)

> **Stack protector level - settled: `-fstack-protector-strong`.** On the same
> 4436-function corpus it raised average fuzzy similarity from `70.1%` to
> `71.64%` and linked-exact functions from 1848 to 1871, without losing a
> linked- or raw-byte-exact function. It also reproduces the exact 224-byte
> `Hud::draw` frame and canary path. See
> [HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md](HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md).

## Files

| Path | Role |
|------|------|
| `tools/verify/setup.sh` | one-time OrbStack provisioning (NDK r18b + binutils) |
| `tools/verify/prepare_work_from_so.py` | create `_work/bins` + `_work/symbols` from original `libgof2hdaa.so` |
| `tools/verify/match_flags.sh` | canonical matching compiler flags (`GOF2_MATCH_OPT`) |
| `tools/verify/build_objs.sh` | resilient per-TU ARM compile → `verify/base/*.o` |
| `tools/verify/delink.py` | extract original functions from the `.so` → `verify/target/*.o` |
| `tools/verify/asmdiff.py` | objdump-based normalize + per-symbol match |
| `tools/verify/verify.py` | orchestrator: table + `report.json`; `--show` one function |
| `tools/verify/orb{cc,as,nm,objdump}` | run NDK clang / ARM binutils in OrbStack or local NDK fallback |
| `cmake/orbstack-ndk-arm.toolchain.cmake` | the `match` preset's toolchain |

Inputs (read-only): `_work/bins/android_2.0.16_libgof2hdaa.so`,
`_work/symbols/android_2.0.16.symbols.tsv`, `_work/symbols/android_thumb_map.tsv`.
If these files are absent but the original APK `.so` is available, regenerate
them with `tools/verify/prepare_work_from_so.py`.

## Known coverage gaps

Auto-discovery matches functions by **exact mangled name**, so anything whose signature mangles
differently than the original is invisible (not wrong — just not compared):

- **`Array<T>` vs `std::vector` — RESOLVED.** The match build (`-DGOF2_MATCH`) now uses a faithful
  hand-written `Array<T>` (global template, layout `{size@0,data@4,capacity@8}`, realloc-based
  growth — bodies transcribed from the Android binary; see common.h). This mangles as `5ArrayI...`
  like the original (so the previously-invisible container functions now compare) **and** matches
  the original's element-access / iteration codegen. Switching from the `std::vector` alias moved
  the report from ~724 → 910 linked-exact and ~2327 → 2474 compared. The macOS dev build keeps the
  `std::vector` alias (the `#else` branch in common.h) for natural 64-bit development.
- TUs that don't compile under the ARM toolchain yet are skipped (see the build summary); their
  functions simply aren't compared until they build. In the 2026-09-24 verification run, the NDK r18b
  build compiles all 204 source-mapped translation units with zero failures. This confirms the
  earlier `Player`, `NewsTicker`, and `StarMap` `SolarSystem*`/`int` conflicts are resolved. Count
  source-mapped objects, not orphan `.o` files left in the base directory.

The same verification run's full-corpus report compared 4,274/4,524 original functions
across 4,613 symbols, skipped no units, and preserved all 2,037 linked-exact and
932 raw-byte-exact functions from its baseline. The focused five-symbol
`TextureCreate*` report is saved at
`_work/texture-create-arm-scalar-trial.json`; it measures 97.54% unweighted
and 93.07%
instruction-weighted source-shape. The 99% target was not reached; see
[`AEI_TEXTURE_CREATE_SOURCE_SHAPE_ARM_2026-09-24.md`](AEI_TEXTURE_CREATE_SOURCE_SHAPE_ARM_2026-09-24.md)
for per-function metrics and rejected trials.

## Direct CLI (without CMake)

```bash
python3 tools/verify/verify.py                 # build + diff everything
python3 tools/verify/verify.py --no-build      # reuse existing base objects
python3 tools/verify/verify.py --only AEMath   # filter by symbol regex
python3 tools/verify/verify.py --show <mangled-symbol>
```

Both the build and the diff fan out across `GOF2_VERIFY_JOBS` workers (default 8). The per-object
delink/objdump calls have a 90s timeout (`asmdiff.DISASM_TIMEOUT`): if an OrbStack/local tool call
wedges, that one unit is skipped with a warning instead of hanging the whole run. If you kill an
OrbStack run mid-diff, check for an orphaned `arm-linux-gnueabihf-objdump` under `orb` and `kill` it.
For `--show`/`verify-fn` and scoped family reports, run one operation at a time per translation
unit; parallel operations for the same `.o` can race while rewriting `verify/target/.../*.o`.

## Selective fionera transfer snapshot (2026-09-20)

Latest TargetFollowCamera override (2026-10-02): the fixed 40 unique bodies
meet **99.965% unweighted / 99.269% instruction-weighted** source-shape;
42 symbols including aliases meet 99.967% / 99.340%. Focused exact counts
are 32 linked / 23 byte. Both bases were remeasured with tested NEON literal
and COMDAT alias corrections; the original 27/19 and every full-corpus exact
pair are preserved. Full ARM wrong-type, 204 TUs, native build and focused host
smoke pass. This does not claim an ARM32 runtime. See
[the complete gate report](TARGETFOLLOWCAMERA_ARM_SOURCE_SHAPE_2026-10-02.md).

After rebuilding current ARM objects with NDK r18b, focused reports measured:

- `PlayerTurret`: 21 symbols, 95.5% source-shape, 12 linked-exact, four byte-exact.
- `TargetFollowCamera`: 42 symbols, 92.5% source-shape, 27 linked-exact, 19 byte-exact.
- `Gun`: 28 symbols, 72.5% source-shape, 14 linked-exact, 13 byte-exact.
- `CameraC2`: 100% strict/source and linked-exact.
- `ListItemWindow::update`: strict/source `71.7/93.6% -> 84.8/96.9%`.

These are scoped family results, not a replacement whole-corpus snapshot. ARM
layout constants independently confirm `PlayerTurret=0x168`, `Camera=0x5c`,
and the `TargetFollowCamera` tail offsets `0x120/0x130/0x138/0x13c`.
