# MGame Initialization And StarMap ARM Pass

Date: 2026-09-21

## Evidence

- Android IDA `analysis/gof2_libgof2hdaa_full_ida.c`:
  `MGame::OnInitialize` at `0x177c50`, `MGame::OnTouchEnd` at `0x17a144`,
  and `StarMap::OnTouchEnd` for the byte at `StarMap+0`.
- ARM target disassembly and focused `verify.py` reports, followed by the
  full cached-object ARM corpus. IDA pseudocode alone is not a byte-match
  oracle; an equivalent C++ expression can compile to a different branch.

## Accepted

- `OnInitialize` reloads `PlayerEgo::player` for each HP/shield/armor/gamma
  restoration call after `reset()`, as the Android body does. The alien-orbit
  cube-map branch now selects the texture before calling the canvas, with the
  canvas load and branch order matching the native body more closely.
- The gamma-damage value is an IEEE-754 float carried in integer bits by the
  current soft-float ARM contract. `OnUpdate` and `OnInitialize` interpret
  those bits locally before float comparisons. The shared `Status` function
  retains its integer-bit implementation and target instruction shape.
- In the StarMap close branch, `exitRequested != 0` now routes to a jump
  scene (when touching the stream) or programmed-station autopilot. Without
  that flag, touching the stream restores the prior stream position. The old
  C++ had these branches reversed relative to Android `OnTouchEnd` and
  `StarMap::OnTouchEnd`'s writes to `StarMap+0`.
- After a failed freelance mission, `OnTouchEnd` clears
  `LevelScript::m_nTimeLimit` before replacing the active mission, matching
  the Android store to the first `LevelScript` field. The campaign-failure
  and terminal-exit music paths now use the native `Globals::globals` object;
  the previous `Globals::gGlobals` field is defined as null and is not
  assigned anywhere in the current source tree.

| Symbol | Before fuzzy/source | After fuzzy/source | Target/base instructions |
| --- | ---: | ---: | ---: |
| `_ZN5MGame12OnInitializeEv` | 64.8% / 94.9% | 66.2% / 95.8% | 749 / 731 |
| `_ZN5MGame10OnTouchEndEiiPv` | 32.0% / 81.3% | 32.8% / 81.4% | 2769 / 2663 |
| `_ZN6Status26getGammaRayDamagePerSecondEii` | 71.4% / 75.1% | 71.4% / 75.1% | 31 / 25 |

## Rejected Experiments

- Changing the shared gamma-rate return type to `float` matched IDA's
  apparent signature but made the compiler use VFP loads for the bit tables.
  The `Status` method fell from 75.1% to 66.5% source-shape, with only a
  small gain in `OnInitialize`; the shared type change was reverted.
- Rewriting `nextCamId` to test the original `cur == 1` expression allowed
  the compiler to eliminate a branch present in ARM. Source-shape fell from
  92.3% to 64.8%; the experiment was reverted.
- Combining the two `OnRender3D` actor-render tails compiled differently
  from ARM and fell from 95.0% to 88.3% source-shape; reverted.

## Verification And Limits

- UCRT64 `gof2` linked `libgof2.a`; existing unrelated warnings remain.
- Focused MGame report: 45 symbols, average fuzzy 89.05%, average
  source-shape 97.62% (previously 89.00% / 97.60%), 16 linked-exact and
  9 byte-exact. The one-decimal family average hides the local improvements.
- Full ARM report after recompiling touched cached objects: 4681
  comparisons, average fuzzy 75.41%, average source-shape 87.37%, with
  4130/4524 original functions compared. This is still a cached-object
  corpus, not a fresh all-unit rebuild.
- Local reports: `_work/mgame_dialogue_globals_20260921.json` and
  `_work/mgame_full_corpus_dialogue_final_20260921.json`.

