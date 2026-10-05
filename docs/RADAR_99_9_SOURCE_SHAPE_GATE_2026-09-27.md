# Radar 99.9% source-shape gate (2026-09-27)

The current 19-symbol Radar family measures **99.5684% average source-shape**
with NDK r18b Clang 7, `-Oz`, Thumb ARMv7 and the canonical match flags.
This is below the requested 99.9% gate. The scoped baseline is
`_work/radar_goal_baseline_20260927.json`; after restoring the neutral trial,
`_work/radar_goal_restored_20260927.json` reproduces the same result. Both are
local generated reports, not repository artifacts.

| Function | Source-shape | Strict | Target / local instructions |
| --- | ---: | ---: | ---: |
| `Radar::draw` | 96.8% | 47.9% | 3486 / 3546 |
| `Radar::drawCurrentLock` | 98.0% | 93.6% | 622 / 635 |
| `Radar::Radar` (`C1`, `C2`) | 98.9% each | 92.1% each | 797 / 803 each |
| `elipsoidIntersect` | 99.3% | 81.7% | 60 / 60 |
| `calcDistance` | 99.8% | 95.1% | 224 / 224 |
| Other 13 functions | 100% | 100% | unchanged |

The average is unweighted by function size. The five distinct non-perfect
bodies, including both constructor aliases, account for the remaining gap.
The 13 perfect functions include 13 linked-exact and eight raw-byte-exact
functions. Neither the family average nor the `draw` score proves behavior or
runtime equivalence.

After the rejected trial was removed, all 204 current ARM translation units
compiled. The unscoped `--fail-on-wrong-type` corpus report at
`_work/full_arm_radar_baseline_20260927.json` completed with 4,626 comparisons,
89.50% corpus source-shape, 76.49% strict, zero skipped and zero
`missing_wrong_type`. This broad report is a regression gate, not a Radar
completion result. The UCRT64 `gof2` target linked `libgof2.a` successfully.

## Evidence and trial

Android Hex-Rays constructor at `0x12d484` stages the image height reciprocal
before storing the image squares, while the recovered source computes both
squares before the two weights. A source-only trial in `Radar.cpp` named the
width/height squares and staged the height reciprocal before the stores. The
focused verifier returned **99.56% family / 98.9% constructor**, unchanged from
baseline; strict, instruction counts, linked-exact and byte-exact counts also
remained unchanged. The trial was rejected and removed. This does not establish
that either source order is the original C++ order.

The Android `elipsoidIntersect` body at `0x12df48` confirms the existing
coordinate/result pairing. Its remaining 60-instruction diff largely changes
register and FP scheduling. Swapping the output coordinates just to align a
register would contradict the visible native return-slot writes. Earlier
reordering trials are recorded in `_work/radar_leaf_handoff_20260915.md`.

The current `drawCurrentLock` diff changes temporary `String` stack placement,
GOT reload lifetime and a literal-pool boundary; the first large `draw` diff
is block scheduling across station/enemy/gas/asteroid phases. These are leads,
not permission to add fake stack locals, spills, or unrelated calls.

## Direct normal-label condition follow-up

Android `drawCurrentLock` at `0x130998` evaluates the normal/red label test
directly before the string assignment. Removing a source-only `bool` staging
variable from that test preserved the branch predicate and improved the body
from **98.0% to 98.1% source-shape** (635 to 634 local instructions, against
622 target). Strict remains 93.6%; this is not linked- or byte-exact. The
19-symbol mean changes from **99.5579% to 99.5632%**. The focused report is
`_work/radar_goal_direct_label_accepted_20260927.json`; the preceding trial
report is `_work/radar_normal_label_direct_trial_20260927.json`.

Rejected: explicit 64-bit half-coordinate locals in `calcDistance` preserved
the numerical formula but moved the conversion schedule away from the Android
body at `0x130704`. It reduced that function from **99.7% / 91.3%** to
**99.0% / 65.8%** source/strict and was removed. Trial report:
`_work/radar_distance_int64_trial_20260927.json`. The earlier constructor
weight-order trial remains rejected. iOS/PC material does not independently
establish ARM instruction scheduling for either source-shape experiment.

