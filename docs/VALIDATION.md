# ASM Validation: original `.so` ↔ our build

> Read [PROJECT_STATUS.md](PROJECT_STATUS.md) before interpreting a saved
> metric. `report.json` is a dated snapshot; a fresh local verifier run is the
> authoritative result for the revision and environment being inspected.

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

## PlayerEgo emergency timer and hacking update (2026-10-14)

Android Thumb `0x9cfe2–0x9d19e` confirms the emergency timer and
the win/reward/cleanup paths after `HackingGame::update`. The five
reward IDs come from Android `.rodata` at `0x1fba34`. Extracted
production-fragment smoke covers the timer crossing, normal hacking,
win without reward, first flagged enemy cleanup, and a null enemy
list. Native `gof2`, 206/206 ARM units, 37 verifier tests (one
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
String/Array paths and probe-module vtable dispatch on the host. It is not
an ARM32 runtime pass; see [the ABI gate](ABI_RUNTIME_GATE_2026-09-27.md)
for the original vtable relocations and remaining ownership risks.

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

- **`L` (linked-exact)** is the real matching signal: the machine code is byte-identical to
  the original **after linking** — i.e. identical except inside relocation-covered fields (call
  targets, GOT/literal-pool addresses), which our unlinked `.o` leaves as placeholders while the
  delinked target carries the linker-resolved value. This is what to chase. (`asmdiff.linked_equal`
  masks the relocation byte-ranges, read from our object's `-r` table, before comparing.)
- **`==` (byte-exact)** is the rarer strict subset: raw bytes identical with *no* relocations at
  all — only leaf functions that never call out or load a global can reach it. A function can be a
  perfect match (`L`) yet not `==` purely because it makes a call; don't chase `==`, chase `L`.
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