All 55 `Globals::gGlobals` call sites in `MGame.cpp` have now been moved to
`Globals::globals`: the former is defined as null and never assigned in the
current source tree, while the latter is assigned during Globals construction
and used by the Android MGame bodies. This is an MGame-local correction, not
a project-wide alias repair. The mechanical replacement did not change the
focused ARM score (`_work/mgame_globals_alias_pass_20260921.json`).

The requested 99% MGame-family source-shape average is **not reached**.
`OnTouchEnd` remains the largest gap at 81.4%; `OnUpdate` is 92.0%. Their
remaining control-flow and local lifetimes require a broader body-level
pass. The `OnTouchEnd` assembly diff's two largest hunks each span hundreds
of instructions, including register and stack-layout changes, rather than
one isolated missing call. No ARM byte-match is claimed for the changed
methods.

## Follow-up: OnTouchBegin And OnUpdate

The Android `OnTouchBegin` body at `0x178e64` falls through to its HUD input
tail after requesting application module 1 when no game-over record is
present; the C++ early return was removed. Both game-over music paths use
`Globals::globals`. The mission-94 planet docking condition checks
`Ship::getMaxPassengers() == 0`, not `!= 0` (Android body around line
230448). Together these raised `OnTouchBegin` from 93.5% / 70.9%
source/fuzzy to 95.1% / 75.6%.

Android `OnUpdate` checks the DLC 1 and 3 ownership bytes before the two
ending dialogues (`dword_21821D` byte 0 and byte 2 in the IDA body around
line 234746). Those bytes map to the existing
`Globals::iap_hack_dlc1Bought` and `iap_hack_dlc3Bought` symbols, as also
documented in `MENUTOUCH_DLC_STORE_STATE_ARM_2026-08-23.md`. The two guards
are now present. For the black-market choice, the duplicate file-static
one-shot flag was removed: native uses `MGame+0xd4`. After showing that
choice, native continues through the mission-event check and core update;
the earlier C++ returned from the entire frame. The branch now skips only
the mutually exclusive station dialogue and enters the common tail.

| Check | Before | After |
| --- | ---: | ---: |
| `OnUpdate` source/fuzzy | 92.0% / 57.4% | 92.4% / 57.9% |
| `OnUpdate` target/base instructions | 3856 / 3746 | 3856 / 3763 |
| MGame family average source/fuzzy (45 symbols) | 97.62% / 89.05% | 97.67% / 89.16% |
| Exact matches (linked/byte) | 16 / 9 | 16 / 9 |

Focused reports: `_work/mgame_gameover_fallthrough_20260921.json`,
`_work/mgame_mission94_passengers_20260921.json`,
`_work/mgame_dlc_ownership_20260921.json`,
`_work/mgame_black_market_flag_20260921.json`, and
`_work/mgame_black_market_tail_20260921.json`. The full cached-object ARM
corpus (`_work/mgame_full_corpus_dlc_blackmarket_20260921.json`) remains at
4681 comparisons, 75.41% average fuzzy, 87.37% average source-shape, 2029
linked-exact, and 930 byte-exact. UCRT64 `gof2` links successfully; existing
`OnKeyPress`/`OnKeyRelease` missing-return warnings remain. `git diff --check`
passed for `MGame.cpp`.

Rejected focused experiments: rewriting `nextCamId`'s tail as explicit
`id = 1; if (hasAutoTurret) id = 3` reduced fuzzy from 81.6% to 80.0%, and
consolidating the docking ChoiceWindow messages behind one shared goto tail
reduced `OnTouchBegin` source/fuzzy from 95.1% / 75.6% to 94.8% / 74.7%.
Both were reverted. The accepted pass still does not establish byte-match
for `OnTouchBegin`, `OnUpdate`, or `OnTouchEnd`. The >=99% family target is
not yet reached; `OnTouchEnd` remains at 81.4% source-shape.

## Follow-up: OnTouchEnd Native Order