After restoring the rejected distance trial, all 204 ARM units compile. The
full `--fail-on-wrong-type` report
`_work/full_arm_radar_direct_label_20260927.json` contains 4,626 comparisons,
zero wrong-type misses, 2,077 linked-exact and 945 byte-exact functions;
UCRT64 `gof2` links `libgof2.a`. The requested 99.9% Radar family gate is
still open. `draw` (96.8%) and the two constructors (98.9% each) dominate the
remaining gap. Next work should compare a bounded native
basic-block cluster within `draw`, especially the station/enemy join and
String return-slot lifetime, rather than perturbing already exact leaves.

## Distance conversion and draw-frame follow-up

Android `calcDistance` at `0x130704` expresses signed 64-bit conversions of
half-coordinate differences inside its sum of squares. Keeping that
arithmetic order but writing the conversions directly, without three
long-lived `float` locals, improved **99.7% / 91.3%** to **99.8% / 95.1%**
source/strict. Target and local both contain 224 normalized instructions;
the function is not linked- or byte-exact. Focused report:
`_work/radar_distance_inline_accepted_20260927.json`. The 19-symbol mean
rose from 99.5632% to **99.5684%**. The still-exact 13 functions did not
regress.

Rejected trial: staging only the Y difference produced the same focused
scores and added no evidence for a closer source form, so it was removed
(`_work/radar_distance_y_local_trial_20260927.json`). A four-quadrant prelude
trial cached width/height and center pairs for the first image draw; it left
`draw` at 96.8% source-shape but lowered strict from 47.9% to 44.7%, so it
was also removed (`_work/radar_draw_quad_pair_trial_20260927.json`). Neither
trial justified retaining extra locals.

The Android `draw` body at `0x12e050` has a 296-byte ARM frame, saved
`d8-d13`, six 12-byte by-reference `Vector` slots and one 60-byte `Matrix`
slot in the IDA stack view. The current compiled body uses a 224-byte frame
and saves `d8-d11`. These observations identify a station/enemy return-slot
and local-lifetime investigation, **not** a reason to synthesize stack data.
The exact ownership and overlapping lifetimes of IDA's named slots need ARM
call-site inspection before any source rewrite. Available iOS and PC material
does not independently verify this Android ARM allocation.

After the accepted distance form, all 204 ARM translation units compile. The
full `--fail-on-wrong-type` corpus run exited successfully with 4,626
comparisons, zero wrong-type misses, 2,077 linked-exact and 945 byte-exact
functions: `_work/full_arm_radar_distance_inline_20260927.json`. UCRT64
`gof2` again links `libgof2.a`. The 99.9% gate remains open by **0.3316
average points**.

## Next acceptance gate

1. Identify one specific Android ARM basic block and its iOS/PC cross-check;
   preserve confirmed behavior while changing the corresponding source body.
2. Run focused `--show` and the 19-symbol family report before and after.
   Accept only a measurable source-shape gain with no regression in exact
   functions or demonstrated behavior.
3. Rebuild all current ARM translation units, run an unscoped corpus report
   with `--fail-on-wrong-type`, and keep the UCRT64 `gof2` build green.
4. Recompute the exact family average from a fresh report. Do not close this
   gate until it is at least 99.9% and the broad checks pass.

## 2026-09-29 workspace relocation and draw callsite audit

The workspace moved to `A:\Galaxy on Fire new version`. The historical
`_work/bins` and `_work/symbols` verifier inputs were absent after the move;
`tools/verify/prepare_work_from_so.py` regenerated them from the surviving
Android `analysis/gof2_engine_audit/extracted_apk_runtime/libgof2hdaa.so`
(4,524 exported function symbols). No Radar source was changed in this pass.

The refreshed 19-symbol report, `_work/radar_resumed_baseline_20260929.json`,
reproduces **99.5684% source-shape / 94.87% strict**, 13 linked-exact and
eight byte-exact functions. The full report
`_work/full_arm_radar_resume_20260929.json` completed 4,626 comparisons,
zero skipped and zero wrong-type misses, with 2,078 linked-exact and 945
byte-exact functions. A fresh UCRT64 CMake configuration and `gof2` build
linked `libgof2.a`; unrelated existing compiler warnings remain.

To narrow the large `draw` diff, direct ARM branch destinations in the
delinked Android body were rebased from `0x12e050` and resolved against the
IDA function-address table. Their call-target multiset was compared with
`R_ARM_THM_CALL` relocations in the local object, normalizing `C1`/`C2`
constructor aliases. Of 344 original and 341 local direct callsites, the
original has exactly one additional physical call to each of
`Level::getPlayer` (29 vs 28), `PaintCanvas::DrawImage2D(Ejiihh)` (12 vs 11),
and `FModSound::play` (7 vs 6). No other direct-call target differs in count.
This is a code-generation fact, **not** proof that three actions are missing.

