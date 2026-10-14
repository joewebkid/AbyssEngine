# ASM Validation: original `.so` ↔ our build

> Read [PROJECT_STATUS.md](PROJECT_STATUS.md) before interpreting a saved
> metric. `report.json` is a dated snapshot; a fresh local verifier run is the
> authoritative result for the revision and environment being inspected.

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