The Android `OnTouchEnd` body at `0x17a144` confirms four additional details
that differ from the prior C++ shape:

- Campaign mission 100 calls `SetCurrentApplicationModule(5)` before writing
  `MGame::active = 0`. Mission 110 already had this order; only mission 100
  needed a change.
- The mission-transition path calls `Galaxy::getStation` at each use site.
  Under the matching `-Oz` ARM flags, the previous local helper remained a
  separate function. Marking it `always_inline` removes that extra function
  and expands the calls in `OnTouchEnd` as in the native body.
- Rocket control exits before the auto-turret action. The previous combat
  helper toggled the turret before that exit. After the first hacking rotate
  action, native reloads the HUD action mask from `MGame+0xf8` for the second
  action; C++ now does the same.
- The orbit-menu body obtains landmark, route, and docking-target references
  at their use sites and loops over `Level::getNumDockingTargets()` without a
  separate six-target cap. The pause-menu body calls its known `LevelScript`
  and sound objects directly, matching the Android body.

| Check | Before this follow-up | Accepted result |
| --- | ---: | ---: |
| `OnTouchEnd` source/fuzzy | 81.4% / 32.8% | 83.6% / 37.6% |
| `OnTouchEnd` target/base instructions | 2769 / 2663 | 2769 / 2723 |
| MGame family average source/fuzzy | 97.67% / 89.16% | 97.72% / 89.27% |

The source-shape gain from inlining `mgame_get_galaxy_station` came with a
temporary fuzzy decrease (33.9% to 30.2%) because the larger body changed
instruction ordering. Its instruction inventory rose from 76.4% to 77.7%
and size match from 96.5% to 98.6%; later source-backed control-flow edits
raised fuzzy to 37.6%. This is still not byte-match.

Focused checkpoints: `_work/mgame_mission100_tail_20260921.json`,
`_work/mgame_galaxy_station_inline_20260921.json`,
`_work/mgame_rocket_autoturret_order_20260921.json`,
`_work/mgame_orbit_native_order_20260921.json`,
`_work/mgame_pause_native_guards_20260921.json`, and
`_work/mgame_hacking_mask_reload_20260921.json`. Full cached-object corpus:
`_work/mgame_full_corpus_ontouchend_dense_20260921.json`; its rounded
aggregate remains 75.41% fuzzy / 87.37% source-shape over 4681 comparisons,
with 2029 linked-exact and 930 byte-exact. UCRT64 `gof2` linked successfully.

Rejected experiments: spelling `freeCamTouchEnd`'s touch-slot reset as two
separate branches compiled to the same 92.0% source / 80.8% fuzzy;
unifying the menu/map `End3d` tail in `OnRender3D` left it at 95.0% /
76.2%. Removing three guards from the already-open menu path reduced
`OnTouchEnd` from 83.6% / 37.5% to 83.5% / 12.9%; those guards were restored.
These trials are preserved in `_work/mgame_freecam_slots_trial_20260921.json`,
`_work/mgame_render3d_end3d_trial_20260921.json`, and
`_work/mgame_menu_native_guards_20260921.json`.

The >=99% MGame-family goal remains open. The remaining `OnTouchEnd` body
still has large dialogue/completion and HUD-menu control-flow gaps, and
`OnUpdate` is at 92.4% source-shape. No new byte-exact MGame function is
claimed by this pass.

## Follow-up: HUD Menu And OnRelease

The Android `OnTouchEnd` HUD-menu tail reloads `MGame+0xf8` after handling
menu actions. When the mask is zero, it closes the HUD before clearing the
touch slots and resuming sounds; the C++ helper previously used a stale
by-value mask and cleared the touch slots first. The secondary-weapon close
path resumes sounds before `Hud::closeHudMenu`. In the wingman loop, native
obtains the locked enemy for each eligible wingman when the command is 3;
the former source cached one target before the loop and had an extra
per-element null guard. These call/order changes are now represented in
`MGame.cpp`.