The most localized rendering lead is the Android enemy/dead-cargo region:
separate `DrawImage2D` callsites at `draw+0x10e6` (`0x12f136`) and
`draw+0x1108` (`0x12f158`) feed different continuations. The local body
tail-merges one of these through a shared draw call. Compare the native
candidate/loop continuation around these callsites with the C++ branches
near `Radar.cpp:1034-1205` before changing source. The extra `FModSound`
call is near the ambient tail (`draw+0x264c`), where prior dispatch-label
experiments were rejected; do not repeat them without a control-flow or
behavioral discrepancy. The exact `Level::getPlayer` excess remains to be
localized among its 29 sites. iOS and PC sources checked so far do not
independently establish this Android physical-call scheduling.

Rejected as unjustified: adding redundant getter, draw, or sound calls solely
to equalize counts. The next accepted form needs native branch and ownership
evidence, a focused score increase without an exact-function regression, and
another full ARM/native gate.

## 2026-09-29 conditional-call correction and constructor trial

The initial constructor call-count probe was wrong because its quick
disassembly parser omitted conditionally executed ARM calls such as `blxne`.
After including them, constructor `C1` has **140 target / 140 local** direct
calls with identical call-target multisets; `drawCurrentLock` has **77 / 77**.
The three-call `draw` difference above remains **344 / 341** after the same
correction. In particular, the target's conditional `String::~String` near
the first label (`C1+0x254`) is present and must not be removed. This
correction rules out the proposed extra-destructor/lifetime explanation for
the constructor's 98.9% score.

Android constructor `0x12d484` computes the radar weights from the image
width/height squares. A trial reading already-stored `imageWidthSq` and
`imageHeightSq` fields in the equivalent C++ expression compiled all 204 ARM
units but left both constructors at **98.9%** and the 19-symbol mean at
**99.5684%** (`_work/radar_ctor_saved_squares_trial_20260929.json`). It was
rejected and the original expression restored. This is separate from the
earlier height-reciprocal staging trial, which was also neutral. No iOS/PC
body checked so far resolves the Android instruction scheduling here.

## 2026-09-29 enemy continuation trial

Android IDA `draw` at `0x12e050` keeps the dead-cargo image draw in its own
branch (`0x12f136`), then branches to the shared autopilot/docking candidate
checks. The C++ already has the same observable call and checks. Adding an
explicit `goto` from that branch to the existing candidate-check join was a
source-backed control-flow spelling trial, but Clang folded it back to the
same body: `draw` remained **96.8% / 47.9%** source/strict and the family
remained **99.5684%** (`_work/radar_dead_cargo_goto_trial_20260929.json`).
It was removed. The saved-squares constructor trial above was also removed;
`_work/radar_post_ctor_revert_20260929.json` reproduces the baseline after
the first revert. All 204 ARM translation units compiled after each trial,
and UCRT64 `gof2` linked after the restored constructor source. No new Radar
source form was accepted in this pass.

Neither `gof2_ios_ida_full_dump.c` nor
`gof2_gidra_ios_engine_full_source.c` exposes an attributed `Radar` body.
The installed PC `GOF2.exe` has no discoverable `Radar` symbol/string in its
binary; it has not been independently function-mapped for this branch.
Therefore Android IDA/ARM remains the direct evidence, and an iOS/PC
instruction-shape corroboration must not be claimed. The next draw experiment
needs a wider native candidate/overlay control-flow map, not an isolated
`goto` or a duplicate call to force the call count.

The final restored source was rebuilt as 204 ARM translation units and
rechecked in `_work/radar_post_trials_restored_20260929.json`: 19 comparisons,
**99.5684%** source-shape, 13 linked-exact and eight byte-exact. The UCRT64
`gof2` target linked `libgof2.a` again after this final restoration. The
most recent full-corpus report for the same source is
`_work/full_arm_radar_resume_20260929.json` (4,626 comparisons, zero skipped,
zero wrong-type misses); no accepted code change followed that report.

## 2026-09-29 draw call-order and instruction-inventory pass