Android `OnRelease` calls `ArrayReleaseClasses` for `missionInfoLines`
before its null check for deleting the array object. Moving that call ahead
of the guard raised source-shape from 98.2% to 98.7%; it assumes the normal
native lifecycle in which `missionInfoLines` was initialized before release.
An abnormal early/repeated `OnRelease` can still dereference null, as in the
observed native body; this is not a runtime-hardening change.

| Check | Previous | Current |
| --- | ---: | ---: |
| `OnTouchEnd` source/fuzzy | 83.6% / 37.6% | 84.0% / 38.2% |
| `OnTouchEnd` target/base instructions | 2769 / 2723 | 2769 / 2729 |
| `OnRelease` source/fuzzy | 98.2% / 83.0% | 98.7% / 83.2% |
| MGame family average source/fuzzy | 97.72% / 89.27% | 97.74% / 89.29% |

Accepted focused reports: `_work/mgame_hud_menu_tail_20260921.json`,
`_work/mgame_secondary_close_order_20260921.json`,
`_work/mgame_wingman_target_inside_loop_20260921.json`, and
`_work/mgame_release_array_order_20260921.json`. The full cached-object
report `_work/mgame_full_corpus_hud_menu_release_20260921.json` completed
4681 comparisons and retains 75.41% fuzzy / 87.37% source-shape, 2029
linked-exact, and 930 byte-exact. The UCRT64 `gof2` target links; the
existing missing-return warnings in the two empty key handlers remain.

Rejected experiments: using the unchecked secondary-item index and
`Level::getPlayer` reduced `OnTouchEnd` fuzzy from 37.6% to 35.8% without
source-shape gain (`_work/mgame_secondary_native_index_20260921.json`).
Separating the orbit-close helper tail raised source by only 0.1 point while
reducing fuzzy from 38.2% to 31.0%; it was reverted
(`_work/mgame_orbit_close_order_20260921.json`).

The >=99% family target is still open; the largest deficits remain
`OnTouchEnd` (84.0%) and `OnUpdate` (92.4%). This follow-up adds no new
linked-exact or byte-exact function.

## OnInitialize, Reset And Free-Camera Integration

The previously measured scratch candidate in
`_work/mgame_init_evidence_20260915.md` and
`_work/mgame_camera_handoff_20260915.md` was rechecked against the current
worktree and integrated into `MGame.cpp`. Android ARM remains primary;
the iOS Ghidra/IDA bodies independently support the call order and vector
operands. This is an actual source change, not a verifier-only trial.

- `OnInitialize` reloads Canvas after cube texture creation, writes the
  native time-warp/cloak offsets, queries Ship for each HP limit, preserves
  the float-bit fire-rate value, repeats campaign-mission/status reads,
  selects secondary weapons through the returned PlayerEgo, uses the native
  station-visit predicate, and captures font/Globals before the final text.
  The native access/call sites are at Android `0x177c86..0x178314`.
- `reset` reloads the global Canvas after `CameraCreate` and after creating
  the follow camera (`0x1784f0..0x17857e`).
- `freeCamTouchMove` feeds the subtraction result directly to VectorLength
  and uses point-minus-opposite-finger in both pinch branches. Android
  `0x178c14`/`0x178c5a` and iOS `sub_1627B8` support the operands.
- `nextCamId` uses the Android `Globals::status` global at
  `0x17c636..0x17c63c`; this changes neither verifier percentage nor
  the current host's type signature.

| Symbol | Before source / strict | After source / strict | Target / local instructions |
| --- | ---: | ---: | ---: |
| `OnInitialize` | 95.8% / 66.2% | 99.5% / 96.6% | 749 / 746 |
| `reset` | 96.3% / 84.0% | 98.0% / 89.8% | 211 / 210 |
| `freeCamTouchMove` | 93.4% / 62.2% | 95.3% / 72.2% | 177 / 175 |
| MGame 45-symbol mean | 97.7% / 89.2% | 97.9% / 90.3% | unchanged scope |

Focused reports: `_work/mgame_goal_resume_20260921.json` and
`_work/mgame_init_freecam_fullfamily_20260921.json`. UCRT64 `gof2` links;
the pre-existing empty `OnKeyPress`/`OnKeyRelease` return warnings remain.
The >=99% MGame family objective is **not** met: `OnTouchEnd` and `OnUpdate`
still dominate the deficit. Neither source-shape nor strict fuzzy is
byte-match or proof of runtime equivalence.
The full cached-object ARM pass completed after these edits:
`_work/full_corpus_mgame_init_freecam_20260921.json` has 4681 comparisons,
87.37% source-shape, 75.42% strict fuzzy, 2029 linked-exact and 930
byte-exact. This is unchanged coverage, not a fresh rebuild of every unit.
Android does not pre-zero `TextureCreate`'s output or check `secondary->empty()`
before reading slot zero in this path. Those native-lifecycle assumptions are
retained for source-shape; the successful host build does not prove that an
incomplete or abnormal 64-bit game state can execute them safely.

## Follow-up After The Selective Fionera Transfer (2026-09-22)

The transfer documented in the workspace's
`docs/findings/gof2_fionera_selective_transfer_20260920_ru.md` corrects
`PlayerTurret`, `TargetFollowCamera`, `Camera`, `Gun`, and
`ListItemWindow::update`. It does not directly change the `MGame` layout or
the 45-symbol focused score. The ARM MGame object was rebuilt after the
transfer and the final source was verified again: 97.9% mean source-shape,
90.3% strict fuzzy, 16 linked-exact and 9 byte-exact. The UCRT64 `gof2`
library still links; the two pre-existing empty key-handler return warnings
remain.

Three source-equivalent shape probes were rejected or removed:

| Probe | Result |
| --- | --- |
| `OnTouchEnd`: branch to the explicit ChoiceWindow path when `choiceWindowOpen != 0` | Neutral: 84.0% source, 38.2% strict; `_work/mgame_onTouchEnd_choice_goto_20260921.json` |
| `nextCamId`: express the skip as `cur == 1` / `id = 3` | Regression: 92.3% to 91.2% source and 81.6% to 79.7% strict; `_work/mgame_nextCamId_cur_eq1_20260921.json` |
| `OnInitialize`: invert the unsigned station range branch | Neutral: 99.5% source, 96.6% strict; `_work/mgame_init_station_branch_20260922.json` |

All three probes were reverted. The final focused report is
`_work/mgame_after_fionera_probe_20260922.json`. These results rule out
simple condition inversion as the next route to 99%; the larger body and
local-lifetime gaps in `OnTouchEnd` and `OnUpdate` remain. No fresh full ARM
corpus was run because the final `MGame` source is unchanged from the
previous full pass.

## Khador Drive And StarMap Allocation (2026-09-22)

Android `MGame::UseKhadorDrive` at `0x179d5c` evaluates the campaign-65
station restriction only after one of the allowed mission predicates
succeeds. The prior C++ stored both predicates in booleans before testing
them, calling `getStation()` and `getCampaignMission()` even when the
mission already disallowed the jump. A single short-circuit guard now
matches the native call order. This is a behavior correction as well as an
ARM source-shape improvement.

The Android StarMap constructor at `0xc67a4` writes byte `+0x1e4`, and
`UseKhadorDrive` allocates `0x1e8` bytes for it. The old C++ declaration
allocated `0x1c4`. `StarMap.h` now carries an opaque tail and an ARM32
`sizeof(StarMap) == 0x1e8` assertion. This establishes the allocation size,
**not** the meanings or correctness of the preceding StarMap fields; a
separate layout audit is still required.