The delinked Android `draw` call targets were rebased to `0x12e050` and
resolved through the IDA address table. Local `R_ARM_THM_CALL` relocations
were read from the ARM object. After stripping Android jump-thunk prefixes,
the physical-order call sequences align at about 87.3% (344 original calls,
341 local). This is a call-order diagnostic, not a CFG or behavioral score.

One contiguous target group containing `Level::getPlayer`,
`getAutoPilotTarget`, `getPlanetTargets`, `getStations` and
`getStationEnumIndex` occurs near `draw+0x6a0`; the corresponding local group
is near `draw+0x198`. IDA pseudocode keeps the approach-target condition
before the radar quadrants, but Clang places its compiled branch block in
different physical order. Large enemy/asteroid and ambient groups are also
reordered. These moved groups must not be mistaken for missing behavior.
The residual direct-call multiset difference remains one each of
`Level::getPlayer`, `PaintCanvas::DrawImage2D(Ejiihh)` and
`FModSound::play`; the separate Android enemy/dead-cargo draw sites at
`draw+0x10e6` and `draw+0x1108` remain the rendering lead.

The normalized instruction inventory independently shows 75 more local
frame-slot loads than the target in `draw`, while the target has 11 more
frame-slot stores. This is compatible with different vector/String local
lifetimes and block scheduling; it does not by itself identify a safe source
rewrite. The next experiment should map the entire station-to-enemy join
and its live return slots from ARM calls before editing the corresponding
`Radar::draw` source region.

Rejected trial: materializing the two integer differences in
`elipsoidIntersect` before float conversion matched Android's visible
expression order but compiled to identical ARM. Its 60/60-instruction body
stayed at 99.3% source-shape / 81.7% strict and the family remained
99.5684% (`_work/radar_ellipse_int_distance_trial_20260929.json`). The
trial was removed; `_work/radar_after_ellipse_revert_20260929.json` confirms
the restored 19-symbol baseline. No new Radar source form was accepted.

All 204 ARM translation units compiled after restoration and UCRT64 `gof2`
linked `libgof2.a`. The latest full `--fail-on-wrong-type` corpus for this
same restored Radar source is `_work/full_arm_abyssengine_20260929_final.json`:
4,630 comparisons and zero wrong-type misses. Its corpus count differs from
the earlier Radar gate because independent AbyssEngine template/body work
was accepted in the shared tree; the Radar family itself did not change.
iOS/PC dumps still do not provide an attributed Radar body for an independent
instruction-shape check.

An additional source-equivalent trial inverted the two mutually exclusive
`planetApproachTarget` branches while keeping every predicate and assignment.
Clang still placed `Radar::draw` at **96.8% source-shape / 47.9% strict** and
the family at **99.5684%** (`_work/radar_planet_branch_inversion_trial_20260929.json`).
That spelling was rejected and removed. The mismatch in the physical
station/planet block order cannot be repaired by merely inverting this `if`;
the next attempt needs call-anchored basic-block edges and return-slot
ownership, not another surface-level branch permutation.
`_work/radar_after_trials_restored_20260929.json` is the final focused
19-symbol report after both trial reversions; all 204 ARM units and the
UCRT64 `gof2` target rebuilt successfully.

## 2026-09-29 crosshair and asteroid event callsites

The fresh baseline after the independent Transform package was still
**99.5684%** for all 19 Radar symbols
(`_work/radar_resume_after_transform_20260929.json`). Android `draw`
at `0x12e050` reads `PlayerEgo::crosshairPos` directly in the offscreen enemy
branch. Replacing a 12-byte local `Vector` copy with a `const Vector &` made
the local ARM use that global directly: `draw` rose from **96.8% / 47.9%** to
**96.9% / 48.7%** source/strict
(`_work/radar_crosshair_ref_accepted_20260929.json`). The branch predicate
and coordinates are unchanged.

The Android ambient tail at about `draw+0x264c` has a distinct Deep Science
`FModSound::play(0x98)` branch, while Supernova and ordinary race tracks
converge on a shared `play(track)`. Expressing that shared track explicitly
raised `draw` to **97.0% source-shape**
(`_work/radar_ambient_shared_track_trial_20260929.json`). Clang still
tail-merges the Deep Science play call, leaving six local physical play sites
against seven original sites. This is accepted as a source-shape/CFG gain,
**not** evidence of sound byte-match.