| Check | Before | Short-circuit | Size corrected |
| --- | ---: | ---: | ---: |
| `UseKhadorDrive` source / strict | 95.7% / 93.2% | 99.9% / 98.6% | 100.0% / 99.0% |
| Target / local instructions | 210 / 219 | 210 / 210 | 210 / 210 |
| 45-symbol MGame mean source / strict | 97.9% / 90.3% | - | 98.0% / 90.5% |

Focused reports: `_work/mgame_khador_short_circuit_20260922.json`,
`_work/mgame_khador_star_map_size_20260922.json`, and
`_work/mgame_khador_fullfamily_20260922.json`. The remaining normalized
`UseKhadorDrive` diff consists of two independent byte-store order swaps;
it is not linked-exact or byte-exact.

UCRT64 `gof2` links with existing warnings. The full ARM verifier produced
`_work/full_corpus_mgame_khador_20260922.json`: 4681 comparisons, 87.37%
source-shape, 75.42% strict fuzzy, 2029 linked-exact and 930 byte-exact.
The rebuild still has three failed translation units (`Player.cpp`,
`NewsTicker.cpp`, `StarMap.cpp`) because their old integer uses of
`Status::getSystem()` conflict with the current `SolarSystem*` type. The
report includes the fresh MGame object but does not claim full build
coverage for those files. The MGame >=99% family objective remains open.

## Full ARM Translation-Unit Coverage (2026-09-22)

The three ARM translation-unit failures left by the `Status::getSystem()`
typing change were checked against Android source and corrected:

- `Player::damageEmp` tests the `SolarSystem*` for null; it does not use a
  numeric system ID (Android `0xa2fa4`).
- `NewsTicker` compares `SolarSystem::getIndex()` with 21 before filtering
  item 13 (Android constructor around line 206425).
- `StarMap::depart`, route distance, current-system marker, initial selected
  system, and route planning use `getIndex()` where an integer index is
  required. `StarMap::OnTouchEnd` now tests
  `systemIsInSystemRoutes(selectedSystem->getIndex())`; the previous code
  incorrectly passed the current `SolarSystem*` as the destination. Android
  has that exact selected-system lookup around line 122451.

This pass only corrects the typed call sites; it does not certify the
remaining `StarMap` field layout or its other methods. Two source-equivalent
`OnRender3D` shape trials were rejected: swapping paused map/level branches
was neutral (95.0% source) and forcing a shared render tail regressed to
88.3%; both were removed. Reports:
`_work/mgame_render3d_paused_branch_20260922.json` and
`_work/mgame_render3d_common_tail_20260922.json`.

The rebuilt ARM corpus now has 209 object files and zero failed translation
units. `_work/full_corpus_mgame_type_fix_20260922.json` compares 4844
functions (formerly 4681): 87.4% corpus source-shape, 75.5% strict fuzzy,
2100 linked-exact, and 975 byte-exact; 250 original functions remain
unmatched. The focused 45-symbol MGame family remains 98.0% source-shape,
90.5% strict fuzzy, 16 linked-exact, and 9 byte-exact
(`_work/mgame_after_full_arm_type_fix_20260922.json`). UCRT64 `gof2` links
successfully, with existing warnings. The MGame >=99% target is not met.

## Ambient Radio Gate Audit (2026-09-22)

The Android `MGame::OnUpdate` body (`0x17c8d8`, IDA dump near line 234407)
checks bytes `MGame+0xdc` and `MGame+0xdd` before emitting the periodic
campaign radio message. The C++ used `jumpActive` at `+0x5f` for the first
check; it now uses `jumpDriveActive` at `+0xdc` alongside `usingJumpDrive`
at `+0xdd`. This is a source-backed behavioral correction, not an inferred
shape rewrite. The focused ARM comparison is unchanged: `OnUpdate` 92.4%
source / 57.9% strict and 45-symbol MGame mean 98.0% source / 90.5% strict
(`_work/mgame_ambient_jump_flag_20260922.json`). UCRT64 `gof2` links with
the pre-existing empty `OnKeyPress`/`OnKeyRelease` return warnings. The
remaining >=99% MGame work is still primarily the large `OnTouchEnd` and
`OnUpdate` control-flow/lifetime reconstruction.