The two asteroid lock event paths are distinct in Android IDA near
`0x130186`: event 9 and event 20 each call `Level::getPlayer` before joining
at one `Hud::hudEvent`. A shared `eventPlayer/eventId` spelling let Clang
merge the two getters. Calling `hudEvent(9, ...)` and `hudEvent(20, ...)`
directly within their existing conditions restored **29 local / 29 target**
`Level::getPlayer` callsites while retaining four local and four target
`hudEvent` callsites. The resulting `draw` is **97.0% source-shape / 48.7%
strict**, with 3,544 local versus 3,486 target normalized instructions;
focused report: `_work/radar_asteroid_direct_event_trial_20260929.json`.
It is not linked- or byte-exact.

Rejected source-equivalent trials: naming a separate offscreen delta left
the old ARM unchanged (`_work/radar_enemy_delta_lifetime_trial_20260929.json`);
hoisting both crosshair float-to-int conversions reduced `draw` source-shape
from 96.9% to 96.8% despite a strict increase
(`_work/radar_enemy_crosshair_preconvert_trial_20260929.json`). Hoisting
only X or Y gave no additional source-shape gain. An explicit shared
asteroid event `goto` compiled to the same body as the shared variables
(`_work/radar_asteroid_event_goto_trial_20260929.json`). These forms were
removed. No redundant getter or sound call was inserted to match a count.

The current callsite audit finds 12 target and 12 local six-argument
`DrawImage2D` calls; the older 12/11 count must not be carried forward as a
current missing-render-call claim. The target and local still differ in
physical block order, stack/String lifetimes, and the ambient play site.
The iOS dumps and PC binary still provide no independently attributed
matching Radar body; the Android IDA body and ARM instructions are the direct
evidence for this package.

All 204 ARM translation units compiled. The full
`--fail-on-wrong-type` report `_work/full_arm_radar_accepted_20260929.json`
completed **4,630 comparisons**, zero wrong-type misses, 2,082 linked-exact
and 946 byte-exact functions. UCRT64 `gof2` linked `libgof2.a`; all 18
other Radar symbols retained their prior source scores. The unweighted
Radar mean is now **99.578947%**, below the 99.9% gate by **0.321053 points**.
The remaining deficits are `draw` 3.0, both constructor aliases 1.1 each,
`drawCurrentLock` 1.9, `elipsoidIntersect` 0.7, and `calcDistance` 0.2
source-shape points. The next pass should map call-anchored block order and
temporary ownership in `draw` or constructor String cleanup; simple getter
and dispatch spelling changes are now exhausted.

## 2026-09-29 lock-label race-icon condition follow-up

Android IDA `drawCurrentLock` at `0x130998` only draws the final race icon
when the target is not the locked station and its `shipGroup` is not 10.
Expressing that predicate directly, without `suppressRaceIcon` and a staged
`raceFrame`, preserved the same behavior and improved the body from
**98.1% / 93.6%** to **98.2% / 93.7%** source/strict. The local body fell from
634 to 633 normalized instructions, against 622 target. Focused report:
`_work/radar_lock_race_direct_accepted_20260929.json`.

Rejected: replacing the staged gas/asteroid label flags with repeated direct
pointer comparisons reduced `drawCurrentLock` to **97.4% source-shape**
(`_work/radar_lock_direct_label_branch_trial_20260929.json`). Assigning the
chosen `String` separately inside each normal-label branch reduced it to
**97.6%** (`_work/radar_lock_direct_assignment_trial_20260929.json`). Both
forms were removed; the common source pointer and two label flags remain.
The experiments show that these source-level temporaries affect String
lifetime and branch layout, not just readability.

For the still-dominant `draw` deficit, the allocation-normalized inventory
has about 77 extra local frame-slot loads. The target instead has more direct
field loads and frame stores; its IDA stack view includes six 12-byte Vector
return slots and a 60-byte Matrix slot. This suggests a callsite-specific
return-slot/lifetime audit, **not** synthetic stack variables or redundant
calls. iOS/PC still do not provide an independently attributed matching
`Radar::draw` body, so this instruction-shape lead is Android-only.

After restoring the two rejected trials, all 204 ARM units compiled. The
full `--fail-on-wrong-type` report
`_work/full_arm_radar_lock_direct_20260929.json` completed 4,630 comparisons
with zero wrong-type misses; 2,082 linked-exact and 946 byte-exact functions
were unchanged. UCRT64 `gof2` linked `libgof2.a`. Only `drawCurrentLock`
changed score across the full corpus. The 19-symbol mean is **99.584211%**,
still **0.315789 points** below the requested 99.9% gate.