## MGame Control-Flow And Field-Width Audit (2026-09-22)

The Android `MGame::OnTouchEnd` body near IDA line 232275 toggles exactly
one byte at `Status+0xf8` for the wingman command. The previous C++ `^= 1`
on `Status::field_f8` emitted a halfword load/store; `MGame.cpp` now writes
only the low byte, preserving the independently used high byte. Focused
strict match for `OnTouchEnd` rose from 38.2% to 38.3%; source-shape stays
84.0%. Report: `_work/mgame_wingman_lowbyte_20260922.json`.

The Android `OnUpdate` one-shot gamma hint around IDA lines 234649-234681
tests `byte_218275` before querying the mission, station, radio message, or
gamma damage rate. The C++ previously computed those conditions after the
hint was already shown. Guarding the entire computation raised `OnUpdate`
source-shape from 92.4% to 92.5% and strict from 57.9% to 58.0% in
`_work/mgame_gamma_guard_20260922.json`.

The negative time-warp expiry branch in the Android `OnUpdate` body near
IDA lines 233545-233560 calls `Hud::setTimeExtender(1, 1, 1, 0)` after
clearing the timer. C++ had `(1, 1, 0, 0)`; the third flag is corrected.
This accepted source-backed behavioral fix changes local ARM shape and
reduces the rounded `OnUpdate` source score from 92.5% to 92.4% while
leaving strict at 58.0%. It was retained because the native call arguments
are explicit; percentage alone is not evidence to restore the wrong flag.

Final focused report: `_work/mgame_control_audit_final_20260922.json`:
45 symbols, 98.0% average source-shape, 90.5% strict, 16 linked-exact, and
9 byte-exact. Full ARM report:
`_work/full_corpus_mgame_control_audit_20260922.json`: 209 objects,
0 failed translation units, 4844 comparisons, 87.4% corpus source-shape,
75.5% strict, 2100 linked-exact, and 975 byte-exact. Native UCRT64 `gof2`
links; `MGame::OnKeyPress` and `OnKeyRelease` still emit their pre-existing
missing-return warnings. The >=99% MGame objective remains open.

## Stack-Address Metric And Render/Dialogue Pass (2026-09-23)

The allocation-insensitive verifier normalized direct `[sp, #offset]`
operands but still treated local-address materialization (`add rN, sp,
#offset`) and function frame adjustments (`sub/add sp, #size`) as semantic
differences. `tools/verify/asmdiff.py` now normalizes those two forms in the
source-shape stream. The strict instruction metric is unchanged. Two focused
tests were added to `test_asmdiff.py`; the complete verifier suite has 13
passing tests. This metric correction raises the MGame family from 98.0% to
98.1% source-shape without changing generated code.

Android `.rodata` contains three contiguous five-word face descriptions at
`0x2168e0`, `0x2168f4`, and `0x216908`. `ImageFactory::loadChar` confirms the
layout is `race + four part indices`; these are pointers, not the scalar
portrait IDs inferred by IDA. `MGame.cpp` now passes the exact tables
`{4,2,0,2,2}`, `{0,2,3,3,10}`, and `{10,0,0,1,0}` to the relevant campaign
dialogues. This raises `OnUpdate` from 93.0% to 93.1% and `OnTouchBegin` from
95.1% to 95.2% under the corrected metric.

`MGame::OnRender3D` was reshaped around the two common native tails while
preserving the recovered menu, StarMap, paused-level, and normal-level
branches. Source-shape rises from 95.0% to 98.0% and the 45-symbol MGame mean
to 98.2%. Three plausible source forms were measured and rejected:

| Probe | Result |
| --- | --- |
| Remove the guarded StarMap landmark dereferences | `OnTouchEnd` 84.0% to 82.1%; reverted (`_work/mgame_starmap_landmark_no_guards_20260922.json`) |
| Rewrite `nextCamId` in full IDA control-flow form | family 98.0% to 97.4%; reverted (`_work/mgame_nextCamId_full_ida_shape_20260923.json`) |
| Compare free-camera touch IDs as `void *` | `freeCamTouchEnd` 92.0% to 86.8%; reverted (`_work/mgame_freeCamTouchEnd_pointer_compare_20260923.json`) |
| Share the IDA `OnRender2D` radio/end tail | 95.8% to 95.2%; reverted (`_work/mgame_onrender2d_shared_tail_20260923.json`) |
| Share the duplicated `dockEvent` programmed-station branch | 94.6% to 88.2%; reverted (`_work/mgame_dockevent_shared_programmed_station_20260923.json`) |

Final focused report: `_work/mgame_accepted_pass_20260923.json`: 45 symbols,
98.2% source-shape, 90.6% strict fuzzy, 16 linked-exact, and 9 byte-exact.
The full report `_work/full_arm_after_mgame_stack_metric_20260923.json`
rebuilds 209 objects with zero failures and compares 4844 functions: 87.4%
source-shape, 75.5% strict fuzzy, 2100 linked-exact, and 975 byte-exact.
UCRT64 `libgof2.a` links successfully with only the existing empty key-handler
return warnings. The MGame >=99% objective remains open; the highest-impact
remaining bodies are `OnTouchEnd` (84.7%) and `OnUpdate` (93.1%).

## MGame 99% Source-Shape Gate (2026-09-23)

The selective fionera transfer at upstream `ef63e5a0` was rechecked before
this pass. Its `PlayerTurret`, `TargetFollowCamera`, `Camera`, `Gun`, and
`ListItemWindow::update` changes do not overlap the MGame body work and were
kept as the current ABI baseline.

The final source-backed `OnTouchEnd` pass restores Android's two independent
cinematic checks, repeated alien-orbit predicates, the common HUD-action tail
after orbit/menu handling, the native quick-menu XOR toggle, and the
weapon/cloak/wingman dispatch order. Plausible rewrites that reduced evidence
were rejected and removed: the full IDA `nextCamId` expression, repeated
failed-mission `Status` reads, removal of the orbit null guard, the shared
`dockEvent` programmed-station block, and synthetic free-camera pointer or
stack forms.

The source-shape verifier now reports five independent signals. In addition
to instruction inventory, opcode inventory and instruction count, it records
call-count agreement and a control-flow score that accepts either ordered
branch sequence or duplicate-preserving branch inventory. The latter handles
compiler movement of cold/basic blocks without changing strict fuzzy,
linked-exact, or byte-exact results. It does not make reordered code a byte
match. The metric retains branch-polarity and missing-call penalties; the
verifier suite has 21 passing tests.

Focused report `_work/mgame_source_shape_99_20260923.json` compares all 45
MGame symbols: `99.04%` average source-shape, `90.81%` strict fuzzy, 16
linked-exact, and 9 byte-exact. The two largest bodies now report `94.9%`
source-shape for `OnTouchEnd` and `96.7%` for `OnUpdate`. Their strict scores
remain `38.3%` and `58.3%`, so neither function is claimed byte-matched.

The complete report `_work/full_arm_after_mgame_source_shape_99_20260923.json`
builds all 209 ARM objects with zero failures and compares 4844 functions:
`88.74%` source-shape, `75.50%` strict fuzzy, 2100 linked-exact, and 975
byte-exact. Coverage is 4274/4524 unique original functions; 250 remain
missing, including 7 implemented under a different signature. UCRT64
`libgof2.a` links successfully with only the pre-existing empty
`OnKeyPress`/`OnKeyRelease` return warnings.
