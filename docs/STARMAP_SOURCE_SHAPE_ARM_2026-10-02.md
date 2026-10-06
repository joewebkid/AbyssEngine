# StarMap Android ARM recovery - 2026-10-02

## Status and scope

**PARTIAL: the requested 99.0% target is not reached.** The fixed 20 distinct
implementation bodies measure **99.380% unweighted / 98.88257% original-instruction
weighted source-shape**. The clean baseline measures **83.81% / 73.61763%**.
Strict ordered fuzzy matching improves from **48.29% / 28.05016%** to
**88.040% / 83.12933%**, respectively. Source-shape is an inventory/control-flow
metric, not executable equivalence or a percentage of proven behavior.

- Clean baseline: `f60694dd0ca578831cec832e7026b32644642d97`.
- Isolated branch: `codex/starmap-arm-99-20261002`.
- Isolated checkout: `A:/gof2-starmap-99-20261002`.
- Changed production files: `src/game/world/StarMap.cpp`, `StarMap.h` only.
- Added validation: `tests/check_starmap_source_shape.py`, this report and its
  compact evidence snapshots. No verifier, shared configuration, Level, Gun,
  PlayerTurret, TargetFollowCamera or Godot files were changed.
- Reviewed the 2026-09-24 report and rejected forms before changing code.
  Main checkout and other agents' worktrees were left untouched.

The denominator remains **7101 original instructions** in the same 20 bodies.
Constructor C2 and destructor D2 aliases are checked against C1/D1 but excluded
from both means. No bodies, weights or verifier settings were changed to raise
the result. No volatile/register barriers, dead-code padding or synthetic
explicit template instantiations were added.

## Follow-up after checkpoint 7fc7701c

The first checkpoint (95.65% / 91.29538%) remains in commit 7fc7701c.
This continuation changes StarMap.cpp only:

- OnTouchEnd: 86.3% -> 97.2%, strict 33.0% -> 67.6%. Android station
  warning branches continue through inertia and Help rather than returning
  early. Restored automatic-map suppression, repeated mission/ship access,
  the direct Status::playerStation load, max-before-min easing calls and
  the temporary concatenation expression. Help is constructed only when used.
- init: 90.0% -> 97.3%, strict 57.9% -> 67.0%. Restored separate route/no-route
  station scans and repeated getters. On entering planet mode, copies the
  current camera and changes only Z, rather than replacing its translation.
  Uses Globals::galaxy and the original borrowed playerStation comparison.
- draw: 89.9% -> 97.2%, strict 39.6% -> 81.7%. Restored per-iteration
  visibility reads, original geometry-count bound, value snapshots before
  Vector assignment, the current-route negative-Z test, temporary name/header
  lifetime, shared text X and the explicit two-argument cost String copy.
- Focused packages: _work/touch-reloads-trial, touch-order-trial2,
  init-reloads-trial, draw-reloads-trial JSON/logs. touch-order-trial had one
  local namespace compile error, fixed before accepting any measurement.

The remaining gaps are measured, not a proven compiler ceiling. Direct call
inspection also identified by-value Header/Help temporary copies for further
work. The 99% objective remains unmet.

## Follow-up after checkpoint dca65361

Checkpoint dca65361 measured 96.925% / 94.59986%. This continuation edits
StarMap.cpp only and keeps the original corpus and verifier unchanged.

- drawOnScreenInfo: 90.9% -> 96.6%, strict 22.9% -> 48.9%. Replaced duplicated
  station/system campaign/freelance/pending branches with Android's common
  ordering, repeated Station indices and Status getters. Mission 59 suppresses
  the common marker at station 101/system 23; it does not add a target there.
  Restored station name alpha clamp 64, unconditional alpha resets after the
  current-system pulse, select icon before race/security labels and the single
  hasNoOwner query that determines the security-label row.
- draw: 97.2% -> 98.0%, strict 81.7% -> 83.2%. Direct temporary Header String
  removes a copy that direct ARM call inspection showed was absent.
- OnTouchEnd: 97.2% -> 96.8%, strict 67.6% -> 68.4%. Direct temporary Help
  String matches the Android by-value call. This semantic/ABI-shape correction
  was retained despite the small source-shape decrease.
- initStarSystem: 91.2% -> 95.9%, strict 48.6% -> 69.2%. Canonical Globals::rnd
  and Globals::appManager, repeated texture reads/member loads, unsigned angle
  division, translated Vector assignment, fresh planet position and campaign
  reads. Restored index-zero child check; ArraySetLength explicitly zeroes its
  pointer storage, so this does not read uninitialized memory. Restored ring
  distance float/int conversion and yaw/pitch write ordering.
- ELF bytes prove systemNameImages at 0x1fdde8 and planetTextures at 0x1fdd14
  have 32-bit slots; uint32_t tables preserve that stride while retaining the
  same low-16-bit IDs accepted by the rendering API.
- update: 93.9% -> 94.7%, strict 54.1% -> 57.7%. Removed a gratuitous third
  CameraGetCurrent call, kept fresh position values for projection, scoped
  camera matrix copies at their uses, and restored two GetMaxValue reads per
  checked transition axis, short-circuiting Y/Z as Android does.

Packages: info-common-trial, info-colors-trial, station-reloads-trial2,
update-reloads-trial, station-layout-trial, third-focused. The initial station
reload trial failed on a leftover local system argument; corrected before any
measurement was accepted. Native log is _work/native-third.log.

Original campaign-116/125 and type-14 freelance branches assume valid mission/agent
objects; those Android contracts are retained and need native runtime coverage.
ArraySetLength pointer-zeroing was checked in src/engine/core/Array.h before
restoring the index-zero child check.

The target remains unmet. Further direct evidence identifies another extra
CameraGetCurrent call in OnTouchMove and residual instruction/control inventories
in the large bodies. This is incomplete source recovery, not a claimed compiler
ceiling or runtime validation.

## Follow-up after checkpoint 50e10b79

Checkpoint 50e10b79 measured 97.505% / 96.19283%. This continuation modifies
StarMap.cpp only and the report/snapshots.

- OnTouchBegin: 94.8% -> **99.3%**, strict 63.5% -> **97.4%**. Direct Android
  body 0xca7e0 confirms positive-test absolute-value selection, refreshed
  Layout/Sound services and a second Status::getSystem/getIndex pair when
  jump distance is zero. Restore those reads and station-position reloads.
- OnTouchMove: 90.7% -> 93.8%, strict 60.4% -> 75.6%. Removed the third
  CameraGetCurrent call and unused matrix construction, restored the nested
  dialog/transition/control/drag flow and nonzero-mode branch before galaxy
  mode. Pitch clamps preserve Android's negative-bound/NaN path; speed uses
  the original less-than-ten choice. No volatile/register barriers or branch
  profile hints were used to force instruction selection.
- Constructor C1: 93.4% -> **98.0%**, strict 60.3% -> **84.2%**. Initializer
  list zeroes both scratch vectors before automatic inline easing construction.
  Previous scratchVector2.x, lastTouchX/Y and dragging initialization gaps are
  corrected. Uses canonical Galaxy/Random services, repeated system/member
  accesses, the three-float setPosition overload and direct marker position
  temporary. C2 remains a checked alias, excluded from aggregates.
- update: source remains 94.7%, strict 57.7% -> 57.9%. Direct ARM call at
  0xc9e0c proves scalar-left Vector multiplication. Correct overload restored.
- The expanded pitch form suggested by IDA was trialled but rejected because
  it retained redundant checks. Simple equivalent bounds were kept rather
  than retaining redundant source for metric shaping.

Packages: move-abi-trial, begin-reloads-trial, ctor-abi-trial,
touch-shape-trial (rejected expanded pitch), touch-simple-trial,
move-flow-trial (accepted final focused input). Fresh original extracts:
_work/begin-android.c and ctor-android.c. Inventory differences were inspected
through the unchanged asmdiff reader and recorded in _work/move-inventory.txt.
The 99% goal is still unmet; OnTouchMove predication and the large-body
instruction/control inventories remain work, not a claimed compiler ceiling.

## Follow-up after checkpoint b5010fdb

Checkpoint b5010fdb measured 98.115% / 96.69851%. This continuation edits
StarMap.cpp and its evidence only. update improves **94.7% -> 97.7%**, strict
**57.9% -> 82.3%**; original/current instructions: 971/953, call count 100%.

- Android update at 0xc9b90 projects station positions without an added null
  guard; uses fresh member accesses and fresh Globals::Canvas for marker
  Transform lookup and CameraSetLocal calls. Direct Vector temporaries remove
  extra assignment calls in marker interpolation, direction and normalization.
- Direct ARM instructions at 0xc9d6e-0xc9d82 and 0xca0f4-0xca0fc prove
  double intermediates for marker scale and automatic-selection scale. ELF
  PT_LOAD mapping decodes doubles -0.014, 0.02 and 0.002 at 0xc9d40/48/50.
  Restore double literals and final float conversion; IDA float notation alone
  would have lost this precision. Source-shape grows 97.2% -> 97.7%.
- Positive-test absolute-value selection preserves Android's signed-zero and
  NaN paths. Station speed keeps the original less-than-ten selection and pitch
  keeps the negative-bound NaN path. Galaxy centering explicitly emits +0 at
  equal screen coordinates; Y equality retains both ordered comparisons so
  unordered values still follow the original division path.
- Restored galaxy-mode-first flow, station-center-before-inertia flow, integer
  yaw distance <=10, signed selected-index check and per-iteration station
  geometry loads. Transition completion uses nested X/Y/Z checks and the
  transition-in branch first; no synthetic finished variable remains.
- Original transition-out calls release helpers before optional destruction.
  Restore that order, retaining the initialized-array contract verified in
  initStarSystem (stationGeoms, ringGeoms and usedFlags are allocated before
  transition-in, then released on transition-out). Station centering assumes
  a valid selectedStation as Android does; no native runtime proof is claimed.

Packages: update-fifth-trial, update-fifth-flow, update-fifth-release and
update-fifth-nested and update-fifth-double (accepted fifth-focused). Original evidence is
_work/update-android.c and update-original-direct.asm; Array helper contracts were checked in Array.h.
Direct real external-call inventories now agree for update except the documented
Array<bool>/Array<uint8_t> release/destructor spelling retained to preserve the
baseline exact pair. _work/update-fifth-real-call-differences.txt excludes
literal-pool pseudo-calls and records those two name substitutions.
Removing the synthetic completion variable produced the same ARM measurement,
so the simpler faithful control flow was retained without claiming a gain.
The first fifth-corpus run was deliberately stopped after new double-precision
evidence required a source revision. Final full run: fifth-double-corpus.
The 99% target remains unmet; residual inventories require further evidence.

## Follow-up after checkpoint d4e3330a

Checkpoint d4e3330a measured 98.265% / 97.10873%. This continuation changes
StarMap.cpp only, with report/snapshots. drawOnScreenInfo improves **96.6% ->
97.9%**, strict **48.9% -> 64.9%**, original/current instructions: 1207/1221.

- The decisive remaining call discrepancy was a real signedness error:
  SolarSystem.h declares getStationEnumIndex as uint32_t and its method returns
  0xffffffff when absent. All seven >=0 comparisons in this StarMap body were
  therefore constant true, eliminating the galaxy fallback call and incorrectly
  marking absent station targets. Android's signed comparisons at 0xc849e,
  0xc8544, 0xc8660, 0xc876c, 0xc87c6, 0xc884e and 0xc88e4 prove the caller must
  interpret that sentinel as -1. Explicit int32_t casts restore those checks
  without changing SolarSystem.h/cpp or other production files.
- Direct vcvt.u32.f32 at 0xc89d4 and 0xc8ab4 proves pulse alpha's unsigned
  conversion. Alpha and Canvas are captured before getPulseValue, as Android
  does; marker image coordinates reload scratchVector after calls.
- The station technology label is now a direct DrawString concatenation
  expression, preserving every temporary until the draw call ends. Name
  getters already return temporary Strings and DrawString/GetTextWidth accept
  const references, so no extra name-copy workaround was added.
- Station text X remains float until each draw, text Y is computed before
  GetTextWidth and station lookup. Icon row X/Y stay int, iconBuffer and
  missionIconWidth reload in the loop. This removes unnecessary int/float
  round trips and restores the original integer row accumulation.
- Mission 116/120/125 success goes to the shared special-marker write and skips
  the remaining special checks. A structured nested trial and the explicit
  shared-exit form produced identical ARM measurements before signed casts;
  the latter exposes Android's common exit without fabricated helper calls.
- Mission-163 targets keep the local array pointer, refreshed after an unmatched
  iteration. Race image address is selected after getRace then loaded for draw;
  neither image nor target array is held across a call beyond the original scope.

Packages: info-sixth-trial, info-sixth-flow, info-sixth-shared,
info-sixth-signed and info-sixth-loads (accepted sixth-focused). Native static
library build: _work/native-sixth.log. Direct instruction evidence:
_work/info-original-direct.asm; original body: _work/info-android.c.
The original String integer constructor uses C2, current C1 is its ABI alias;
that spelling is not another implementation body or a metric denominator.

A further IDA-only OnTouchEnd candidate was rejected: its X comparison renders
!(less || equal), suggesting different NaN handling, but direct ARM uses gt
for both axes (0xcb0ec/0xcb0fa and 0xcb8d8/0xcb8e2). No speculative source
change was made; _work/end-original-direct.asm and touch-next-evidence.txt
record the counter-evidence.

These are source/ABI-shape and branch-contract corrections. No game executable
or renderer runtime was exercised. The requested 99% target remains unmet.

## Follow-up after checkpoint bc98481c

Checkpoint bc98481c measured 98.33% / 97.32970%. This continuation edits
StarMap.cpp, report and snapshots only. OnTouchEnd improves **96.8% -> 97.5%**,
strict **68.4% -> 76.4%**, original/current instructions: 1037/1003.
initStarSystem improves **95.9% -> 96.1%**, strict **69.2% -> 74.6%**,
original/current instructions: 710/673.

- Direct ARM at 0xcb71e/0xcb720 loads StarMap fields +104/+96, then gets
  systemGeoms[selectedSystem]'s position. The previous planetGeom position
  was an incorrect receiver inferred from IDA's incomplete sret call. The
  transition now starts at the selected system marker as Android does.
- Direct scratchVector/scratchVector2 assignments retain Android's two Vector
  assignments at 0xcb734/0xcb76e; remove the extra two assignments through a
  local p. Original transition setup returns after clearing momentum/velocity,
  so the corrected path returns before Help rather than falling through.
- Normal input precedes the dialog-confirmation tail; station mode precedes
  galaxy mode. Explicit branch-local ChoiceWindow allocation and jump-mode
  snapshot match the original lifetime/read order. Reordering produced the
  same ARM code, so no metric gain is claimed for those source-only changes.
- A separate warning-3217 shared-exit trial was rejected: it increased generated
  calls and lowered source-shape to 97.4%. The existing warning selection gives
  the same native warning and proceeds through choice visibility/inertia/Help.
  No duplicate allocations or artificial calls were added to inflate metrics.
- initStarSystem seed: 0xc7638 is 32-bit mul and 0xc763e sign-extends that result
  for setSeed. Replace premature 64-bit multiplication with unsigned 32-bit
  multiplication followed by int32_t conversion/sign extension; this preserves
  the ARM wrap without C++ signed multiplication overflow.
- Used-flag zeroing snapshots pointer/size and uses != as Android's loop does;
  ring and station-position bounds reload member arrays. Position allocation
  captures the destination array before operator new and stores into that
  captured array, matching original call ordering.

Packages: end-seventh-trial, end-seventh-flow, end-seventh-warning (rejected),
end-seventh-final, station-seventh-trial (accepted seventh-focused).
Direct evidence: _work/end-original-direct.asm, station-original-direct.asm;
original bodies: touch-android.c, station-android.c. Native log:
_work/native-seventh.log. Raw named-call inventories based only on relocation
lines omit indirect GOT/register calls: before this repair Vector assignment
was loaded once and called four times through r5. Those inventories alone must
not be described as complete named-call equality; unchanged verifier call
counts include those actual indirect call instructions.

No native executable/runtime test is claimed. Remaining instruction/control
inventories are incomplete recovery, not a proved compiler ceiling. The 99%
objective remains active and unmet.

## Follow-up after checkpoint 42331c92

Checkpoint 42331c92 measured 98.375% / 97.45192%. This continuation edits
StarMap.cpp, report and snapshots only. draw improves **98.0% -> 98.3%**,
strict **83.2% -> 87.6%**, original/current instructions: 916/909.
init improves **97.3% -> 97.7%**, strict **67.0% -> 72.6%**, instructions:
750/735. depart improves **95.1% -> 100.0% source-shape**, strict
**60.1% -> 88.3%**, instructions: 205/205. depart remains non-exact.

- draw uses the three-argument DrawImage2D overload for the system header
  image, as direct ARM at 0xc95d2 calls Ejii rather than Ejiih. Restores mode
  reload after Ease getters and clamps the locally computed alpha. Security
  label Y uses fresh Layout fields after intervening calls. Footer font,
  Canvas, X and width are captured before GetTextWidth; Y reloads Layout
  afterward, matching original native read order.
- init removes the extra Vector assignment through local pos: Android
  0xc6eaa/0xc6eb4 and 0xc6eda/0xc6ee4 use getPosition followed by one
  assignment into scratchVector. Canvas is captured across CameraGetCurrent;
  key font/Canvas are captured before GameText::getText. Station array deletion
  rereads the member after ArrayReleaseClasses, as 0xc7076 does.
- init's planet-mode camera position had another incorrect getPosition
  receiver: direct ARM 0xc7000/0xc7006 loads +0x68/+0x60 and calls getPosition
  at 0xc7012. It now uses systemGeoms[selectedSystem] rather than planetGeom.
  Makes the camera copy explicit as the 60-byte memcpy at 0xc7044/0xc7048
  before changing word 11 and reuses the existing local matrix. Matrix is
  already 60 bytes in the shared header, so this does not repair a copy-size
  mismatch. No fabricated padding or forced extra instructions were added.
- depart removes getJumpgateUsed, absent from the Android body. The original
  0xc7e54 jumpgateUsed call is followed by a jump/jumpMapModeB test. Reloads
  stations/selectedStation after callbacks and during cleanup, restores
  programmedStation's post-equals member read, and uses Globals::achievements
  and Globals::appManager as original. Production Level files are untouched.

Packages: draw-eighth-trial, init-eighth-trial, depart-eighth-trial (accepted
as eighth-focused). Direct evidence: _work/draw-original-direct.asm,
init-original-direct.asm, depart-original-direct.asm; original IDA extracts:
draw-android.c, init-android.c, depart-android.c. The earlier full-corpus run
was cancelled before completion when these additional source repairs were
identified; no results from that cancelled run are accepted.

No native executable/runtime test is claimed. The fixed 20-body, 7101-original-
instruction denominator and alias exclusions are unchanged. The 99% objective
remains active and unmet; source-shape 100% for depart does not mean exact code
or validated gameplay.

## Follow-up after checkpoint 52079d64

Checkpoint 52079d64 measured 98.655% / 97.67433%. This continuation edits
StarMap.cpp/h, report and snapshots only. OnTouchMove improves
**93.8% -> 98.4%**, strict **75.6% -> 79.2%**, instructions: 265/255.
initStarSystem improves **96.1% -> 96.3%**, strict **74.6% -> 81.7%**,
instructions: 710/680. init improves **97.7% -> 98.0%**, strict
**72.6% -> 76.1%**, instructions: 750/737.

- Correct OnTouchMove result from void to int and return zero on every path.
  Original common exit 0xcab52 explicitly moves zero into r0. Original
  ChoiceWindow::OnTouchMove and AEGeometry::setRotation are ordinary calls
  followed by the common exit. The incorrect void prototype allowed tail
  calls with duplicated guard checks and a different return register value.
  This restores the observed native result without barriers or flag changes.
- initStarSystem normalization writes a local position, not scratchVector:
  getPosition at 0xc785c writes sp44, VectorNormalize at 0xc7866 writes sp32,
  and Vector assignment at 0xc786e writes sp44. Remove the unintended member
  mutation. Ring texture bound is unsigned, as cmp/bhi at 0xc7886/0xc788a
  proves; negative signed indices no longer enter the shift expression.
- Local Matrix initialization: initStarSystem's matrix base is sp128
  (0xc76b2); init's matrix base is sp160 (0xc6dba). Each loads four float
  words at matrix offset40 from Android ELF literals 0xc7d48/0xc7468:
  bytes 0000803f000000000000803f0000803f, values [1,0,1,1]. Combined with
  explicit word0/5/14 stores this proves identity with all three scale fields
  equal1. The shared constructor currently sets m10 and m13 to zero. Repair
  only those two differing fields in StarMap's local matrices. No shared
  Matrix implementation or MatrixIdentity call is changed; no redundant
  copies of already-correct identity words are added as instruction padding.
  init constructs its matrix after CameraSetPerspective, matching Android.
- init allocates TouchButton before querying its text through the direct new
  expression, and constructs ChoiceWindow before resetting systemPath and
  storing choiceWindow. Both restore original native callback/read order.
- drawOnScreenInfo captures shared font/Canvas before branching for text width,
  as original 0xc8920..0xc8958 does. The source form changed but generated
  focused metrics are identical; no numerical gain is claimed for that edit.

Packages: move-ninth-trial, station-ninth-trial, info-ninth-trial,
matrix-ninth-trial, init-order-ninth-trial (accepted as ninth-focused).
Evidence: _work/move-original-direct.asm, station-original-direct.asm,
init-original-direct.asm, info-original-direct.asm, matrix-ninth-evidence.txt.
Matrix.h/cpp and AEMath.cpp were inspected read-only to compare fields and
setter contracts. A proposed Matrix-copy-width discrepancy was rejected:
Matrix is already60 bytes, confirmed by its sizeof assertion. No size repair
or source change was made for that rejected candidate.

No native executable/runtime test is claimed. Source-shape and ABI-source
repairs require runtime validation; remaining emitted inventories are not a
proved compiler ceiling. Both requested 99% means remain below threshold.

## Evidence and version boundaries

Primary reference is **Android ARMv7 GOF2 HD 2.0.16**,
`_work/bins/android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Method bodies were checked in the local IDA dump
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`;
direct ARM disassembly and ELF data were used where the decompiler was ambiguous.

Android anchors: constructor `0xc67a4`, init `0xc6bf0`, destructor `0xc749c`,
initStarSystem `0xc7580`, initLights `0xc7d9c`, setStart `0xc7dd8`,
depart `0xc7e0c`, isInPlanetMode `0xc8040`, setJumpMapMode `0xc804c`,
renderBG `0xc8056`, render `0xc8058`, drawKey `0xc80e8`,
drawOnScreenInfo `0xc8354`, draw `0xc90f4`, missionChanged `0xc9b48`,
askForJumpIntoAlienWorld `0xc9b50`, update `0xc9b90`,
OnTouchBegin `0xca7e0`, OnTouchMove `0xcab0c`, OnTouchEnd `0xcae90`.

**iOS HD 1.1.4 cross-check only:** version verified from the local app's
Info.plist (`CFBundleVersion` and `CFBundleShortVersionString` both `1.1.4`).
IDA functions `sub_1C3E5C`, `sub_1C4398`, `sub_1C8418` confirm the inline
EaseInOut objects, geometry position access, constructor scales, perspective,
marker interpolation and station world-matrix composition. iOS Status offsets
differ (target-system access +120 versus Android +124); they were not substituted
into the Android layout.

**PC release data cross-check only:** the local July-22 comparison does not
identify an executable/version number. It is therefore labelled an unidentified
PC release dataset, not an Android implementation reference. PC stations.bin
SHA-256 `65cebd57aa5cb8229ff35dfa91a15b70c55218a98697ce7617c329bb8e157572`;
the comparison report is
`analysis/gof2_pc_android_bin_comparison_20260722/pc_android_bin_diff.json`.
All 109 common station records agree; PC has 27 systems versus Android 34,
and 23 of the 27 common systems agree, with four Android route extensions.
PC cardinalities and routes were not used to overwrite the Android map.

## Accepted source changes

- Restored interior Android object offsets rather than retaining a misleading
  correct total size with shifted members. Three 16-byte inline EaseInOut objects
  replace incorrectly declared 12-byte vectors; missing slots are represented
  explicitly, the fabricated trailing padding is removed, and 32-bit compile
  assertions check the allocation size `0x1e8` and important offsets.
- Replaced local pseudo-sret casts and unresolved wrappers with typed geometry,
  matrix/vector, Station, SolarSystem, FileRead and path-finder calls. Android
  return-by-value ABI now comes from their declarations. Canonical Globals
  accesses restore service loads and call ordering.
- Constructor restores Android map scales `.012/.018/.025`, extents,
  selected-system marker position and rotation. init restores the typed station
  loader, mission/station lookup, fallback positions, camera transform and
  route/no-route branches.
- initStarSystem restores typed scale/texture overloads, initial/root rotation,
  station selection, lighting service calls and the planet-ring branch. The
  ring mask is the ARM literal `0x00210200`; IDA's apparent `Item::makeItem`
  pointer was rejected after checking literal-pool bytes.
- draw restores route visibility/pulse, route animation, race/security labels,
  header and cargo/jump-cost footer. update restores inline and heap easing,
  station world-matrix composition using station geometry index +1, marker
  position/scale interpolation, inertial motion, signed yaw wrapping, pitch/pan
  limits and path overshoot halving.
- Touch handlers restore drag signs/threshold, reverse easing on leaving planet
  mode, transition position, accepted jump outputs, equipment/passenger/cargo
  restrictions and the localized warning branches visible in Android.
- drawOnScreenInfo restores clipping, name lifetimes, repeated service getters,
  station inverse alpha, galaxy alpha, security colors and Android icon slots.
  drawKey uses the directly confirmed six-argument drawBox overload with a
  temporary String; the extra flags argument was rejected by direct disassembly.
- initLights now calls FogEnable(0, 1); setStart clears a deleted route array;
  missionChanged reads the corrected offset. These and askForJumpIntoAlienWorld
  gain exact comparisons.

## Rejected/intermediate forms and remaining uncertainty

- Initial layout assertions exposed omitted interior gaps; that form was rejected
  before accepting the ARM build. Compile errors from legacy getStation/GetEngine
  return declarations and one remaining Station_getSystem shim were corrected
  locally, without changing their shared declarations.
- A temporary Array<bool> spelling removed the existing Array<uint8_t> constructor
  exact pair from this translation unit. It was rejected. Byte-valued 0/1 storage
  is retained and documented; original Android template spelling is not claimed
  to be reproduced. No dummy instantiation was used to conceal the loss.
- An intermediate icon layout scored slightly higher but contradicted the
  Android mission/warp-gate slots; the semantic correction was retained.
- Focused packages are preserved locally as layout-trial2, draw-update-trial,
  touch-key-trial, init-typed-trial2, station-move-trial2, info-trial,
  info-reloads-trial, legend-icons-trial and globals-trial JSON/logs under _work.
- The large bodies remain below 99%; cached restrictions, temporary String
  construction, branch ordering and emitted instruction/control inventories
  require further work, particularly OnTouchEnd, init and draw. No source-shape
  score is presented as exact recovered gameplay.
- Some Layout words are accessed by confirmed raw offsets because typed shared
  fields remain unnamed. Unknown StarMap slots keep field-offset names.
- Existing ownership assumptions around station loading/deletion and repeated
  geometry attachment are retained from the Android body and need a native
  executable test. No renderer, camera, touch or mission runtime was exercised.
  A successful static-library build does not prove native ABI/runtime behavior.

## Follow-up after checkpoint 7b76861c

This continuation changes StarMap.cpp only. Android ARM instructions at
0xca1d8..0xca224 and the station-centering block at 0xca50c onward preserve
the selected station and old pitch/yaw, then choose the quarter-step direction
by comparing current and target angles. The implementation now follows those
reads, direct comparisons and +/-0.25f branches. Inertial additions also follow
the native operand order. These branches are evidenced by Android 2.0.16;
no iOS/PC formula is substituted.

Update source-shape: 97.7% -> 98.3%; strict ordered match: 82.3% -> 82.0%;
original/current instructions: 971/953 -> 971/961. Fixed-body means change
from 98.910% / 97.89768% to 98.940% / 97.97972%. Strict matching declines
slightly to 85.610% / 77.88157%; this is explicitly not an exact-match claim.

Two OnTouchEnd experiments were rejected and restored before the accepted
build: a sequential warning chain measured 97.4%, and an explicit common
confirmation exit measured 97.3%, against 97.5% in the retained implementation.
The compiler already merges the confirmation calls. Neither experiment showed
a separate behavior correction, so neither remains in production code.

Evidence extracts and trial reports are retained under _work. The existing
fixed 20-body gate, original weights and alias checks remain unchanged.

## Follow-up after checkpoint 72045054

Two source-order corrections are retained in StarMap.cpp:

- initStarSystem: Android 0xc7ad2..0xc7af0 releases stationPositions, reloads
  the pointer before deletion, and unconditionally clears the member before
  allocating its replacement. The zero assignment now sits after the release
  branch. Source-shape remains 96.3%; strict match changes 81.7% -> 82.0%.
- drawOnScreenInfo: Android 0xc8e60 loads selectedSystem before SetColor at
  0xc8e68 and compares that saved value at 0xc8e70. The selected marker branch
  now preserves that read across the canvas call. Source-shape remains 97.9%;
  strict match changes 64.9% -> 65.1%.

The fixed source-shape means remain 98.940% / 97.97972%. No source-shape gain
is claimed for this package. An alternate clamp-first alpha expression was
rejected and restored: it reduced drawOnScreenInfo to 97.8% without improving
strict match beyond 65.1%.

Call-site inspection identifies the initStarSystem count difference: Android
has inline matrix initialization, while this TU calls the shared Matrix C1
constructor and then fixes its two local native identity fields. Current-only
constructor call and the already documented Array<uint8_t>/Array<bool>
symbol differences are distinct. Shared Matrix code and verifier settings
remain outside scope; no raw-storage alias, ODR override or fabricated call
is used to conceal those differences.

Evidence: _work/station-original-direct.asm, info-original-direct.asm,
eleventh-current.asm; station-eleventh.json, info-eleventh.json and the rejected
info-alpha-eleventh.json. Final accepted inputs use eleventh-focused.json.
A first post-restore focused report was discarded: restoring a backup retained
an older source timestamp, so the incremental ARM build reused the rejected
alpha object. The associated full comparison was explicitly stopped before
acceptance. The source timestamp was refreshed and the focused build rerun
before starting the accepted full comparison; no stale-object result is used
as final evidence.

## Investigation after checkpoint eeebc3e6

No production source change is retained from this investigation. Four focused
variants were tested, then restored with a fresh source timestamp:

| Variant | Method source-shape | Strict | Decision |
|---|---:|---:|---|
| Capture ChoiceWindow in each warning branch | OnTouchEnd 97.5% | 76.4% | Identical metrics; restore simpler retained source |
| Integer 0/1 campaign-target flag | drawOnScreenInfo 97.9% | 65.0% | Worse strict/control agreement; reject heuristic type |
| Read X/Y/Z before coordinate scaling | constructor C1 98.0% | 84.2% | Compiler already schedules this form; restore |
| Select final position pointer in separate mode branches | drawOnScreenInfo 97.9% | 65.1% | Identical metrics; restore |

The integer flag type is a heuristic; it is not established from the native
register value. The retained bool expression remains valid. No barriers,
profile changes or duplicate calls were introduced.

Instruction-inventory inspection identifies 25 original-only loads of the
shape ldr g0,[g1,#0] versus 24 current-only ldr g0,[g0,#0] in drawOnScreenInfo.
These include a destination/base register alias difference that the existing
metric deliberately preserves. This observation does not prove every such
load is semantically equivalent; it is a lead for further disassembly mapping,
not a justification to change the gate or add register-forcing source code.
OnTouchEnd raw call-site counts also differ from normalized call tokens; direct
relocation counts alone must not be used to infer an extra gameplay call.

Original ELF .comment and the installed NDK r18b compiler both identify
Android build 4751641, Clang 7.0.2 based on r328903, Clang revision
003100370607242ddd5815e4a043907ea9004281 and LLVM revision
1d739ffb0366421d383e04ff80ec2ee591315116. The ELF also lists GCC 4.9.x for
linked objects; .comment alone does not assign a producer to StarMap. No
compiler/profile switch follows from this evidence.

Evidence: _work/twelfth-inventory.txt, warning-window-twelfth.json,
info-flag-twelfth.json, ctor-coordinates-twelfth.json, info-position-twelfth.json,
ctor-original-direct.asm and twelfth-restored-focused.json/log.
The final restored ARM StarMap.o SHA-256 is
c46cec559bddc6dd914101672568036aaa423954c321917c9b30ed8549c9de57,
identical to the accepted checkpoint eeebc3e6 object. Production source is
unchanged relative to that commit. Its native build and full-corpus evidence
remain the current final validation; no trial report replaces those snapshots.

## Follow-up after checkpoint 644c59e5

Only two comparison expressions in update are changed. Android path-centering
compares X at 0xca47c, branches with bpl at 0xca488, and conditionally moves
the calculated step under gt at 0xca6d4. The bpl outer condition includes an
unordered comparison: it is the complement of less-than, not ordered >=.
The retained source now uses !(currentX < targetX).

For Y, 0xca6de compares the coordinates, 0xca6e6 skips the zero override under
gt, and 0xca6e8/0xca6ea zero the step under pl. Thus neither greater-than nor
less-than is the zero-override condition, including the unordered case. The
source uses !(currentY > targetY) && !(currentY < targetY).

This is recovered machine-branch behavior, not a claim to recover the original
C++ expression or its floating-point compile flags. The surrounding float-to-int
conversions still require their existing valid-input assumptions; no end-to-end
C++ NaN safety or native runtime equivalence is claimed. Finite less/equal/greater
cases retain the same arithmetic, thresholds and calls.

Update source-shape: 98.3% -> 98.5%; strict: 82.0% -> 82.1%; original/current
instructions: 971/961 -> 971/966. Fixed means are 98.950% / 98.00707%; strict
85.640% / 77.95923%. No body, weight, alias or profile is changed.

Rejected larger variants: merged step-selection (98.2%, strict 79.3%), nested
shared-step selection (98.3%, strict 82.0%) and shared camera-boundary impulses
(98.2%, strict 80.3%). None of their extra locals or goto flow remains. The
accepted two-line comparison change is separately rebuilt and measured.

Evidence: _work/update-original-direct.asm; update-path-thirteenth.json,
update-path-thirteenth-nested.json, update-bounds-thirteenth.json, and the
accepted update-compare-thirteenth.json/log (copied to thirteenth-focused.json).
Current accepted disassembly: _work/thirteenth-current.asm; it emits bpl for
X's outer branch. Predicate agreement does not establish floating-point
exception-flag equivalence or the original compiler's FP assumptions.

The first full comparison reported 4601 entries and three skipped shader
units (GenericShader, GlowShader, GreenShader). Both preservation gates
rejected this incomplete report before any acceptance. All three units
subsequently compared successfully without source/object changes, six entries
each. The complete comparison was rerun with GOF2_VERIFY_JOBS=2 instead of
its default eight verification threads. This only limits verifier execution
concurrency, not compilation flags or metrics. The rejected report/log are
preserved as _work/thirteenth-corpus-skipped.json/log; unit retry inputs are
thirteenth-generic-retry.json, thirteenth-GlowShader-retry.json and
thirteenth-GreenShader-retry.json. Final evidence must come from the complete
rerun, never a synthetic merger of the incomplete and unit reports.

## Follow-up after checkpoint e07a1f16

Only the first route-line depth predicate in draw changes. Direct Android ARM
at c92a6..c92c0 compares the first Z to zero and branches with BPL to DrawLine;
the second comparison skips DrawLine with BMI. This draws unless both depths
are ordered negative. The retained source used z1 >= 0 || z2 >= 0, which differs
on unordered comparisons. It now uses !(z1 < 0 && z2 < 0), the form already used
for the current-route line immediately below. This is machine-predicate evidence,
not recovery of the original C++ spelling or floating-point exception flags.
Normal projected depths are +1/-1; no broader NaN-safe rendering claim is made.

Focused draw source-shape: 98.3% -> 98.5%; strict: 87.6% -> 87.8%; instruction
counts remain 916/909. The unchanged fixed means become 98.960% / 98.03287%;
strict means become 85.650% / 77.98503%. All other focused rows are unchanged.
The 99% objective remains unmet. Primary evidence: _work/draw-original-direct.asm;
focused package: _work/draw-depth-fourteenth.json/log. Current object disassembly
_work/fourteenth-current.asm emits VCMPE/BPL at 3ba/3c2 and VCMPE/BMI at
3c8/3d0, matching the original branch predicates. Object SHA-256:
a5d385c6e8385bcd581713e528a815432c9f502c284b42f62fac14e4b543bc3b.
Previous accepted focused
and full inputs are preserved as _work/checkpoint13-focused.json and
_work/checkpoint13-corpus.json. No compiler/profile/verifier edits were made.

Validation for this package: 204 ARM TUs compiled, zero failures; native gof2
static-library build passed; complete full corpus has 4619 comparisons and zero
skips. Baseline and checkpoint e07a1f16 exact-pair gates pass with zero losses
(2083 linked / 960 byte). Default 99% gate remains FAIL with the expected
threshold message, not a missing-unit error.

## Investigation after checkpoint 2f20db28

Three isolated source-shape hypotheses were tested and rejected. Production
source is restored to 2f20db28; no new production changes are retained.

- initStarSystem distance addition: original c77ca adds the previous distance
  before c77d0 adds 1600. An explicit random + previous + 1600 expression gave
  identical focused rows to the retained previous += random + 1600 form.
  Package: _work/station-distance-fifteenth.json/log. This expression spelling
  does not explain the remaining gap and is not claimed as recovered C++.
- drawOnScreenInfo target-result type: replacing only local targetMatch bool
  with int reduced source 97.9% -> 97.7%, strict 65.1% -> 64.9%, instructions
  1221 -> 1220. The inferred type is not proven by the ARM integer registers.
  Package: _work/info-target-result-fifteenth.json/log. Rejected and restored;
  the earlier missionTarget type trial was a different local variable.
- drawKey box captures: explicitly saving keyBoxHeight, Layout and top inset
  before constructing the empty temporary String gave identical focused rows.
  Package: _work/key-box-captures-fifteenth.json/log. Accepted-object disassembly
  _work/fourteenth-current.asm already loads boxHeight at 1a5a and top inset
  at 1a60 before the String constructor at 1a6c, corresponding to original
  c813a/c8140 before c814e. No missing pre-constructor read was established.

Direct checks also confirm the retained 32-bit-slot image tables produce LDRH
with index shifted by two for both Image2DCreate and TextureCreate, as original
c7ac8 and c7b84. Changing these table representations has no evidence basis.

After restoration, _work/fifteenth-restored-focused.json is fully identical to
_work/final-focused.json, not merely equal in rounded means. The regenerated
StarMap ARM object SHA-256 remains
 a5d385c6e8385bcd581713e528a815432c9f502c284b42f62fac14e4b543bc3b.
204/204 ARM TUs compiled without failures. Source remains unchanged from the
validated checkpoint; its complete fourteenth-corpus full comparison and native
static-library build remain the applicable final evidence. No new full-corpus
or native-runtime run is claimed. The next investigation must address another
actual call/control/data-flow discrepancy, rather than retry these spellings.

## Follow-up after checkpoint f10955b3

The current inventory (_work/sixteenth-inventory.txt) exposed an actual type
conversion discrepancy in init. Android c6d7e/c6d82 copies the 32-bit word at
Layout+0x90 directly to StarMap+0x1cc. OnTouchMove cabb4/cabd0/cabd4 loads that
word as a float and multiplies the touch deltas by it. Our Layout header declares
field_0x90 as int, so the previous assignment to float dragScale introduced an
integer-to-float conversion that Android does not perform.

StarMap now copies sizeof(dragScale) bytes with __builtin_memcpy. This preserves
the input float bits without strict-aliasing type punning and avoids changing
the shared Layout header or implementation. The accepted compiled init emits
LDR at 46ce and STR at 46d2, matching the original transfer. This is a recovered
word-copy behavior; the builtin's original C++ spelling is not claimed.

Additional primary evidence: Layout constructor d1954 reads a float literal,
d195c stores it at +0x90. Its literals at d2854/d2858 are 0x3ea66666/0x3f266666
(approximately 0.325/0.65), verified from Android .text bytes. Original code also
has other layout/platform branches; these two values are not universal constants.
The shared Layout initialization remains incomplete and is outside this task's
edit scope. This change does not establish native touch usability by itself.

Focused init source: 98.0% -> 98.1%; strict: 76.1% -> 76.4%; instructions
750/737 -> 750/736. Fixed means: 98.965% / 98.04343%; strict means:
85.665% / 78.01672%. All other focused rows are unchanged; 99% remains unmet.
Evidence: _work/init-original-direct.asm, move-original-direct.asm,
layout-ctor-original-sixteenth.asm, layout-drag-literals-sixteenth.txt,
layout-ctor-android-sixteenth.c (IDA crosscheck), init-drag-bits-sixteenth.json/log
and sixteenth-current.asm. The raw Android binary remains the authority.
Accepted StarMap.o SHA-256:
6a02526027cf192e51abca9e5da4557e1b79cb13368e1e32ed0bc5e3ef49ee41.
Previous inputs are preserved as checkpoint15-focused.json/corpus.json in _work.

Validation for this package: 204 ARM TUs compiled, zero failures; native gof2
static-library build passed; full corpus has 4619 comparisons, zero skipped.
Both baseline and previous-checkpoint exact-pair gates pass with zero losses
(2083 linked / 960 byte); the default 99% gate fails with the expected threshold
message. No native executable/runtime test is claimed.

## Follow-up after checkpoint 613af2c6

The unweighted unique-body source mean reaches 99.000%, but the weighted mean
is still 98.12905%; the objective requires both, so this remains PARTIAL.
Only four integer comparison expressions change in StarMap.cpp. No body, weight,
compiler flag or metric is changed.

- draw: three visibility-size tests now express size <= index / size > index,
  instead of index >= size / index < size. Android c91f8/c91fa, c925e/c9260 and
  c9392/c9394 compare count first and skip with BLS. The new object emits the
  corresponding CMP/BLS pairs at 308/30a, 36e/370 and 4b8/4ba. Unsigned truth
  tables and read/call ordering are preserved. This follows verified machine
  comparison direction; it does not prove the original C++ spelling. Source
  98.5% -> 99.0%, strict 87.8% -> 89.1%, instructions 916/909 -> 916/907.
  Focused package: _work/draw-count-seventeenth.json/log.
- init: the key-width maximum test now expresses width > keyBoxWidth. Android
  c732e/c7330 compares the returned width first and conditionally stores under
  GT (c7332/c7336). The retained code used keyBoxWidth < width. Signed truth
  tables and calls are unchanged. Source 98.1% -> 98.3%, strict 76.4% -> 77.0%,
  instruction counts remain 750/736. Focused accepted package:
  _work/init-width-seventeenth.json/log.
- Rejected supplementary trial: OnTouchMove yaw and spin additions were written
  delta + yaw / speed + spin following cac70/cadaa. All focused rows stayed
  identical to the draw-only package. The spelling change was removed rather
  than treating it as another gain. Package: _work/move-add-order-seventeenth.json/log.

Fixed source means: 99.000% / 98.12905%; strict means: 85.760% / 78.24778%.
All other focused function rows are unchanged. Original/current disassembly:
_work/draw-original-direct.asm, init-original-direct.asm, move-original-direct.asm
and seventeenth-current.asm. Accepted StarMap.o SHA-256:
e0d96b374ca3d8da7c933c01a2b091d7c6674f48a319dcc04b95d25ee504591a.
Previous focused/full inputs: _work/checkpoint16-focused.json/corpus.json.
The signed weighted deficit to 99 is 0.87094775 percentage points, using the
unchanged original weights. Largest positive terms (99 - body source) * weight
/ 7101: initStarSystem 0.26996198, OnTouchEnd 0.21905365, drawOnScreenInfo
0.18697367, init 0.07393325, update 0.06837065, C1 0.04689480. Bodies above
99 contribute negative terms. This ranks further investigation; it neither
changes scoring nor proves that any particular source repair can close a term.

Validation: all 204 ARM TUs compiled, zero failures; native gof2 static-library
build passed; complete full corpus has 4619 comparisons, zero skips. Exact-pair
gates against clean baseline and 613af2c6 pass with zero losses: 2083 linked /
960 byte. Default 99% gate remains FAIL because the weighted mean is below 99,
with the expected threshold message. No native executable/runtime test claimed.

## Follow-up after checkpoint f9e870cf

Only initStarSystem's texture-15 planet-scale branch changes. The retained code
computed one float scale through a default and a ternary. Direct Android ARM
c7bf6..c7c32 instead takes the <=105 path first, reads campaign again only on
its >105 path, then reads planetGeom before comparing the second result to 157.
The result selects 0.01/0.004 with ITE LE and LDRLE/LDRGT; the low path uses
0.007. The non-texture-15 path retains its separate 0.004 scaling call.

Source now spells those branches as direct setScaling calls and captures the
second campaign result and subsequent planetGeom read locally. This preserves
thresholds, repeated getter calls, pointer reload after that getter, and scaling
values. It does not prove original C++ spelling. No shared Matrix, Array,
compiler/profile/verifier files are changed.

Two focused packages were measured:
- _work/station-scale-eighteenth.json/log: explicit high/low scale calls with
  the >105 path written first: source 96.3% -> 96.6%, strict 82.0% -> 82.1%,
  instructions 710/680 -> 710/679.
- _work/station-scale-order-eighteenth.json/log: <=105 path first, accepted:
  source 96.9%, strict 83.7%, 710/679. All other focused rows are unchanged.

The final object (_work/eighteenth-current.asm) emits CMP #105/BGT at
3022/3024, reads planetGeom at 3048 after the second campaign call at 3042,
then CMP #157/ITE LE/LDRLE/LDRGT at 304c..3052. These correspond to Android
c7bfc/c7bfe, c7c22 after c7c1c, and c7c26..c7c2c. Source means are
99.030% / 98.18904%; strict means are 85.845% / 78.41776%. Both >=99 are
still required; the weighted threshold remains unmet.
Accepted StarMap.o SHA-256:
32aad49ebce037cd4761026f8f74717d57b5f7e2a9f4d27f5a48c75c6cd01cb6.
Original evidence: _work/station-original-direct.asm, station-android.c (IDA
crosscheck); previous inputs: _work/checkpoint17-focused.json/corpus.json.

Validation: all 204 ARM TUs compiled, zero failures; native gof2 static-library
build passed; complete full corpus has 4619 comparisons and zero skips. Baseline
and f9e870cf exact-pair gates pass with zero losses (2083 linked / 960 byte).
Default 99% gate fails with the expected threshold message because the weighted
mean is below 99. Its remaining deficit is 0.81095620 percentage points. No
native executable/runtime test is claimed.

## Follow-up after checkpoint 80ea89b1

Constructor C1/C2 now spells the texture-15 campaign scale selection as direct
setScaling calls, capturing campaign and then the corresponding system geometry
before the second threshold choice. Android c6956 reads campaign, c695c..c6964
loads geometry around the compare, then c6968..c696c selects scaling constants
and c6980 performs the common scaling call. The retained code computed a local
float default/ternary before reloading geometry for the final call.

The new source retains texture/mission thresholds, repeated campaign getter calls
and the fresh geometry read after that getter. Scale values remain
0.012 default / 0.018 through mission 105 / 0.025 through mission 157.
This matches verified data-flow shape; no original C++ spelling is claimed.
The final compiler merges the direct source call branches into a common call,
so no synthetic duplicate runtime calls were introduced.

Focused package: _work/ctor-scale-nineteenth.json/log. C1 source 98.0% -> 98.9%,
strict 84.2% -> 94.6%, original/current instructions 333/332 -> 333/333.
Call count, ordered control-flow and control inventory each measure 100%.
C2 agrees with C1 and remains excluded from both fixed means. All other focused
rows are unchanged. Means: 99.075% / 98.23125%; strict: 86.365% / 78.90546%.
The weighted target remains unmet.

Original evidence: _work/ctor-original-direct.asm, ctor-android.c (IDA crosscheck).
Current object: _work/nineteenth-current.asm. It reads campaign at 42e6 and
geometry at 42ec..42f4 before loading selected constants. It still chooses
CMP #106/ITE LT/LDRLT/LDRGE at 42f0..42fc, versus original CMP #105/ITE GT/
LDRGT/LDRLE at c6960..c696c; these are equivalent signed integer predicates,
not exact instruction equality. No full-ABI/runtime equivalence is claimed.
Accepted StarMap.o SHA-256:
c4889f58700542a9d753be6f4fd13ed53d4b4c15774c4a90cd9e5bb5b6448427.
Previous inputs: _work/checkpoint18-focused.json/corpus.json.

Native static-library build and all 204 ARM translation units passed. Complete
full corpus: 4619 comparisons, zero skips. Baseline and checkpoint 80ea89b1
preservation gates pass with zero exact-pair losses (2083 linked / 960 byte).
The default target gate exits 1 with its expected threshold failure; the
weighted deficit is 0.76875088 percentage points. Runtime remains untested.

## Follow-up after checkpoint c8b7e875

Constructor campaign scaling now spells the low-mission branch first:
if campaign <=105, set 0.018; otherwise set 0.025. Getter count, geometry
capture order and all threshold values remain unchanged. Original Android
c6960 compares #105 and c6968..c696c uses ITE GT/LDRGT/LDRLE. The previous
source's high-mission-first order compiled as CMP #106/ITE LT/LDRLT/LDRGE.
The new object uses CMP #105 at 42f0, ITE GT at 42f8 and LDRGT/LDRLE at
42fa/42fc. This directly closes the verified predicate-form difference;
no claim of exact original source spelling or whole-function equality is made.

Focused package: _work/ctor-order-twentieth.json/log. C1 source 98.9% -> 99.3%,
strict 94.6% -> 95.8%, original/current instructions stay 333/333. C2 agrees
and is excluded from both means. All other focused rows are unchanged.
Fixed means: 99.095% / 98.25001%; strict 86.425% / 78.96174%.
The weighted 99% target is still unmet, with a 0.74999296 point deficit.
No corpus, weight, shared file, verifier or compilation flag was changed.

Evidence: _work/ctor-original-direct.asm, ctor-android.c (IDA crosscheck),
twentieth-current.asm. Current StarMap.o SHA-256: d7d0ea4bee4c66d8cc336a177ab18134b036fb6af4d30cf084775dfd011c1a5c.
Previous inputs: _work/checkpoint19-focused.json/corpus.json.

Current weighted deficit contributions relative to 99% (fixed original
instruction weights): OnTouchEnd +0.21905365, initStarSystem +0.20997043,
drawOnScreenInfo +0.18697367, init +0.07393325, update +0.06837065,
drawKey +0.04157161 and OnTouchMove +0.02239121 percentage points.
Bodies above 99% partially offset these deficits. This ranks the next
evidence review; it does not establish a compiler ceiling or justify padding.

Native static-library build and all 204 ARM translation units passed. Full
corpus: 4619 comparisons, zero skips. Baseline and checkpoint c8b7e875 gates
preserve all exact pairs (2083 linked / 960 byte), with zero losses. Default
99% gate exits 1 with its threshold message. Native runtime remains untested.

## Follow-up after checkpoint cf030a4e

Two evidence-guided source hypotheses were tested and rejected. Production
StarMap.cpp is restored exactly to cf030a4e; no metric gain is claimed.

- OnTouchEnd passenger warning: original cb464 compares #9 and cb466 uses
  BLE, while the retained object compares #10 at 2460 and uses BLT at 2462.
  These signed integer predicates are equivalent. Splitting the combined
  mission-91/94 condition into an explicit high-capacity continuation to
  mission 94 still generated the same #10/BLT sequence and identical focused
  JSON. The added label/nesting was removed. This is not a reason to force
  registers, flags, artificial calls or arithmetic into the source.
- drawOnScreenInfo: IDA's system target expression places missionTarget on
  the left of bitwise OR. Swapping the two Boolean operands was tested
  without changing call count or mission behavior. Entire focused JSON and
  ARM object SHA-256 remained identical. The simpler retained expression
  was restored; IDA spelling is not proof of original C++ source spelling.

Read-only direct ARM review also confirms that the retained freighter marker
paths capture Status before station-index/warp-gate getter calls (original
c855c before c8564 and c85b8 before c85c0; current e5c before e62 and eba
before ec0). Mission-163 loop bound already uses CMP index,count/BCC
(original c8670/c8672, current f6c/f6e). Freelance/pending-product selection
matches the reviewed getter ordering; no unverified source adjustment is
retained for those reads. This review does not prove entire-function equality.

Packages: _work/passenger-twentyfirst.json/log/asm,
_work/target-or-twentyfirst.json/log, before-passenger-twentyfirst.cpp and
before-target-or-twentyfirst.cpp. Final restored package:
_work/twentyfirst-restored-focused.json/log. A fresh rebuild compiled all 204
ARM translation units with zero failures, compared 22 symbols with zero skips,
and produced byte-identical raw focused JSON to _work/final-focused.json.
Restored StarMap.o SHA-256 equals the accepted checkpoint:
d7d0ea4bee4c66d8cc336a177ab18134b036fb6af4d30cf084775dfd011c1a5c.

The native static-library and full-corpus checks from cf030a4e therefore remain
the latest accepted production validation; no new native/full run is claimed
for rejected trials. Fixed means stay 99.095% / 98.25001%, with weighted
threshold unmet. This follow-up commits only this evidence report. There is
no basis here to declare a compiler ceiling or relax the 99% objective.

## Follow-up after checkpoint 7e7528b7

drawOnScreenInfo now checks X bounds before reading/checking Y, as the
Android instruction sequence does. Original c83a4/c83ac checks X against -50,
c83c2/c83ca checks against width+50, c83ce then loads Y and c83d2/c83da checks
its lower bound. The retained source loaded X and Y together and combined
all four predicates; its object moved the Y lower check ahead of X upper.
The new source splits the same predicates into one X guard followed by one
Y guard. Constants, comparison directions, early-return results and getter
count are unchanged. It uses ordinary control flow without barriers or flags.

New object: X lower c98/ca0, X upper cb6/cbe, then Y load cc2 and lower check
cc6/cce. The change matches verified ordering; no exact C++ source recovery,
whole-function equality or native runtime equivalence is claimed.
Focused package: _work/info-bounds-twentysecond.json/log/asm. Info source
97.9% -> 98.6%, strict 65.1% -> 74.9%, original/current instructions
1207/1221 -> 1207/1197. Call count stays 100%; ordered control-flow 88.8% ->
89.1%. All other focused rows are unchanged. Fixed means become
99.130% / 98.36899%; strict 86.915% / 80.62750%. Weighted 99% remains unmet.
Current StarMap.o SHA-256: c3a12acbd35219b39533f3d129d25207f26cb34ea2a7b967b43c06d8f4e502e1.
Previous inputs: _work/checkpoint22-before-focused.json/corpus.json.

The current weighted shortfall is 0.63100972 percentage points. Deficit
contributions relative to 99% are OnTouchEnd +0.21905365, initStarSystem
+0.20997043, init +0.07393325, update +0.06837065, info +0.06799042,
drawKey +0.04157161 and OnTouchMove +0.02239121; bodies above 99% offset
part of their sum. These are measured priorities, not a proven ceiling.

Two station-angle hypotheses were rejected before this retained package:

- Capture usedFlags after random nextInt and reuse it through flag store and
  size read: original c775c..c776c uses this captured pointer. It removes the
  current object's extra member reload after the flag store and raises strict
  station matching 83.7% -> 84.0%, but source decreases 96.9% -> 96.8% and
  current instruction count 679 -> 678. No separate valid-game behavior defect
  is demonstrated by the alias-sensitive reread, so this score-lowering variant
  is not retained. Package: _work/angle-flags-twentysecond.json/log; disassembly
  _work/twentysecond-current.asm describes this rejected object, not final.
- Compute stationAngle before assigning it into the member array and use that
  local for radians. This tests original c7772 division before c7786 loads the
  stationAngles member. Entire focused JSON is identical to the accepted
  pre-turn checkpoint. Removed the extra source local. Package:
  _work/angle-value-twentysecond.json/log.

Original evidence: station-original-direct.asm, station-android.c (IDA
crosscheck), info-original-direct.asm and info-android.c. Source for each
station trial was restored from _work/before-angle-flags-twentysecond.cpp
with a fresh file timestamp before the final focused build. No rejected
station edit remains in the production diff.

Native static-library build and all 204 ARM translation units passed. Full
corpus: 4619 comparisons, zero skips. Baseline and checkpoint 7e7528b7
preservation gates pass with zero losses (2083 linked / 960 byte). The default
99% gate fails with its expected threshold message. Runtime remains untested.

## Follow-up after checkpoint 43f3e1c2

initStarSystem now searches for an unused orbital-angle slot with a completion
flag. Each failed random selection retries; only an unused slot is marked,
converted to stationAngle and completes the loop. A local angleFlags pointer
captures the member after nextInt, matching original c775c. The completed
integer angle is assigned to stationAngles and converted to radians afterward.

This is an ordinary search loop with an actual failure/retry path, not an added
one-iteration loop around the prior do/while. There are no dead padding calls,
forced registers or compiler flags. It preserves one random call per candidate,
marks exactly the accepted slot and uses the same unsigned division, product
and float constants. Original method assumes at least one unused slot; this
change does not invent a new behavior when that native precondition fails.

Direct Android ARM c7764/c7766 retries occupied slots, c7768..c7776 sets the
completion flag, stores occupancy and calculates the angle; c7778/c777a
checks the completion flag before converting/storing angle at c777c..c778a.
New object: 2b5a/2b5c retries, 2b5e..2b6c accepts/calculates, and 2b6e/2b70
uses the same LSLS #31/BEQ completion test. No exact C++ source spelling or
whole-function equality is claimed. The earlier local-pointer-only hypothesis
in the preceding report omitted this control structure and was rejected;
it is not reused as an independent metric workaround.

Focused package: _work/angle-search-twentythird.json/log/asm. Station source
96.9% -> 97.3%, strict 83.7% -> 84.7%, original/current instructions
710/679 -> 710/683. Ordered control-flow 94.1% -> 94.8%, control inventory
96.2% -> 96.9%; call count stays 99.0%. All other focused rows are unchanged.
The corpus and original instruction weights remain fixed. Weighted target
remains unmet. Current StarMap.o SHA-256: e650943d8de8560afae62079ab176de5b80264992479581ade08d5278c49e0ae.
Evidence: _work/station-original-direct.asm, station-android.c (IDA crosscheck),
angle-search-twentythird.asm. Previous inputs:
_work/checkpoint23-before-focused.json/corpus.json.

Fixed means: 99.150% / 98.40898%; strict: 86.965% / 80.72749%. The
weighted shortfall remains 0.59101535 percentage points. Native static-library
build and all 204 ARM translation units passed. Full corpus: 4619 comparisons,
zero skips. Baseline and checkpoint 43f3e1c2 preservation gates pass with zero
losses (2083 linked / 960 byte). The default 99% gate exits 1 with the expected
threshold message. Runtime remains untested.

## Follow-up after checkpoint fcb1741b

Three source hypotheses were measured and rejected. Production StarMap.cpp
is restored to fcb1741b; this follow-up changes only this evidence report.

- OnTouchEnd mission 142: guard both checks with one warningText <0 branch,
  then use if/else-if for missing equipment and free-space warnings. This
  keeps repeated campaign/station/ship calls and their short-circuit behavior.
  The compiler already produces the same result for the retained two guards:
  complete focused JSON is identical. Removed the extra source nesting.
- Info target-result int: the prior viewport-order repair changed register
  allocation across the method, so the local-result type was retested against
  this new checkpoint, not reused from the fifteenth report. Original c876e..
  c8776 uses CMP #-1/IT GT/ORRS; the new checkpoint uses EOR with the return
  value's sign bit at 1058..105e. Changing only targetMatch bool to int worsens
  info source 98.6% -> 98.5%, strict 74.9% -> 74.7%, current instructions
  1197 -> 1196, ordered control-flow 89.1% -> 88.6%. Rejected. ARM does not
  prove the original C++ local type, and no register/flag workaround is added.
- Info mission 120: explicit station/system branches and a negative-index
  continuation to freighter checks were tested against the ternary condition.
  Original c8544/c8546 compares #0/BGE while the current e34/e38 sequence
  compares #-1/BGT (addresses e34/e38 in angle-search-twentythird.asm).
  The predicates are equivalent signed tests; complete focused JSON and object
  SHA-256 remain identical despite the source restructuring. Removed the label.

Read-only mapping checked all seven Android getStationEnumIndex call sites in
info against the current object. The first, mission-163 search, fallback and
pending-product tests already preserve their signed zero/negative branch forms.
The target Boolean formation and mission-120/freelance branch layout retain
instruction differences. This map identifies residual evidence, not a proven
compiler ceiling or permission to change the metric. The signed int32_t return
casts remain necessary; the shared header's unsigned declaration is unchanged.

Packages: _work/142-group-twentyfourth.json/log,
target-result-twentyfourth.json/log and mission120-twentyfourth.json/log.
Own source backups: before-142-group-twentyfourth.cpp,
before-target-result-twentyfourth.cpp and before-mission120-twentyfourth.cpp.
After restoring with a fresh timestamp, _work/twentyfourth-restored-focused
JSON/log proves 204 ARM TUs compiled, zero failures, 22 comparisons, zero skips,
and raw JSON byte equality to the accepted _work/final-focused.json.
Restored StarMap.o SHA-256 equals the accepted fcb1741b object:
e650943d8de8560afae62079ab176de5b80264992479581ade08d5278c49e0ae.

The previous native static-library/full-corpus validations remain the latest
accepted production evidence; no new full or native run is claimed for these
rejected variants. Fixed means stay 99.150% / 98.40898%, weighted target unmet.

## Follow-up after checkpoint f7cbbae0

drawOnScreenInfo's icon loop now captures Globals::Canvas once after confirming
that an icon exists, before choosing the first-icon or regular-icon branch.
Both branches draw through that snapshot. Original Android c9036 loads Canvas
before c903a tests the loop index, then the first-icon path reads iPadHD at
c904c and iconWidth at c9050. The retained object used separate Canvas reads
at 1918 for regular icons and 193e after the first-icon offset calculations.

The new object reads Canvas at 1916, tests index at 191a, then reads iPadHD
at 192c and width at 1930. Its regular drawing path still reloads missionIconWidth
only after DrawImage2D returns, as original c9040/c9044 does. No callback-spanning
icon-width snapshot was added. The change removes one extra instruction and
matches verified receiver/read ordering; no full-function or runtime equivalence
is claimed. It does not add calls, barriers, padding or compiler flags.

Focused package: _work/icon-canvas-final-twentyfifth.json/log. Info source
remains 98.6%; strict 74.9% -> 75.5%, current instructions 1197 -> 1196,
instruction inventory 97.1% -> 97.2%, opcode inventory 98.5% -> 98.4%.
Call/control scores and all other focused rows are unchanged. The fixed means
remain 99.150% / 98.40898%; strict means improve to 86.995% / 80.82947%.
This is a verified ordering correction, not a claimed gain toward the source
threshold. The weighted 99% objective remains unmet.

An additional first-icon offset spelling (negative Y offset first, then width,
then X offset) was tested. It generated identical complete focused JSON and
object bytes to the Canvas-only version, so the extra source local/order change
was removed before final validation. Packages:
_work/icon-canvas-twentyfifth.json/log, icon-offsets-twentyfifth.json/log,
twentyfifth-current.asm. The latter disassembly also describes the final object,
whose fresh focused rebuild confirms identical SHA-256:
7cfc414cb456444e775d12e657c5c23d6507a5545af3096010f027fdb2380de9.
Own backups: before-icon-canvas-twentyfifth.cpp and
icon-canvas-accepted-twentyfifth.cpp. Previous accepted inputs:
_work/checkpoint25-before-focused.json/corpus.json.
Original evidence: _work/info-original-direct.asm, info-android.c (IDA crosscheck).

Native static-library build and all 204 ARM translation units passed. Full
corpus: 4619 comparisons, zero skips. Baseline and checkpoint f7cbbae0 gates
preserve all exact pairs (2083 linked / 960 byte), with zero losses. Default
99% gate exits 1 with its threshold message. Native runtime remains untested.

## Follow-up after checkpoint 0fe3756d

Two source-form trials were rejected; production source is unchanged.

- The final race-image branches used race != 0 with the default image first,
  rather than race == 0 with the neutral image first. This produced identical
  complete focused JSON and object bytes to the accepted checkpoint; the trial
  was removed. Package: _work/race-branch-twentysixth.json/log.
- OnTouchMove's pitch clamp was expressed as one nested conditional using
  the same two comparisons and retaining the negative-bound NaN result.
  This was a source-form hypothesis, not a recovered original C++ expression.
  Source match fell 98.4% -> 97.4%, despite strict rising 79.2% -> 80.9%;
  current instruction count fell 255 -> 252, instruction inventory 97.7% ->
  95.9%, opcode inventory 98.1% -> 96.3%, size match 96.2% -> 95.0%.
  All other rows were unchanged. Rejected package:
  _work/move-clamp-twentysixth.json/log.

Read-only comparison of _work/move-original-direct.asm with
_work/twentyfifth-current.asm confirms both original and accepted objects
already use VNMUL for the negative products. Original cac74/cac7c/cac8a/cac9c
contain four pitch comparisons; accepted 4f08/4f10 contains two. The literal
pool at cae60 is -8192 (0xc6000000), cae68 is +8192 (0x46000000).
No redundant checks were inserted merely to reproduce compiler instruction
count. OnTouchEnd's separate original 3217 text-window site remains a known
structural difference; no extra gameplay call or new source correction is
claimed from that observation.

Fresh restore package _work/twentysixth-restored-focused.json/log passes all
204 ARM translation units with zero failures, 22 comparisons and zero skips.
Its complete JSON is byte-identical to _work/final-focused.json, and the
restored StarMap.o SHA-256 is
7cfc414cb456444e775d12e657c5c23d6507a5545af3096010f027fdb2380de9.
Backups: before-race-branch-twentysixth.cpp and
before-move-clamp-twentysixth.cpp. Source means remain 99.150% / 98.40898%;
strict means remain 86.995% / 80.82947%. The native static-library and full
corpus results below remain those of checkpoint 0fe3756d; neither is claimed
as a new run for this report-only follow-up. The weighted 99% objective is
still unmet; native runtime remains untested.

## Follow-up after checkpoint 03aa0342

Production source remains unchanged after five focused source-form trials.
They test concrete Android instruction differences, not recovered C++ syntax.

- OnTouchEnd's confirmation tail was nested under else if (result == 0),
  following the original result-1/result-0 dispatch at caec4/caec8. Current
  1eac/1eb2 tests zero first. The nested form produced identical complete
  focused JSON; it was removed. Package: choice-result-twentyseventh.json/log.
- init's target-station ownership test combined its two Boolean predicates
  with bitwise AND. Original c6f7a..c6f82 reads playerStation and compares it
  before the conditional null comparison; current 4af0..4afe tests null first.
  The alternate expression produced identical JSON and object SHA-256; it
  was removed. Package: target-ownership-twentyseventh.json/log. No invalid
  ownership or callback-visible bug is claimed for the retained expression.
- update's station-centering yaw and pitch assignments were placed directly
  into their positive/negative quarter-step branches. Original ca218..ca228
  and ca52c..ca53e branch to select the direction; current 3832..383e and
  3878..3888 use conditional instructions. The combined trial reduced update
  source 98.5% -> 98.4%, instruction inventory 96.7% -> 96.3% and opcode
  inventory 98.2% -> 97.9%; strict rose 82.1% -> 82.2%. Counts, calls and all
  other rows were unchanged. Package: angle-branches-twentyseventh.json/log.
- Isolated yaw assignment produced identical complete JSON to the accepted
  version. Isolated pitch assignment produced identical complete JSON to the
  rejected combined trial. Both were removed. Packages:
  yaw-branch-twentyseventh.json/log and pitch-branch-twentyseventh.json/log.

Read-only checks rule out two additional proposed corrections: the station
inertia block at current 282a..287e already matches original cb896..cb8ea's
coordinate loads, absolute-value selections, 150 thresholds and velocity
stores. Cargo/jumpCost are signed int32_t, matching original cb5e0/cb5e2 and
cb60a/cb60e's signed BGE conditions. These observations do not prove the
remaining function equivalent. Allocation-shape inventory review of init,
info, update and drawKey did not change the verifier, its normalizations or
its weights. The previously documented shared Matrix constructor/inline
initialization difference remains; no scope workaround was introduced.

Fresh restore: _work/twentyseventh-restored-focused.json/log, 204 ARM TUs
compiled, zero failures, 22 rows, zero skips. Complete restored JSON is
byte-identical to _work/final-focused.json. StarMap.o SHA-256 remains
7cfc414cb456444e775d12e657c5c23d6507a5545af3096010f027fdb2380de9.
Backups: before-choice-result-twentyseventh.cpp,
before-target-ownership-twentyseventh.cpp and
before-angle-branches-twentyseventh.cpp. All package names above are in _work.

Source means remain 99.150% / 98.40898%; strict 86.995% / 80.82947%.
The native static-library and full-corpus validations remain those of
0fe3756d; this report-only package claims no new full/native run.
The weighted 99% target is still unmet. Native runtime remains untested.

## Follow-up after checkpoint 72465845

The station text X coordinate in drawOnScreenInfo is now converted once to
int after subtracting half the text width, and that integer is used by station
name and technology draws. The system text branch already used an int.
The previous station implementation kept X as float until each drawing call.
This supersedes the earlier retained-form description in the fifth checkpoint;
it does not claim recovery of the original C++ spelling.

Original Android c897a subtracts the half-width; c898e converts X to signed
integer and c8992 moves it into r4 before c8996's repeated Station::getIndex.
The name draw at c8b76 passes that integer in r3. New object 1268 subtracts,
127c converts, 1280 moves into r4 and 1284 performs the repeated getter.
Old object performed the repeated getter at 1278 before that X conversion.
The new scheduling follows the original early conversion and integer lifetime.
No coordinate is converted back to float, and no marker/scratchVector reload
across callbacks was replaced by this local text-coordinate snapshot.

Focused package: _work/station-textx-twentyeighth.json/log. Info source
98.6% -> 98.8%; strict 75.5% -> 75.3%; current instructions 1196 -> 1198;
instruction inventory 97.2% -> 97.5%, opcode inventory 98.4% -> 98.7%,
size match 99.2% -> 99.4%. Other focused rows, including exact bodies and
aliases, are unchanged. Fixed source means become 99.160% / 98.44298%;
strict means become 86.985% / 80.79548%. This is a measured source gain
with a small strict decline, not an exact-match or runtime-equivalence claim.
The weighted target remains below 99%.

Evidence: _work/info-original-direct.asm and
_work/station-textx-twentyeighth.asm. Backup:
_work/before-station-textx-twentyeighth.cpp. Previous checkpoint inputs:
_work/checkpoint28-before-focused.json/corpus.json. Final object SHA-256:
1a12eaa4a0002c78e01fccc2a8a8b2a7aee04366607cd84e6fe97bfdace9879d.

Native static-library build and all 204 ARM translation units pass. The full
corpus contains 4619 comparisons with zero skips. Baseline and previous
checkpoint 72465845 preservation gates pass with zero lost exact pairs;
all 2083 linked-exact / 960 byte-exact pairs are retained. Default 99% gate
exits 1 with the unchanged threshold message. Native runtime is untested.

## Follow-up after checkpoint 364ddd7d

Two initStarSystem source-form trials were rejected because each produced
identical complete focused JSON and object bytes to the accepted checkpoint.

- An explicit i >= stationGeoms->size() break at the beginning of the main
  station loop was tested in place of the for-condition. Original c76e0/c76e8
  tests exit at the top and c792a jumps back unconditionally; current
  2d32..2d3a tests continuation at the bottom. The alternate source form did
  not change compiler output. Package: station-loop-twentyninth.json/log.
- The ring predicate was expressed as a negative guard that skipped only the
  ring creation block. It preserved the unsigned texture bound before the
  shift and the conditional orbitHasPlanetRing call. Original c7896 branches
  to the secondary test when the texture bit is absent; c794a branches back
  to creation on success. The guard did not change compiler output. Package:
  ring-guard-twentyninth.json/log. Neither expression is recovered C++ syntax.

Read-only comparison confirms the station-distance calculation already
matches: original c77ac/c77b2..c77ba chooses previous distance before nextInt
at c77c6, then c77ca adds it and c77d0 adds 1600. Current 2b96..2bba has the
same ordering. The previously rejected fifteenth-checkpoint addition trial
was not repeated.

Allocation-shape counter review also found an input-data caveat: original-only
BVC and SUBS #35 tokens correspond to the two Thumb halfwords at c7d74/c7d76.
Their combined word is 0x3c23d70a (float 0.01), loaded as scale data by LDRLE
at c7c2a. These are literal data, not evidence of a missing gameplay branch.
The raw/normalized inventory distinction does not establish a compiler ceiling
or prove a maximum attainable score. No instructions, padding or extra calls
were added to imitate literal bytes; the frozen verifier and 7101 weights
remain unchanged.

Fresh restore package: _work/twentyninth-restored-focused.json/log. All 204
ARM translation units compiled, zero failures, 22 rows, zero skips. Complete
JSON is byte-identical to _work/final-focused.json, and StarMap.o SHA-256 is
1a12eaa4a0002c78e01fccc2a8a8b2a7aee04366607cd84e6fe97bfdace9879d.
Backups: before-station-loop-twentyninth.cpp and
before-ring-guard-twentyninth.cpp, both under _work. Original evidence:
_work/station-original-direct.asm; accepted disassembly:
_work/station-textx-twentyeighth.asm.

Production code and source means remain 99.160% / 98.44298%; strict means
remain 86.985% / 80.79548%. Native static-library/full-corpus results below
remain those of 364ddd7d; this report-only follow-up claims no new full/native
run. Weighted 99% is still unmet, and native runtime remains untested.

## Follow-up after checkpoint 9f17e758

update's two projection-result selections were tested with an explicit local
depth default of -1.0f, changed to +1.0f when GetScreenPosition returns nonzero,
followed by the existing member-array reload and single Z store. The accepted
source uses a ternary with the same values. No extra stores or calls were
introduced, and this was a source-form hypothesis, not recovered C++ syntax.

Original system c9bca/c9bd0 loads -1/+1; c9c0a starts with -1 and c9c10/c9c12
selects +1 under NE. Original station c9cae/c9cb4/c9cb6 follows the same rule.
Current 3230/3232 and 32d6/32d8 selects -1 under EQ, starting from +1.
The inverse condition and inverse default give the same zero/nonzero result.
Original c9c06/c9caa and current 3226/32cc reload the member arrays after
GetScreenPosition; the trial retained those callback-spanning reloads.

The trial produced identical complete focused JSON and identical object bytes
to the accepted checkpoint; it was removed. Package:
_work/depth-select-thirtieth.json/log. This rules out the explicit-local form
as a source-match improvement, without claiming a compiler ceiling or exact
function equivalence. Primary evidence: _work/update-original-direct.asm;
accepted disassembly: _work/station-textx-twentyeighth.asm.

Fresh restore: _work/thirtieth-restored-focused.json/log, all 204 ARM TUs
compiled, zero failures, 22 rows, zero skips. Complete JSON is byte-identical
to _work/final-focused.json, and StarMap.o SHA-256 is
1a12eaa4a0002c78e01fccc2a8a8b2a7aee04366607cd84e6fe97bfdace9879d.
Backup: _work/before-depth-select-thirtieth.cpp. Production source, source
means 99.160% / 98.44298%, strict means 86.985% / 80.79548% and exact pairs
remain unchanged. Full-corpus and native static-library results below remain
those of 364ddd7d; no new full/native run is claimed. Weighted 99% remains
unmet; native runtime remains untested.

## Investigation after checkpoint 5bf36633

No production source changes or new compiler experiments were made. A concrete
normalization issue in the existing OnTouchEnd measurement was traced by
read-only inspection of the unchanged verifier and original/accepted objects.
This extends the previously noted raw-call/token discrepancy with addresses.

Original raw machine code has 136 direct call sites and 19 indirect sites;
current code has 134 direct and 19 indirect sites. A primary-ELF PLT-callee
counter and current-object relocation counter agree that the only raw direct
callee-count difference is the separate GameText::getText/ChoiceWindow::set
pair for original text 3217. This counts static sites, not calls per execution;
it does not prove an extra or missing gameplay action.

Existing normalized tokens instead report 133 original direct calls versus
134 current. Three real original call instructions are treated as pool data:

| Original call | Callee | Range converted to data | Spurious pool-reference origin |
|---|---|---|---|
| cb6e0 | SolarSystem::getIndex | cb6e0..cb6e4 | cb37c |
| cb6e8 | SolarSystem::systemIsInSystemRoutes | cb6e8..cb6ec | cb364 |
| cb712 | StarMap::initStarSystem | cb710..cb718 | cb374 |

cb364/cb374/cb37c are pool words referenced by real code at cb2b4/cb2fe/cb308.
Stripped-target disassembly decodes the words themselves as PC-relative LDRs.
The current literal normalizer scans those apparent LDRs and follows their
spurious references into executable code. Its exact-boundary range test does
not distinguish these apparent pool instructions from real references. The
three calls above are then converted to .word tokens. The apparent reference
from cb36c to cb4e0 overlaps hasEquipment at cb4e2, but that candidate range
fails the exact-boundary proof and is not among the three removed calls.

This explains why restoring a separate static warning pair cannot be evaluated
by assuming the normalized call-count signal will approach its native raw
count. It does not prove a maximum achievable score, remove the fixed 99%
requirement, or justify extra calls, padding or register-forcing source edits.
The verifier, literal normalization and original weights remain unchanged.

Evidence: _work/end-original-direct.asm, accepted
_work/station-textx-twentyeighth.asm, read-only target object
cmake-build-match/verify/target/game/world/StarMap.o, and compact tracked
snapshot docs/validation/starmap_2026-10-02_call_normalization.json.
Detailed range data also remains in _work/thirtyfirst-normalized-calls.json.

Accepted object SHA-256 was rechecked unchanged:
1a12eaa4a0002c78e01fccc2a8a8b2a7aee04366607cd84e6fe97bfdace9879d.
No new build/full-corpus result is claimed. Source means remain 99.160% /
98.44298%, strict 86.985% / 80.79548%; existing full preservation and native
static-library results remain valid for the unchanged production source.
The weighted target is unmet, and native runtime remains untested.

## Investigation after checkpoint b1ca08e8

A drawKey source trial grouped drawBox and the imageX/textX/y coordinate
assignments into one comma expression. This extended the temporary by-value
String lifetime through those integer calculations without creating another
String copy. The primary Android code computes coordinates at c817c..c8192
between drawBox at c8178 and String destruction at c8196. Accepted code has
drawBox at 1a58, destruction at 1a5e and coordinates after it. This was a
source-shape hypothesis; the original C++ expression was not recovered.

The focused trial _work/key-lifetime-thirtysecond.json is identical to the
accepted focused JSON across all fields and 22 rows, including drawKey source
97.8%, strict 34.8% and 246/243 original/current instructions. There was no
measurable benefit, so the comma-expression trial was removed. No trial-object
byte-identity claim is made; the trial object was not separately hashed before
restoration. Backup: _work/before-key-lifetime-thirtysecond.cpp. Evidence:
_work/drawkey-original-addresses.asm and accepted
_work/station-textx-twentyeighth.asm.

A fresh restored-source focused run compiled 204/204 TUs, zero failures,
compared 22 symbols with zero skips, and reproduced the entire accepted JSON.
The build log contains transient MSYS child-copy/mkdir diagnostics, but the
terminal build summary reports all 204 compiled with zero failures; neither
that summary alone nor exit status was used instead of the object check.
Restored StarMap.o SHA-256 also equals the accepted object:
1a12eaa4a0002c78e01fccc2a8a8b2a7aee04366607cd84e6fe97bfdace9879d.
Fresh evidence: _work/thirtysecond-restored-focused.json/log.

Production source remains unchanged. Means remain source 99.160% / 98.44298%
and strict 86.985% / 80.79548%, with six unique linked-exact and three unique
byte-exact bodies. Full-corpus/native results below remain those of 364ddd7d;
no new full-corpus or native-library run is claimed. Both 99% source gates
must pass before completion; the weighted target remains unmet and native
runtime remains untested.

## Investigation after checkpoint 0d0ad086

An init legend-width trial explicitly retained the last maximum in keyWidth,
while still reloading keyBoxWidth after each GetTextWidth callback. The final
padding addition used that local maximum. Original c7324..c7336 calls width,
reloads the field and conditionally updates both field and register; c7346
adds padding using the register. The source trial produced an identical
focused JSON and identical complete StarMap.o SHA-256. It was removed.

Inspection of the accepted disassembly confirms that it already implements
this sequence at 4c3a..4c4c and 4c5c. The suspected final field reload is absent;
this is not a remaining source-shape defect. No exact original C++ spelling
is claimed. Init remains source 98.3%, strict 77.0%, 750/736 instructions.
Evidence: _work/init-original-direct.asm,
_work/station-textx-twentyeighth.asm, _work/key-width-thirtythird.json/log and
_work/before-key-width-thirtythird.cpp.

A fresh restored-source focused build compiled 204/204 TUs with zero failures,
compared 22 symbols with zero skips, and reproduced the entire accepted JSON
and object hash 1a12eaa4a0002c78e01fccc2a8a8b2a7aee04366607cd84e6fe97bfdace9879d.
Logs/reports: _work/thirtythird-restored-focused.json/log. Production source
is unchanged; source means remain 99.160% / 98.44298%. No new full-corpus or
native-library result is claimed; those validations remain the accepted
364ddd7d results. Weighted 99% is still unmet, and runtime remains untested.

## Follow-up after checkpoint de7b2d56

The initial OnTouchEnd drag/inertia block was inspected before changing code.
Accepted 1f16..1f82 matches primary Android cb0a2..cb10e for the float loads,
negation/positive selection, ordered GT threshold 3.0, momentum store, dragging
clear and X/Y velocity stores. The apparent unordered condition in the IDA
pseudocode is not the machine predicate: cb0ea/cb0ec and cb0f8/cb0fa use IT GT.
No OnTouchEnd source change was justified by this block.

The system-label alpha in drawOnScreenInfo now first clamps alpha to 64,
then restores the original alpha for selectedSystem == index and for
selectedSystem < 0. The selected index is captured after the name draw and
String destruction, before SetColor. There are no intervening callbacks
between these three integer selections. This preserves final alpha for all
signed alpha/index values and uses ordinary conditional assignments.

Primary Android c8ce8..c8d12 loads alpha/selected index, applies GE clamp,
EQ restoration and LT restoration, then invokes SetColor. The new object
1664..168e implements the same instruction sequence, including Canvas load
between the equality and negative-index checks. This proves this local
machine ordering, not an exact original C++ spelling or runtime equivalence.

Focused info source remains 98.8%, strict improves 75.3% -> 78.3%, and
1207/1198 instructions remain unchanged. Every other focused row is unchanged
when compared by symbol (report row order can differ). Inventory, opcode,
control-flow, call-count and size metrics also remain unchanged. Fixed source
means stay 99.160% / 98.44298%; strict means improve 86.985% / 80.79548% ->
87.135% / 81.30541%. The weighted 99% requirement remains unmet.

Focused build: 204/204 TUs, zero failures; 22 comparisons, zero skips;
six unique linked-exact and three unique byte-exact bodies. Native UCRT gof2
static-library build passed. Native runtime remains untested.
Evidence: _work/object-alpha-thirtyfourth.json/log/asm,
_work/native-thirtyfourth.log, _work/info-original-direct.asm,
_work/end-original-direct.asm and _work/before-object-alpha-thirtyfourth.cpp.
Accepted-before inputs were preserved as
_work/checkpoint34-before-focused.json/corpus.json.

Full corpus: 4619 comparisons, zero skips, 2083 linked-exact / 960 byte-exact.
Preservation gates pass against both the clean baseline and checkpoint de7b2d56,
with zero lost (unit, symbol) pairs. Fixed-target gate still fails with the
expected >=99.0% target-not-reached assertion. Gate logs:
_work/thirtyfourth-gate.json, thirtyfourth-previous-gate.json and
thirtyfourth-target-gate.log (exits 0, 0, 1). Full report/log:
_work/thirtyfourth-corpus.json/log. Accepted StarMap.o SHA-256:
7bf78a18dd1e94481cc71f880ebe406dd9ddfa380f69cebe697d95293b75bb05.

## Investigation after checkpoint 84fab485

The galaxy camera-bound calculation already matches primary Android
ca194..ca1cc at accepted 37b8..37f0: both integer pan conversions, float sums,
integer coordinate conversions and the upper-X comparison are the same.
No source reordering was justified by this block. The final station pitch
clamp also loads pitch before yaw conversion in the accepted object despite
the source statement order; original ca60a..ca61a and accepted 3c26..3c2a
confirm the loads. Original four pitch comparisons versus accepted two are
not permission to add redundant comparisons solely for instruction padding.

A separate station-centering trial moved the existing pitchFactor/pitchStep
selection before the yaw assignment and yawDistance <= 10 completion store.
Yaw and pitch formulas, signed integer differences, quarter-step factors,
completion thresholds and the final store order were preserved. There are no
callbacks between these local calculations. Original ca1e0..ca216 prepares
pitch/yaw integer distances before the angle writes; the source trial tested
whether earlier step preparation would improve compilation. Exact original
C++ spelling was not recovered.

The trial adds one instruction (971/966 -> 971/967 original/current), lowers
instruction inventory 96.7% -> 96.5% and opcode inventory 98.2% -> 98.1%,
and improves size score 99.6% -> 99.7%. Update source 98.5% and strict 82.1%
remain unchanged, as do every other focused row. Overall source and strict
means remain unchanged. This offers no benefit toward the requested target,
so the trial was removed.

Evidence: _work/update-original-direct.asm, accepted
_work/object-alpha-thirtyfourth.asm, _work/center-steps-thirtyfifth.json/log
and _work/before-center-steps-thirtyfifth.cpp.

Fresh restored-source focused run: 204/204 ARM TUs, zero failures, 22
comparisons, zero skips; entire report and accepted object hash are identical.
Object SHA-256: 7bf78a18dd1e94481cc71f880ebe406dd9ddfa380f69cebe697d95293b75bb05.
Fresh evidence: _work/thirtyfifth-restored-focused.json/log. Production source
is unchanged. Source means remain 99.160% / 98.44298%, strict 87.135% /
81.30541%; weighted 99% remains unmet. No new full-corpus/native-library run
is claimed; those results remain the accepted 84fab485 validations. Native
runtime remains untested.

## Investigation after checkpoint 8273bb53

Read-only raw-machine call-site counts for update match: 74 direct and six
register-indirect sites in both primary Android and accepted object. The direct
original count includes only resolved PLT targets, excluding pool word ca79c
which the stripped disassembler displays as a BL to an unrelated address.
Current direct sites are counted from ARM call relocations. Normalized direct
call tokens also count 74 on both sides. This is static-site evidence, not
calls per execution or runtime correctness.

The only direct callee-name differences are original ArrayRelease<bool> and
Array<bool>::D2 at ca282/ca28c versus current ArrayRelease<uint8_t> and
Array<uint8_t>::D2, one each. Every other direct-callee count matches. The
six original indirect sites c9ee2/c9eea/c9ef2/c9efe/c9f06/c9f0e correspond to
accepted 350a/3512/351a/3526/352e/3536. No missing gameplay call was identified
by this count. This does not authorize changing the retained array type or
manufacturing template symbols; the exact-pair preservation requirement stays
in force. Future update trials should address arithmetic/control differences,
not add calls based on the five-instruction count deficit.

Station marker selection also already matches primary c78dc..c7908 at accepted
2ce4..2d10: marker guard, current-system comparison, Station::getIndex, then
fresh Status pointer/field_80 loads and the conditional centeredStation store.
No earlier Status snapshot was introduced. Final playing-time seed also keeps
both return words when passing to setSeed; no narrowing correction is needed.
Auto-pulse update reloads pulseSystem/autoTimer after OnTouchBegin at original
ca0ce/ca0d2 and accepted 36f2/36f6, preserving callback-visible state. Its scale
calculation already uses float division followed by double multiplication and
float conversion. These inspected regions do not justify another source trial.

Evidence: _work/update-original-direct.asm, _work/station-original-direct.asm,
_work/object-alpha-thirtyfourth.asm and tracked
 docs/validation/starmap_2026-10-02_update_calls.json. Accepted object SHA-256
was rechecked unchanged: 7bf78a18dd1e94481cc71f880ebe406dd9ddfa380f69cebe697d95293b75bb05.
No production change or new build/full-corpus result is claimed. Source means
remain 99.160% / 98.44298%, strict 87.135% / 81.30541%; latest validations
remain those of 84fab485 and the restored focused run after its investigation.
Weighted 99% remains unmet; native runtime remains untested.

## Investigation after checkpoint 578ceaa4

A read-only update instruction-inventory comparison located the native integer
yaw-distance conditional negation among remaining differences. Original
ca210..ca216 uses SUBS followed by IT MI/NEGMI, while accepted 385e..3862
uses a zero comparison, BPL and NEGS. The source already computes the same
integer absolute distance; no new gameplay formula was inferred.

A trial replaced the two-line subtraction/negative test with
abs(yaw - target). This standard-library spelling preserves the existing
integer expression and its thresholds and adds no calls, barriers or forced
registers. The entire focused JSON and complete ARM object were identical to
the accepted state: update source 98.5%, strict 82.1%, 971/966 instructions.
Thus the compiler already recognizes the manual absolute-value expression;
this spelling does not repair the conditional-instruction difference. The
trial was removed. This does not establish a compiler ceiling or relax 99%.

Evidence: _work/yaw-abs-thirtyseventh.json/log,
_work/before-yaw-abs-thirtyseventh.cpp, _work/update-original-direct.asm and
_work/object-alpha-thirtyfourth.asm. A fresh restored-source run compiled
204/204 ARM TUs with zero failures, compared 22 symbols with zero skips,
and reproduced the entire accepted JSON and object SHA-256:
7bf78a18dd1e94481cc71f880ebe406dd9ddfa380f69cebe697d95293b75bb05.
Fresh evidence: _work/thirtyseventh-restored-focused.json/log.
Production source remains unchanged; source means stay 99.160% / 98.44298%,
strict 87.135% / 81.30541%. No new full-corpus or native-library result is
claimed; those validations remain 84fab485. Weighted 99% remains unmet;
native runtime remains untested.

## Follow-up after checkpoint e2f1fb0c

The freelance mission marker in drawOnScreenInfo now computes a local
freelanceTarget in its station/system branch and writes iconBuffer[2] once
afterward. The station branch still obtains stationId before getType; the
system branch still captures its SolarSystem before getType, conditionally
checks cargo 115/1, and selects agent station versus target station. The signed
station-enumeration test, all calls and their conditional paths are preserved.
No calls, barriers, forced registers or template symbols were added.

Primary Android system success at c8850 branches to the common marker write
c885e..c8866; station equality at c885a/c885c reaches that same write. The
subsequent mode check c8868/c886c skips station-only warp-gate processing.
Accepted-before code instead wrote at 113a..1142 in the system path and again
in the station path. New object system success 1134 and station equality
113e/1140 converge at 1142..114a, followed by mode check 114c/1150. This recovers
the local machine control structure; exact original C++ spelling and runtime
equivalence are not claimed.

Info source improves 98.8% -> 98.9%, strict 78.3% -> 79.2%, original/current
instructions 1207/1198 -> 1207/1196. Instruction inventory 97.5% -> 97.8%,
opcode inventory 98.7% -> 98.9%, control inventory 98.3% -> 98.7%, ordered
control 89.1% -> 90.2%. Size score decreases 99.4% -> 99.1%; direct-call score
remains 100%. Every other focused row is unchanged. Fixed source means rise
99.160% / 98.44298% -> 99.165% / 98.45998%; strict means rise 87.135% /
81.30541% -> 87.180% / 81.45839%. Weighted 99% remains unmet.

Focused build: 204/204 ARM TUs, zero failures; 22 comparisons, zero skips,
six unique linked-exact and three unique byte-exact bodies. Native UCRT gof2
static-library build passed. Native runtime remains untested. Evidence:
_work/freelance-store-thirtyeighth.json/log/asm,
_work/native-thirtyeighth.log, _work/info-original-direct.asm,
_work/object-alpha-thirtyfourth.asm and
_work/before-freelance-store-thirtyeighth.cpp. Accepted-before reports were
preserved as _work/checkpoint38-before-focused.json/corpus.json.

Read-only coordinate checks also excluded unsupported changes: original
c8e1c..c8e4c and accepted-before 16fa..172a agree on single-precision half-row
multiply/add and signed conversion for the special index-26 image. Original
c904c..c906c and accepted-before 1926..1946 agree on first-legend-icon offsets,
one iPadHD load, width addition, conditional negative Y and X subtraction.
No coordinate changes were made based on the pseudocode alone.

Full corpus: 4619 comparisons, zero skips; 2083 linked-exact / 960 byte-exact.
Baseline and previous accepted checkpoint e2f1fb0c preservation gates passed
with zero lost pairs (exit 0 each). The default 99% gate failed as expected
(exit 1, StarMap >=99.0% target NOT reached). Overall source/fuzzy means: 89.06% / 76.16%.
Latest gate evidence: _work/thirtyeighth-gate.json,
thirtyeighth-previous-gate.json, thirtyeighth-target-gate.log.
Accepted StarMap.o SHA-256: 163fba948b78575f9bc6f412f6fda356951c0e3e6b8d798db88e9031180662e0.
Full JSON SHA-256: e2de9697df72f7768b131b42eac198cc97c8503f0d23349f89ceeb69b0bfdfa7.

## Follow-up after checkpoint 9bd67af9

StarMap::init now passes FogMode_1 instead of FogMode_dummy to FogEnable.
Primary Android c6c08 loads r2 = 1, c6c1e loads r1 = 0, and c6c20 calls
PaintCanvas::FogEnable(bool, FogMode). Accepted-before current code loaded
r2 = 0 at 4514; the new object loads r2 = 1 at the same address and calls at
452c. This is a proven call-argument correction, not an inferred fog formula.
The IDA excerpt omits the arguments; ARM machine instructions are authoritative.
The shared RenderEnums.h already defines FogMode_1 = 1 and was not edited.

Read-only PaintCanvas.cpp inspection shows that its current implementation
stores the second argument in fogEnableFlag and enters its GL-enable/disable
branch only for zero. No claim about correct complete native fog rendering or
runtime ABI follows from this local argument repair. Shared code was not changed.

Focused results: init source remains 98.3%, strict 77.0% -> 77.1%; instruction
inventory 97.8% -> 97.9%, all other init scores and instruction count unchanged.
Every other focused row is unchanged. Fixed source means remain 99.165% /
98.45998%; strict means become 87.185% / 81.46895%. Weighted 99% remains unmet.
ARM compilation passed 204/204 TUs, zero failures; 22 focused comparisons,
zero skips, six unique linked-exact and three unique byte-exact bodies.
Native UCRT gof2 static-library build passed; native runtime remains untested.
Evidence: _work/init-fog-thirtyninth.json/log/asm, native-thirtyninth.log,
init-original-direct.asm and before-init-fog-thirtyninth.cpp.
Previous accepted reports: _work/checkpoint39-before-focused.json/corpus.json.
New StarMap.o SHA-256:
3c172c7234c4ee9b0097f05a76bd5fd09e5bd15042b461d4102eb0f8c64720e7.

Full corpus: 4619 comparisons, zero skips, 2083 linked-exact / 960 byte-exact.
Baseline and previous checkpoint 9bd67af9 preservation gates both passed
with zero lost pairs (exit 0 each). Default 99% gate failed as expected
(exit 1, StarMap >=99.0% target NOT reached). Source/fuzzy corpus means
remain 89.06% / 76.16%. Gate evidence: _work/thirtyninth-gate.json,
thirtyninth-previous-gate.json and thirtyninth-target-gate.log.
Full JSON SHA-256: a97c5982f5b6aa1901449fa1e84dc4c4aa7602e8fbb9af2ae46d05633af1f442.

## Investigation after checkpoint 0db68768

Two narrow OnTouchEnd source forms were tested and removed; neither changes
the entire focused JSON or complete ARM object bytes.

- Close flag snapshot: move the suppressNextClose read before choiceVisible
  is cleared, using one uint8_t local. Native caece reads field 484 before
  caed4 clears field 169. Disassembly then showed that accepted code already
  does the same: 1ff4 reads field 484 and 1ff8 clears field 169. The source
  spelling had obscured an already-correct generated order. No extra read,
  call, register barrier or volatile operation was introduced. Package:
  _work/close-snapshot-fortieth.json/log; backup before-close-snapshot-fortieth.cpp.
- Choice result switch: express the existing result 1, result 0 and default
  paths with a switch. Native caec4/caec6 checks result 1 first, then
  caec8/caeca checks result 0; result 1 clears choiceVisible and
  alienJumpPending at cafb8/cafbc. Accepted code checks 0 at 1e9c/1e9e then
  uses conditional clears after the 1eA2 comparison. The switch preserves all
  actions but compiles to the identical accepted object. Package:
  _work/choice-switch-fortieth.json/log; backup before-choice-switch-fortieth.cpp.

A further read-only warning-105 control-flow check rules out adding a second
runtime dialog call to emulate a static-site count. Failed hasEquipment at
cb4e2 takes cb4e8 to cb7fe. That block obtains text 3217 at cb80c, calls
ChoiceWindow::set once at cb814, sets suppressNextClose at cb818 and branches
cb81c to cb890. It then sets choiceVisible and enters inertia at cb896.
The mission-139/142 checks at cb4ec..cb5d2 are bypassed on this path.
The retained warningText chain likewise skips later warning checks once 3217
is selected and enters common visibility/inertia processing after one set.
This proves this path's static CFG correspondence, not runtime execution or
complete warning-handler equivalence. No new production edit follows from it.
Evidence: _work/end-original-direct.asm and touch-android.c.

Both trial object hashes and both fresh restored-source object hashes match
3c172c7234c4ee9b0097f05a76bd5fd09e5bd15042b461d4102eb0f8c64720e7.
Fresh restored packages: _work/fortieth-restored-focused.json/log and
fortieth-switch-restored-focused.json/log. Each passed 204/204 ARM TUs,
zero failures and 22 comparisons with zero skips. OnTouchEnd remains
97.5% source / 76.4% strict, 1037/1003 instructions. Production source is
unchanged; no new native-library or full-corpus result is claimed. Those
remain the accepted 0db68768 validations. Means stay 99.165% / 98.45998%
source and 87.185% / 81.46895% strict; weighted 99% remains unmet.
These negative results do not prove a compiler ceiling or runtime equivalence.

## Follow-up after checkpoint 5176204a

The station-projection phase of update now assigns MatrixGetPosition to
this->scratchVector2 and passes that member to GetScreenPosition, replacing
its previously used local tmp. The local tmp remains for transition easing.

Primary Android c9c4c creates this+132 (0x84) in r6. MatrixGetPosition at
c9c8a produces a temporary, Vector::operator= at c9c92 writes to r6, and
c9ca4/c9ca6 passes the same member to GetScreenPosition. Accepted-before
code instead assigns a temporary to sp+48 at 32a8..32ac and passes that
stack address at 32be/32c0. New object creates this+132 at 3268, assigns
through it at 32aa..32ae and passes it at 32c0/32c2. This repairs the native
member write and argument identity; no runtime execution or complete ABI
equivalence is claimed. The generated member address proves offset 0x84;
existing layout assertions check the surrounding scratchVector (0x78) and
stationGeoms (0x90) offsets and total size. Shared headers and verifier were
not edited.

Focused update source remains 98.5%; instruction inventory improves
96.7% -> 96.8%, while strict decreases 82.1% -> 81.5%. Instructions remain
971/966. Every other focused row is unchanged. The correction is retained
for its proven member write even though it is not a source-score gain and
strict scheduling changes unfavorably. Fixed source means remain 99.165% /
98.45998%; strict means become 87.155% / 81.38690%. Weighted 99% remains unmet.

204/204 ARM TUs compiled, zero failures; 22 focused comparisons, zero skips,
six unique linked-exact and three unique byte-exact bodies. Native UCRT gof2
static-library build passed. Native runtime remains untested. Evidence:
_work/station-scratch-fortyfirst.json/log/asm, native-fortyfirst.log,
update-original-direct.asm, init-fog-thirtyninth.asm and
before-station-scratch-fortyfirst.cpp. Previous accepted reports are
_work/checkpoint41-before-focused.json/corpus.json. New StarMap.o SHA-256:
c191e84a4ae6496aaa22d842ee8d425e9b5bffef0aaf22a52a360a1cefe91a0f.

Full corpus: 4619 comparisons, zero skips; 2083 linked-exact / 960 byte-exact.
Baseline and previous accepted checkpoint 5176204a preservation gates passed
with zero lost pairs (exit 0 each). Default 99% gate failed as expected
(exit 1, StarMap >=99.0% target NOT reached). Corpus source/fuzzy means: 89.06% / 76.16%.
Gate evidence: _work/fortyfirst-gate.json, fortyfirst-previous-gate.json,
fortyfirst-target-gate.log. Full JSON SHA-256: b713428bfecf82526cc126615015d4d028638d0d1117e8653219ebe5ed9d53ce.

## Follow-up after checkpoint 274a33ed

The system-projection loop in update now captures PaintCanvas* before
AEGeometry::getPosition, once per iteration, and uses that saved receiver
for GetScreenPosition. It still reloads the destination systemPositions
member after getPosition and after GetScreenPosition. The station-projection
phase and its newly repaired scratchVector2 write remain unchanged.

Primary Android c9be4 captures Canvas in r5 before getPosition c9bf0 and
passes r5 at c9c00/c9c02. Accepted-before 3208 called getPosition before
3212 loaded Canvas. New object 3204 captures Canvas in r5 before getPosition
3210 and passes it at 3220/3222. No calls, loops, volatile barriers, forced
registers or shared-header/compiler changes were added. This repairs native
call-observable pointer-read order; runtime execution remains untested.

Update source improves 98.5% -> 98.7%, strict 81.5% -> 82.7%, instruction
inventory 96.8% -> 97.0%, control inventory 98.0% -> 98.7% and ordered
control 94.7% -> 95.4%. Instructions remain 971/966; every other focused
row is unchanged. Fixed source means improve 99.165% / 98.45998% ->
99.175% / 98.48733%; strict means improve 87.155% / 81.38690% ->
87.215% / 81.55099%. Weighted 99% remains unmet.

ARM focused build passed 204/204 TUs, zero failures; 22 comparisons, zero
skips, six unique linked-exact and three unique byte-exact bodies. Native
UCRT gof2 static-library build passed; no native runtime/ABI equivalence is
claimed. Evidence: _work/system-canvas-fortysecond.json/log/asm,
native-fortysecond.log, update-original-direct.asm,
station-scratch-fortyfirst.asm and before-system-canvas-fortysecond.cpp.
Previous inputs: _work/checkpoint42-before-focused.json/corpus.json.
New StarMap.o SHA-256:
1384d74b71ee1da0f7da2daad528e398ce992ee3798e14ba1a8f2aae2503cc80.

Full corpus completed after the interrupted observation: 4619 comparisons,
zero skips; all 22 StarMap rows agree with the focused report for both
scores, instruction counts and exact flags. Baseline and checkpoint 274a33ed
gates passed with zero lost pairs (exit 0 each), preserving 2083 linked-exact
and 960 byte-exact pairs. Default 99% gate failed as expected (exit 1,
StarMap >=99.0% target NOT reached). Corpus source/fuzzy means: 89.06% / 76.16%.
Gate evidence: _work/fortysecond-gate.json, fortysecond-previous-gate.json,
fortysecond-target-gate.log. Full JSON SHA-256: 327cf9df7ccfa5a7b7785b17dbe7acf73519fc18f84b2dd6a23713a642f0a8dc.

## Investigation after checkpoint 2ac358b9

Read-only signature review retained int StarMap::init(bool, Mission *, bool,
int). The Android normal epilogue at c7416..c7426 subtracts the stack guard
values in R0; every successful exit therefore leaves R0 = 0. Absence of a
separate MOV zero does not prove void. Constructor c6b24 calls init and c6b28
immediately replaces R0 while checking its own guard, so this caller does not
use the init result. Ordinary C++ mangling does not establish this return type;
the IDA int declaration is also not proof of the original source signature.
Changing to void solely to remove a generated MOV zero would be unjustified.
No signature or production change was made for this finding.

Inspected StarMap.h/.cpp, src creation sites (MGame.cpp, MissionsWindow.cpp and
WantedWindow.cpp), _work/ctor-original-direct.asm and
_work/init-original-direct.asm. Station vector transform/normalization and
Canvas capture before getRace were also checked read-only: c77f8..c7802,
c785c..c786e and c7aba..c7ace correspond to accepted 2bda..2be4,
2c3e..2c50 and 2ea4..2eb8. No additional correction was warranted there.

A focused source trial replaced the initial drawOnScreenInfo pointer ternary
with an explicit if/else. Android c8378..c837e conditionally loads the two
pointer fields; accepted c66..c70 selects an offset and then loads one pointer.
The alternative was a source-shape hypothesis, not recovered C++ syntax.
_work/position-select-fortythird.json is identical to the entire accepted
focused report; 204/204 TUs compiled, zero failures, 22 comparisons, zero skips.
Trial StarMap.o SHA-256 is also identical to 2ac358b9:
1384d74b71ee1da0f7da2daad528e398ce992ee3798e14ba1a8f2aae2503cc80.
The trial was removed. Evidence: _work/position-select-fortythird.log,
_work/info-original-direct.asm, _work/system-canvas-fortysecond.asm and
_work/before-position-select-fortythird.cpp.

Fresh restored-source focused verification also compiled 204/204 TUs, zero
failures, compared 22 rows with zero skips and reproduced the entire accepted
JSON and object hash. Package: _work/fortythird-restored-focused.json/log.
Production source is unchanged. Source means remain 99.175% / 98.4873257287706%;
strict means remain 87.215% / 81.55099281791297%. Full-corpus/native validations
below remain those of 2ac358b9; no new full/native run is claimed. The weighted
99% target remains unmet and native executable/runtime validation is untested.

## Follow-up after checkpoint 5256c4fa

Only draw's current-route alpha clamp changes. Android c945c..c946a converts
the computed route alpha into R2, compares the StarMap alpha in R1 against it,
and uses IT LT / MOVLT R2,R1 before UXTB. Accepted pre-trial 55c..568 used
IT GT / MOVGT R1,R2 and UXTB on R1. The new in-place source clamp,
if (this->alpha < routeAlpha) routeAlpha = this->alpha, emits CMP / IT LT /
MOVLT / UXTB at 560..566 matching the primary branch and destination selection.
This is a source-form hypothesis grounded in machine behavior, not recovered
original C++ syntax. Integer values, field reads and runtime calls are unchanged;
no barriers, flags, extra calls or padding were introduced.

Draw source-shape improves 99.0% -> 99.1%, strict 89.1% -> 89.4%; counts remain
916/907. Instruction inventory improves 98.6% -> 98.9% and opcode inventory
98.9% -> 99.1%; control and call-count metrics are unchanged. Every other one
of the 22 focused rows is unchanged. Fixed unique-body source means become
99.180% / 98.50022532037741%, strict 87.230% / 81.58969159273342%.
The weighted shortfall remains 0.49977467962259 percentage points.

Before this trial, a fresh read-only inventory was generated from the accepted
2ac358b9 object and reproduced all 22 reported source metrics. The diagnostic
snapshot docs/validation/starmap_2026-10-02_remaining_inventory.json is explicitly
labelled as pre-trial evidence, with object SHA-256. It preserves all rows and
unmodified normalization and does not replace or filter the gate. Read-only
init checks found matching callback ordering for the TouchButton arguments
(c7102..c7128 vs 4978..499e) and path-finder/system pointer capture before
Status/getIndex (c723c..c7252 vs 4bc0..4bd6); no production change follows
from those checks. The station-mode spin clamp in update also already matches
ca56c..ca5da vs 3b80..3be8, including MI selection of the 10.0f cap.

Primary evidence: _work/draw-original-direct.asm; current disassembly:
_work/route-alpha-fortyfourth.asm; focused report/log:
_work/route-alpha-fortyfourth.json/log; backup before-route-alpha-fortyfourth.cpp.
Prior focused/full inputs are checkpoint44-before-focused.json and
checkpoint44-before-corpus.json. Trial/accepted object SHA-256:
b074aa464b492d3eba51e14b69f2f85b2bed927ebdd56b0a34cb9f9460b8e7ec.

Focused ARM build compiles all 204 TUs with zero failures, comparing 22
symbols with zero skips. Native UCRT Debug gof2 static-library build passes;
no executable/runtime test is claimed. Full comparison has 4619 entries,
zero skips, source 89.06% and strict fuzzy 76.16%, with unchanged 4274/4524
coverage, 250 missing (7 wrong-type / 243 absent), and 129 extra symbols.
All 22 StarMap full/focused rows agree for source/strict scores, counts and
linked/byte exact flags.

Baseline and previous accepted 5256c4fa preservation gates both pass, with
no lost exact (unit, symbol) pairs; 2083 linked and 960 byte exact pairs remain.
Default 99% gate exits 1 only with the expected weighted-target failure.
Evidence: _work/native-fortyfourth.log, fortyfourth-corpus.json/log,
fortyfourth-gate.json, fortyfourth-previous-gate.json and
fortyfourth-target-gate.log. Full JSON SHA-256:
0fd05a776b12126fd16340c3d7aa99afb8aecb0372e4164917686656ebcd1e11.

## Investigation after checkpoint 1d9a4c65

A first-route depth source trial materialized bool bothBehind, initially false,
assigned only when scratchVector.z < 0.0f from scratchVector2.z < 0.0f, then
used !bothBehind to draw. It preserved short-circuit evaluation and the existing
unordered predicate behavior. Primary Android c93c8..c93dc materializes a
zero/one result with IT MI, whereas accepted 3c2..3d0 branches directly on the
second comparison. The trial was a source-form hypothesis, not recovered syntax.

The entire focused trial report _work/depth-bool-fortyfifth.json is identical
to the accepted report, and the complete StarMap.o SHA-256 also remains
b074aa464b492d3eba51e14b69f2f85b2bed927ebdd56b0a34cb9f9460b8e7ec.
All 204 ARM TUs compiled with zero failures; 22 rows compared with zero skips.
The trial was removed. Backup/evidence: before-depth-bool-fortyfifth.cpp,
depth-bool-fortyfifth.log, draw-original-direct.asm and
route-alpha-fortyfourth.asm in _work. No extra checks or synthetic materialization
were inserted to force the compiler's instruction count.

Read-only review also confirms update's matrix order already matches: original
c9c62..c9c92 obtains the root matrix first, then the station matrix, multiplies,
extracts position and assigns scratchVector2. Accepted 3282..32b2 has the same
calls and member destination. OnTouchEnd's galaxy selection guard already reads
selectedSystem, autoMode, pad_0xa8_a and lastSelectedSystem in the original
cb254..cb26e order at accepted 21c0..21de; no condition reorder is justified
by this inspection.

Fresh restored-source verification _work/fortyfifth-restored-focused.json/log
again compiled 204/204 TUs with zero failures, compared 22 rows with zero skips
and reproduced the entire accepted report and object hash. Production code and
canonical gate inputs remain those of 1d9a4c65. Means remain source
99.180% / 98.50022532037741%, strict 87.230% / 81.58969159273342%.
No new native/full-corpus run is claimed; the accepted fortyfourth validations
below remain authoritative. Weighted 99% is unmet and native runtime untested.

## Investigation after checkpoint 27cf2441

OnTouchEnd's primary warning paths converge at cb7fe..cb818, then continue at
cb890. A source-form trial replaced the negative warningText sentinel and its
guards with explicit jumps to one common warning block. Each warning assignment
jumped past later warning predicates and the ordinary confirmation dialog; normal
dialog paths jumped past the warning block to the common choice-visible tail.
It retained one getText/set call site and the existing inertia/Help continuation.
This was a control-flow hypothesis, not recovered C++ syntax. It was distinct
from the previously rejected separate-3217 call-site trial.

All 204 ARM TUs compiled with zero failures. The entire 22-row, zero-skip focused
report warning-goto-fortysixth.json equals the accepted report, including every
reported metric, count and exact flag. However, whole-object bytes differ:
accepted SHA-256 b074aa464b492d3eba51e14b69f2f85b2bed927ebdd56b0a34cb9f9460b8e7ec;
trial SHA-256 e8aaa6fee52010224a707c505b5fe5bcb7bbb0d77f36cb780d2f72eefd0ff8c9.
ELF section payload comparison finds only .text (four changed bytes) and .rel.text
(six changed bytes), with unchanged section sizes. Relocations at text offsets
0x2960/0x2964 exchange shipClasses (type 3) and Globals::gameText (type 96).
The corresponding pool words and two references change. Thus identical focused
metrics do not mean byte-identical objects; no new full-corpus exact preservation
claim is made for this rejected trial.

The trial was removed because it added jumps without metric progress. Fresh
restored-source verification fortysixth-restored-focused.json/log compiled all
204 TUs, failed zero and compared 22 rows with zero skips. Its entire JSON and
whole-object SHA-256 equal the accepted fortyfourth checkpoint. Production code
and canonical gate inputs remain unchanged. Source means remain 99.180% /
98.50022532037741%, strict means 87.230% / 81.58969159273342%. Weighted 99% remains
unmet. Accepted fortyfourth native/full-corpus validations remain authoritative;
no new native/full-corpus run or native runtime test is claimed.

Evidence in _work: before-warning-goto-fortysixth.cpp,
warning-goto-fortysixth.json/log/o, fortysixth-restored-focused.json/log and
fortysixth-object-delta.json. Compact section-difference evidence is committed at
docs/validation/starmap_2026-10-05_warning_goto_trial.json.

## Investigation after checkpoint 78765e07

A fresh read-only allocation-shape alignment of the accepted initStarSystem
object reproduces source 97.3%, instruction inventory 96.7%, opcode inventory
97.4%, control 96.9%, ordered control 94.8%, call count 99.0% and size 96.3%.
The diagnostic does not replace or filter the fixed-corpus gate. Full local
alignment groups are retained in _work/station-local-differences-fortyseventh.json.

Raw primary Android calls were compared against current ARM relocations inside
verified function bounds, excluding initLights. Original has 100 direct and eight
indirect calls; accepted has 101 direct and eight indirect calls. After accounting
for the existing Array<bool>/Array<uint8_t> constructor and ArraySetLength
spellings, the only extra direct call is the shared Matrix C1. No missing lighting,
ring, station or vector-normalization call is exposed by this count comparison.
This is a static-site inventory, not a claim of path-by-path runtime equivalence.
Committed evidence: docs/validation/starmap_2026-10-05_station_calls.json.

Primary c77c6..c77e0 and accepted 2ba8..2bc2 already add the random result to the
previous distance, then add 1600, before converting/storing. Primary c7c1a..c7c32
and accepted 3004..301c already capture the planet member after the second
campaign getter and select the <=157 / >157 constants before one scaling call.
The successful angle-slot retry and flag write also agree at original
c774c..c7778 versus accepted 2b2e..2b5c. These are not candidates for another
arithmetic or branch-order rewrite.

Sequence alignment alone misleadingly presents a second planetGeom reset:
raw accepted 2f52 writes member444 once, then 2f56 writes member376; original
c7b68 writes member376 and c7b6c writes member444. Both reset once before the
texture-index getter. Their reordered pure stores do not establish an extra
runtime operation. No code change is justified by this apparent alignment gap.

The Matrix initialization region remains genuinely different (inline primary
NEON initialization versus the shared constructor and two required field fixes),
while ring-check block placement and literal-pool decoding also affect alignment.
No raw-storage, shared-constructor, verifier or compile-flag workaround was added.
Production and canonical reports are unchanged; accepted fortyfourth validations
remain authoritative. No new build/runtime result is claimed. Means stay
99.180% / 98.50022532037741%; weighted 99% remains unmet. The next investigation
returns to init/OnTouchEnd rather than repeating the now-confirmed station
arithmetic and scale branches.

## Investigation after checkpoint 67bf0b1b

Fresh allocation-shape alignment of init reproduces source 98.3%, instruction
inventory/opcode inventory 97.9%, control 98.8%, ordered control 80.1%, call count
99.0% and size 97.7%. The low ordered-control score includes displaced source
blocks and does not by itself prove missing runtime branches. Diagnostic groups
are saved in _work/init-local-differences-fortyeighth.json without altering any
canonical gate input or corpus weight.

Two new source-form hypotheses were compiled separately and rejected:

- Initial camera focus: primary c6eba/c6ee8 bypass the target-station branch to
  common camera setup at c6f8e. The trial gave auto/current-system position paths
  explicit jumps to camera_focus_ready and left the target-station branch as a
  separate fall-through scope. Target getters, ownership test/deletion and the
  common CameraGetCurrent/GetLocal sequence were retained. The entire focused
  report and whole object were identical to accepted source.
- Mission target search: primary c7176..c725a contains the principal scan/path
  branch before the fallback scan at c725c..c72dc. The trial used the unchanged
  initial short-circuit predicate to jump to search_without_routes, placed the
  principal branch first, then jumped to mission_search_ready. Both scans kept
  repeated mission/system/station getters, index updates and the same common
  pathAnim/momentum/selectedSystem tail. Again the entire focused report and
  whole object were identical to accepted source.

These trials hypothesized source layout, not recovered C++ syntax. Neither
forced registers, inserted a call/check, changed flags or used a barrier. Both
were removed: they added labels without improving source shape or generated
bytes. Backups/logs/reports are before-init-focus-fortyeighth.cpp,
init-focus-fortyeighth.json/log, before-init-search-fortyeighth.cpp and
init-search-fortyeighth.json/log in _work.

Each trial and each fresh-source restoration compiled all 204 ARM TUs with zero
failures and compared 22 rows with zero skips. Both trial object hashes and the
final restored object hash are
b074aa464b492d3eba51e14b69f2f85b2bed927ebdd56b0a34cb9f9460b8e7ec.
Restoration reports are fortyeighth-restored-focused.json/log and
fortyeighth-search-restored-focused.json/log. Compact committed evidence is
in docs/validation/starmap_2026-10-05_init_layout_trials.json.
Production code and canonical reports remain those of 1d9a4c65; accepted
fortyfourth native/full-corpus validations remain authoritative. No new
native/full-corpus run is claimed. Source means remain 99.180% /
98.50022532037741%, weighted 99% remains unmet and native runtime untested.
The next search must examine a different discrepancy; source block placement
alone did not change this compiler output in either new trial.

## Accepted pitch-bound restoration after checkpoint eeca8928

OnTouchMove now snapshots the two ordered pitch predicates, selects pitchBound
from the lower predicate, selects the candidate result, then uses that same
bound when the upper predicate is true. Primary cac56/cac74..caca2 initializes
and selects an intermediate bound before selecting the final pitch. The trial
uses each source predicate once rather than inserting the original's four
compiled comparisons or forcing redundant instructions. This is a source-form
hypothesis supported by the intermediate-value data flow, not an exact recovery
of C++ syntax.

The transformation preserves the pitch result: below/equal -8192 and unordered
NaN select -8192; values inside the interval, including signed zero, are retained;
equal +8192 retains +8192; above +8192, including +infinity, selects +8192.
The existing yaw computation, drag velocities, member writes, callback/camera
calls and return remain unchanged. No register forcing, barrier, padding,
compiler flag or shared code change was used.

The first focused trial was temporarily restored for isolation before its new
metrics were inspected, then reapplied and independently confirmed. Trial and
confirmation have identical entire JSON and object bytes; the intermediate fresh
restore reproduced accepted fortyfourth source/JSON/object. No rejected variant
was left in production. Evidence: _work/before-pitch-bound-fortyninth.cpp,
pitch-bound-fortyninth.json/log, fortyninth-restored-focused.json/log,
pitch-bound-fortyninth-confirmed.json/log and pitch-bound-fortyninth.asm.

Only OnTouchMove's row changes: source 98.4% -> 98.8%, strict 79.2% -> 78.8%,
current instructions 255 -> 258 (original 265), inventory 97.7% -> 98.3%, opcode
98.1% -> 98.6%, size 96.2% -> 97.3%. Control/call metrics remain 100%; the other
21 focused rows are identical. The new 4ef8/4f00 comparisons feed three bound/result
moves at 4f06, 4f10 and 4f42. The compiler still uses two comparisons; no four-
comparison claim is made.

Fixed means become source 99.200% / 98.51515279538093%, strict 87.210% /
81.5747641177299%. The weighted shortfall is 0.48484720461907 percentage points.
All 204 ARM TUs compiled with zero failures, focused has 22 rows with zero skips,
and every focused/full StarMap metric/count/exact flag was checked for agreement.
Confirmed object SHA-256:
91b513f97dbc427e3a32a977dd2417dfa7d22c8edc85585f7b1401032476f86d.
Native UCRT gof2 static-library build passed (native-fortyninth.log).
Full corpus has 4619 comparisons, zero skips, 2083 linked-exact and 960 byte-exact;
coverage remains 4274/4524, missing 250 (7 wrong-type / 243 absent), extra 129.
Baseline and previous accepted fortyfourth preservation gates both exit 0 with
zero exact-pair losses. Default 99% gate exits 1 with the expected threshold
failure; native runtime remains untested. Full evidence: fortyninth-corpus.json/log,
fortyninth-gate.json, fortyninth-previous-gate.json and fortyninth-target-gate.log.
Full JSON SHA-256: b7bfd34e1730f566cc064c66b3978520bffcea2773c8c07ce20aaeb6b5145805.

## Investigation after checkpoint e43de497

The accepted OnTouchMove bound-selection form was evaluated separately in update,
where primary ca60a..ca678 has its own intermediate pitch-bound selection and
four compiled comparisons. The first trial used two snapshot predicates and
three bound/result selections, keeping the existing yaw assignment before the
pitch clamp. The second put yaw in a local after the lower-pitch result and wrote
yaw/pitch together afterward, corresponding to the primary ca688/ca690 member
writes and the decompiled intermediate yaw value. Both retain the negative-bound
NaN result, signed-zero behavior, existing calls and member destinations.
Neither inserts repeated source checks to force four compiled comparisons.

Unlike OnTouchMove, neither trial improves update: source stays 98.7%, strict
82.7% -> 81.6%, current instructions 966 -> 969 (original 971), instruction
inventory 97.0% -> 96.9%, opcode stays 98.2%, size 99.6% -> 99.9%. Control,
ordered control and calls stay 98.7%, 95.4% and 100%. The other 21 focused rows
are unchanged. The two trial reports and whole objects are identical to each
other, with SHA-256
9500ff5f2b38aab0e5074eb99f6577aacc9fb8266650fe445ac072f854e2bc4c.
The source hypotheses were removed rather than retaining extra variables with
no source-shape gain and weaker strict/inventory evidence.

Both trials and the fresh restoration compile all 204 ARM TUs with zero failures
and compare 22 rows with zero skips. Restored focused JSON equals the entire
accepted forty-ninth report, and restored object SHA-256 equals
91b513f97dbc427e3a32a977dd2417dfa7d22c8edc85585f7b1401032476f86d.
Production and canonical reports remain checkpoint e43de497. Means remain
99.200% / 98.51515279538093% source and 87.210% / 81.5747641177299% strict;
weighted 99% is unmet. Accepted forty-ninth native/full validations remain
authoritative; no new native/full/runtime run is claimed for these rejected trials.

Evidence: _work/before-update-bound-fiftieth.cpp, update-bound-fiftieth.cpp,
update-bound-fiftieth.json/log/asm, update-bound-yaw-fiftieth.json/log and
fiftieth-restored-focused.json/log. Compact committed snapshot:
docs/validation/starmap_2026-10-05_update_pitch_trials.json.

## Accepted drawKey captures after checkpoint 28456390

drawKey now explicitly snapshots Canvas and font anew before each GameText getter,
using the primary per-row order: Canvas/font for text 274, font/Canvas for 401,
Canvas/font for 547, font/Canvas for 556 and Canvas/font for 555. No snapshot is
reused across a drawing callback; the two local variables are refreshed for every
row. Primary Android drawKey at c80e8..c8354 and its IDA body show these receiver/
font captures. Existing image coordinates, fresh Layout line-gap reads, five
image/text call pairs and temporary String lifetime remain unchanged. This is a
source-form hypothesis supported by the observed capture order, not recovered
C++ syntax. It does not cache global values across rows or add a drawing call.

Only drawKey changes: source 97.8% -> 97.9%, strict 34.8% -> 35.1%, current
instructions 243 -> 244 (original 246), inventory 93.8% -> 94.0%, opcode
95.9% -> 96.1%, size 99.2% -> 99.6%. All control/call metrics remain 100%; the
other 21 focused rows are identical. Fixed means become source 99.205% /
98.51861709618363%, strict 87.225% / 81.585157020138%. The weighted shortfall
remains 0.48138290381637 percentage points. No flags, shared code, barriers,
register forcing, padding or verifier settings were changed.

Focused build compiled all 204 ARM TUs with zero failures and compared 22 rows
with zero skips. Every focused/full StarMap field except unit-path spelling was
checked for exact agreement. UCRT native gof2 static-library build passed.
Full corpus: 4619 comparisons, zero skips, 2083 linked-exact and 960 byte-exact;
coverage 4274/4524, missing 250 (7 wrong-type / 243 absent), extra 129, unchanged.
Baseline and previous accepted e43de497/28456390 preservation gates exit 0 with
zero exact-pair losses; default 99% gate exits 1 with the expected threshold failure.
Native runtime remains untested. The weighted goal remains unmet.

Evidence in _work: before-key-text-captures-fiftyfirst.cpp,
key-text-captures-fiftyfirst.json/log/asm, drawkey-android.c,
key original-address disassembly, native-fiftyfirst.log, fiftyfirst-corpus.json/log,
fiftyfirst-gate.json, fiftyfirst-previous-gate.json and fiftyfirst-target-gate.log.
Object SHA-256: 3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Full JSON SHA-256: 139bf7836f3b47c901eca38b8f33d6fe3599298e2988114372eb8daa3b0176d0.

## Investigation after checkpoint 9a96e2e5

Two separate drawOnScreenInfo source hypotheses were tested against the primary
Android IDA body and _work/info-original-direct.asm:

- Explicitly refresh the existing textCanvas/textFont locals before all six
  DrawString sites (selected/unselected station names, station tech text, system
  name, selected-system race and security). Primary v77/v78, v79/v117, v89/v90,
  v82/v83, v93/v95 and v99/v100 capture receiver/font before their getters or
  temporary-string construction. The trial did not reuse values across rows,
  change temporary lifetimes or add calls. Unlike the accepted drawKey change,
  it produced identical entire focused JSON and whole object bytes here.
- Express the four-way race-image choice as switch with cases 2/1/0/default,
  retaining the same member-address selections. This was distinct from the
  previous race 0/default branch swap. It also produced identical entire focused
  JSON and whole object bytes; no branch-order recovery is claimed from it.

Both hypotheses were removed because neither improved generated source shape.
Each trial and each fresh-source restoration compiled 204/204 ARM TUs with zero
failures and compared 22 rows with zero skips. Every report equals the complete
accepted fifty-first report. Both trial object hashes and the final restored
object hash are
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
The unchanged info row remains 98.9% source, 79.2% strict and 1207/1196 instructions.
Production and canonical gate inputs remain 9a96e2e5; source means 99.205% /
98.51861709618363%, strict 87.225% / 81.585157020138%. Weighted 99% remains unmet.
Accepted fifty-first native/full-corpus validations remain authoritative; no new
native/full/runtime run is claimed for these rejected source-only trials.

Evidence in _work: before-info-text-captures-fiftysecond.cpp,
info-text-captures-fiftysecond.json/log, fiftysecond-restored-focused.json/log,
before-info-race-switch-fiftysecond.cpp, info-race-switch-fiftysecond.json/log
and fiftysecond-switch-restored-focused.json/log. Compact committed evidence:
docs/validation/starmap_2026-10-05_info_capture_race_trials.json.

## Investigation after checkpoint 9692dc9b

OnTouchEnd's internal no-warning sentinel was tested as zero instead of -1.
The two warningText<0 guards became ==0 and the final >=0 test became !=0.
All seven assigned text IDs are positive (532,3213..3218), so the same paths
skip later warning predicates, select the same getter/set calls and reach the
same suppression/choice-visible/inertia/Help tail. Zero was a source-form
hypothesis, not a sentinel value recovered from the primary Android binary.
No source padding, extra check/call or compiler setting was introduced.

The trial's entire 22-row focused report and whole object equal accepted source.
It was removed because it gives no source-shape progress. Trial and fresh restore
each compile 204/204 ARM TUs, zero failures, 22 comparisons and zero skips.
Restored entire JSON again equals accepted; both object SHA-256 values are
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Production and canonical inputs remain 9a96e2e5. Source means stay 99.205% /
98.51861709618363%; weighted 99% remains unmet. Accepted fifty-first native/full
validations remain authoritative; no new native/full/runtime result is claimed.
Evidence: _work/before-warning-zero-fiftythird.cpp, warning-zero-fiftythird.json/log
and fiftythird-restored-focused.json/log; committed compact snapshot:
docs/validation/starmap_2026-10-05_warning_zero_trial.json.


## Investigation after checkpoint be2ce265

Three draw header rows were tested with fresh explicit Canvas/font snapshots:
Canvas then font for the system name, font then Canvas for race and security.
Primary Android IDA v53/v54, v55/v56 and v61/v62 support this capture-order
hypothesis; exact C++ syntax is not recovered. Getters, temporary lifetimes,
coordinates, colors and footer remain unchanged. No capture spans multiple
rows and no drawing call is added.

Only draw strict match changes: 89.4% -> 88.8%; source stays 99.1%, all other
metrics and all other 21 rows stay unchanged. The trial was rejected. Trial
object SHA-256: 2e6966cc4aadd1fef4fa8e8a11846552134cc21fb087d430adccbdcce8440de3.
Trial and fresh restore each compile 204/204 ARM TUs with zero failures and
compare 22 symbols with zero skips. Restored entire JSON equals accepted;
whole restored object SHA-256:
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Production/canonical inputs remain 9a96e2e5. Source means remain 99.205% /
98.51861709618363%; weighted 99% remains unmet. Accepted fifty-first native/full
validations remain authoritative; no new native/full/runtime run is claimed.
Evidence: _work/before-draw-header-captures-fiftyfourth.cpp,
draw-header-captures-fiftyfourth.cpp/json/log, fiftyfourth-restored-focused.json/log;
compact committed snapshot: docs/validation/starmap_2026-10-05_draw_header_trial.json.


## Investigation after checkpoint c5ad343b

Read-only station review confirmed the existing Array<uint8_t> constructor
pair is linked-exact in the StarMap translation unit. Changing to the primary
Array<bool> spelling would lose that pair, as the earlier trial documented.
No alias, duplicate instantiation or shared Matrix workaround was introduced.
Station statements still match the reviewed primary call/capture ordering;
this review does not claim all instruction differences are compiler artifacts.

A drawKey coordinate hypothesis flattened textX into
imageWidth + lineHeight + x + lineHeight, matching the printed IDA sum.
Only drawKey changes: source 97.9% -> 97.8%, strict 35.1% -> 43.0%, instructions
246/244 -> 246/242, inventory 94.0% -> 93.8%, opcode 96.1% -> 96.7%, size
99.6% -> 98.3%. All control/call metrics remain 100%; other 21 rows identical.
It was rejected because the requested source metric decreases.

A complete bounded primary disassembly now covers c80e8..c8354 in
_work/drawkey-original-full-fiftyfifth.asm (the older addresses extract is partial).
It clarifies the arithmetic: c817e creates imageX=x+lineHeight, c8182 creates
imageWidth+lineHeight, c8186 adds the two. The accepted grouping already agrees;
the IDA flattened sum does not prove the original source parenthesization.
No literal C++ source recovery is claimed.

Trial and fresh restoration each compile 204/204 ARM TUs, zero failures, and
compare 22 symbols, zero skips. Restored entire JSON equals accepted fifty-first.
Trial object SHA-256: 880d684cfe5c169afd29c8b60824c749127097c064cde51cb798b80b9182a837.
Restored object SHA-256:
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Production/canonical inputs remain 9a96e2e5, source means 99.205% /
98.51861709618363%. Weighted 99% remains unmet. Accepted fifty-first native/full
validations remain authoritative; no new native/full/runtime result is claimed.
Evidence: _work/before-key-sum-fiftyfifth.cpp, key-sum-fiftyfifth.json/log,
fiftyfifth-restored-focused.json/log and complete primary key disassembly;
committed compact snapshot: docs/validation/starmap_2026-10-05_key_sum_trial.json.


## Read-only investigation after checkpoint f28e5433

Current tracked production state is clean. Fresh disassembly of the current
and delinked target objects, through unchanged asmdiff functions, reproduces
every source submetric for init, OnTouchEnd, initStarSystem, update and drawKey.
Allocation-normalized multiset surplus counts (target/current) are respectively
24/7, 53/18, 36/10, 31/27 and 15/14. These are diagnostic token differences,
not a count of missing source statements or behavior. The current object hash
is 3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.

Three init hypotheses were eliminated by machine evidence before editing:

- Path-finder capture: primary c7238..c7256 and accepted 4bbc..4bda both read
  Status, capture pathFinder/systems together, call getSystem/getIndex, then
  freshly read targetSystem for the path call. No receiver reload is missing.
- Camera coordinates: primary c6fce..c6fec uses two VDIV.F32 by 20.0. Existing
  /20.0f agrees; replacing division with multiplication is not justified.
- Background geometry: accepted 4c8a..4d20 already directly passes each fresh
  allocation/constructor pointer into the first setRotation, while reloading
  the member for setPosition after that callback, as primary v71/v72/v73 show.

The init surplus includes the previously documented Matrix inline construction,
conditional null/deletion and epilogue differences, plus decoded pool tokens.
No Matrix workaround, padding, duplicate call or verifier change was made.
This inventory does not establish a compiler ceiling or attribute every surplus
token to an artifact. The five-body comparison is not a new full-corpus gate.
No new build/native/full/runtime result is claimed; accepted fifty-first and
fresh fifty-fifth restoration evidence remain authoritative. Production and
canonical inputs remain 9a96e2e5, source means 99.205% / 98.51861709618363%.
Weighted 99% remains unmet. Diagnostic: _work/remaining-inventories-fiftysixth.json;
committed snapshot: docs/validation/starmap_2026-10-05_remaining_inventories.json.


## Investigation after checkpoint 32e39bbe

Two further source hypotheses were compiled separately and rejected:

- OnTouchEnd combined primary Android's separate 3217 getter/set/suppression
  branch with a common station_warning_selected jump for the other warning IDs.
  All existing short-circuit predicates/getter sequences remain; warning jumps
  skip later predicates, ordinary dialogs skip the warning block, and every path
  still reaches choiceVisible, inertia and Help. The warningText read can only
  follow an assignment. This combination was distinct from the earlier separate
  3217 trial with sentinel guards and from the common-goto trial without a separate
  3217 site. It does not add a call to a runtime path, duplicate a check or force
  register allocation. Primary has this separate static delivery site; original
  C++ syntax is not recovered.
- update computed pitchDistance before yawDistance, following primary v63/v64.
  Expressions, oldPitch/oldYaw/selectedStation snapshots, conditional selections,
  stores, thresholds and calls remain unchanged. No extra computation was added.

Only OnTouchEnd changes in the first trial: source 97.5% -> 97.4%, strict 76.4%
-> 77.8%, instructions 1037/1003 -> 1037/1015; inventory 96.4% -> 96.6%, opcode
97.0% -> 97.2%, control 98.1% -> 97.8%, ordered control 83.6% -> 84.1%, call
count 99.3% -> 97.8%, size 96.5% -> 97.7%. Other 21 rows are identical by symbol,
not zipped report position (sorting changes). Its measurements reproduce the
old separate-warning result; combining common jumps gives no additional gain.
The frozen normalizer's previously documented three missed primary calls remain
unmodified. Trial object SHA-256:
1eae2e51e9befa8d14643e53faf291d819a23e099d0ef8d8b1d19dc6eb8eeb12.

The distance-order trial produces the entire accepted focused report and whole
accepted object unchanged. Both trials and both fresh-source restorations compile
204/204 ARM TUs with zero failures, compare 22 symbols with zero skips. Both
restored entire reports equal accepted fifty-first. Final restored object hash:
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Production/canonical inputs remain 9a96e2e5; means remain source 99.205% /
98.51861709618363%. Weighted 99% remains unmet. No new native/full/runtime result
is claimed; accepted fifty-first validations remain authoritative.

Evidence: _work/before-warning-combination-fiftyseventh.cpp,
warning-combination-fiftyseventh.cpp/json/log, fiftyseventh-restored-focused.json/log,
before-center-distance-order-fiftyeighth.cpp, center-distance-order-fiftyeighth.json/log
and fiftyeighth-restored-focused.json/log. Compact committed evidence:
docs/validation/starmap_2026-10-05_warning_distance_trials.json.


## Investigation after checkpoint bab9cd7f

A primary machine precision audit found no new mismatch in station spin/rotation:
ca596 uses VMUL.F32 for magnitude * 0.01, ca5ac uses VADD.F32 for spin, and
ca5da uses VMUL.F32 for dt * 0.0002. Existing float arithmetic agrees; IDA's
unsuffixed decimals do not justify changing these expressions to double.

Two separate update pitch-selection hypotheses were tested:

- Sequential lower then upper: retain the lower-bound ternary and apply the
  upper comparison to its result instead of nextPitch. Source 98.7% -> 98.6%,
  strict 82.7% -> 81.1%, instructions 971/966 -> 971/963, inventory 97.0% -> 96.8%,
  opcode 98.2% -> 98.1%, size 99.6% -> 99.3%.
- Upper then lower: upperPitch = nextPitch > 8192 ? 8192 : nextPitch, followed
  by pitch = upperPitch > -8192 ? upperPitch : -8192. Source 98.7% -> 98.5%,
  strict 82.7% -> 81.0%, instructions 971/966 -> 971/963, inventory 97.0% -> 96.7%,
  opcode 98.2% -> 98.1%, size 99.6% -> 99.3%.

Both retain two predicates and the same final pitch for below/at lower bound,
interior values including signed zero, at/above upper bound, infinities and NaN
(the latter selects -8192). These are equivalent output-selection hypotheses,
not recovered source syntax or a claim about native runtime equivalence. No
extra check/call, forced register, padding, compiler flag or shared change was
introduced. Neither improved the requested source metric, so both were removed.
All control metrics and call count remain unchanged; other 21 rows identical
by symbol for each trial. Trial hashes, respectively:
91a9aed2b0e232fb092cc3c190ddeaafe3ff4991e6ced48d50ff4c73203c11d0 and
5d1b4bb731b2d553b5116a66d080cdefe81eac664039a9b3e8ef9c43a4f73d90.

Each trial and each fresh restoration compiles 204/204 ARM TUs with zero failures,
22 comparisons and zero skips. Both restored entire reports equal accepted
fifty-first, and final object hash is
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Production/canonical inputs remain 9a96e2e5; fixed means remain source 99.205% /
98.51861709618363%. Weighted 99% remains unmet. No new native/full/runtime run
is claimed; accepted fifty-first validations remain authoritative.

Evidence: _work/before-update-sequential-clamp-fiftyninth.cpp,
update-sequential-clamp-fiftyninth.json/log, fiftyninth-restored-focused.json/log,
before-update-upper-first-sixtieth.cpp, update-upper-first-sixtieth.json/log and
sixtieth-restored-focused.json/log. Compact committed snapshot:
docs/validation/starmap_2026-10-05_update_clamp_trials.json.


## Investigation after checkpoint 8c1c4487

Fresh info allocation-shape diagnostics reproduce source 98.9%, inventory 97.8%,
opcode 98.9%, control 98.7%, ordered control 90.2%, calls 100% and size 99.1%.
The 42 sequence-difference groups are diagnostic alignment, not 42 source bugs.
Primary c8ecc..c8ee8 and accepted 17a2..17be already capture Canvas, GameText and
font before getRace, then call getText and DrawString in the same order. No
new capture change is justified by this selected-system race row.

Primary IDA v75/v114 uses an address into alpha for repeated low-byte draws.
The diagnostics show target add-address/byte-load tokens where current code
uses direct field loads/casts. A separate source trial captured a const
unsigned-char pointer to alpha before the system-label color operations and
used fresh byte reads for their alpha arguments. Full-int comparisons/clamps,
station inverse-alpha paths, pulse conversion and callbacks were retained.
It neither cached alpha across a callback nor changed calls. On the primary
little-endian ARM and current Windows targets, byte zero gives the same low
byte as the previous casts. This was a legal object-representation/source-form
hypothesis, not exact recovered syntax; no raw Matrix storage, compiler hint
or barrier was introduced.

The entire trial focused report and whole object equal accepted source.
It was removed because it gives no generated-code/source-shape progress.
Trial and fresh restore each compile 204/204 ARM TUs, zero failures, 22
comparisons and zero skips. Restored entire report equals accepted fifty-first;
trial and restored object SHA-256:
3435be8567c5d64ae5a6d703439be96ba129f9cf76432aab9d81e2781163c20d.
Production/canonical inputs remain 9a96e2e5; source means stay 99.205% /
98.51861709618363%, weighted 99% remains unmet. No new native/full/runtime run
is claimed; accepted fifty-first validation remains authoritative.
Evidence: _work/info-local-differences-sixtyfirst.json,
before-info-alpha-byte-sixtyfirst.cpp, info-alpha-byte-sixtyfirst.json/log and
sixtyfirst-restored-focused.json/log; compact committed evidence:
docs/validation/starmap_2026-10-05_info_byte_trial.json.


## Accepted position-copy restoration after checkpoint 9cd4cf43

drawOnScreenInfo now performs the station/system scratchVector copy in explicit
branches, instead of choosing an Array pointer before the copy. Each runtime
path still reads only its selected array, copies the same indexed Vector to
scratchVector, and reaches the unchanged bounds/marker/text logic. Vector is a
three-float 12-byte struct, with a trivial implicit copy construction; its
assignment copies x/y/z. No extra runtime call, predicate, bounds change,
compiler hint, shared-code edit or padding was introduced.

Primary c8376..c8398 uses ITE EQ, LDREQ from this+404, LDRNE from this+408,
then a common data/index load and one Vector assignment. New c66..c88 now
reproduces this selection and single-copy sequence. The compiler merges the
two source copies. This is a source-form hypothesis supported by resulting
machine evidence, not recovered original C++ syntax. It differs from the old
explicit Array-pointer if/else trial, which left offset selection unchanged.

Only info changes: source 98.9% -> 99.0%, strict 79.2% -> 79.9%, instructions
1207/1196 -> 1207/1195, inventory 97.8% -> 98.1%, opcode 98.9% -> 99.0%, size
99.1% -> 99.0%. Control/call metrics and other 21 rows are unchanged. Fixed
means become source 99.210% / 98.53561470215463%, strict 87.260% /
81.70414026193494%. Weighted shortfall remains 0.46438529784537 percentage
points. Both means must pass 99%; the goal remains unmet.

Focused ARM compiled 204/204 TUs, zero failures, 22 rows, zero skips. UCRT
gof2 static-library build passed. Full corpus: 4619 comparisons, zero skips,
2083 linked-exact and 960 byte-exact, avg source 89.06%, fuzzy 76.16%, 2203
fuzzy-perfect entries. Coverage 4274/4524, missing 250 (7 wrong-type/243 absent),
extra 129, unchanged. Every focused/full StarMap metric/count/exact flag except
unit-path spelling was checked for exact agreement. Baseline and previous
9cd4cf43 (production 9a96e2e5) preservation gates exit 0, zero exact-pair losses;
default 99% gate exits 1 with the expected threshold failure. Runtime remains
untested; a static build/source metric does not prove executable equivalence.

Evidence: _work/before-info-position-copy-sixtysecond.cpp,
info-position-copy-sixtysecond.json/log/asm, native-sixtysecond.log,
sixtysecond-corpus.json/log, sixtysecond-gate.json,
sixtysecond-previous-gate.json and sixtysecond-target-gate.log. Previous canonical
inputs are checkpoint62-before-focused.json/corpus.json. Object SHA-256:
156da3b108337cf6b83438749665458002e66fb24536600aa795dc86469e25a3.
Full JSON SHA-256: f413437473ee7152d6301ffa143a7f6d2ba664133562d6362f0b0784f0dc5e39.
Compact snapshot: docs/validation/starmap_2026-10-05_info_position_copy.json.


## Investigation after checkpoint 6c5707e7

The accepted branch-local info copy was checked for analogous paired-field
selection opportunities; no equivalent case justified copying that change
elsewhere blindly. A constructor image hypothesis combined the two image
choices into campaign >157 && i==27 with a base-texture else. The campaign
getter remains the first operand and still executes once on every iteration;
texture getter order, 16-bit conversion and final selected IDs are unchanged.
The primary v15/v16 select/restore logic proves the final values, not this
compound source syntax. No getter is cached or extra check/call introduced.

The trial's entire focused report and whole object equal accepted sixty-second.
It was removed because it gives no generated-code/source progress. Trial and
fresh restoration each compile 204/204 ARM TUs, zero failures, 22 comparisons
and zero skips; restored entire JSON equals canonical accepted input. Both
trial/restored object SHA-256:
156da3b108337cf6b83438749665458002e66fb24536600aa795dc86469e25a3.
C1/C2 remain source 99.3%, strict 95.8%, 333/333 instructions.

Read-only discovery typing review found Station::isDiscovered returns uint8_t
and SolarSystem::isFullyDiscovered returns int. Accepted d24/d34 calls converge
at CBZ R0 d38 and icon store, matching primary c8434/c8444/c8448. There is no
caller masking/conversion instruction to remove, so no bool/int rewrite is
justified by this suspected discrepancy. Shared headers were only inspected.

Production/canonical inputs remain 6c5707e7, means source 99.210% /
98.53561470215463%. Weighted 99% remains unmet. Accepted sixty-second native/full
validation remains authoritative; no new native/full/runtime run is claimed.
Evidence: _work/before-ctor-image-selection-sixtythird.cpp,
ctor-image-selection-sixtythird.json/log and sixtythird-restored-focused.json/log;
compact committed snapshot: docs/validation/starmap_2026-10-05_ctor_image_trial.json.

## Sixty-fourth trial: confirmation inside choice result case (removed)

Inspected StarMap.cpp OnTouchEnd, the primary Android touch-end extract,
and focused ARM outputs. The entire original confirmation tail was placed
inside switch case 0; case 1 retained its two flag clears and early return,
and default retained return 0. All calls, predicates and exit paths were
preserved. This source-form hypothesis is distinct from fortieth's case 0
break with the confirmation tail outside the switch. It does not establish
recovered original C++ syntax.

Trial and fresh restoration each compile 204/204 ARM TUs with zero failures,
22 comparisons and zero skips. Both entire focused reports equal accepted
sixty-second. Both whole objects have SHA-256
156da3b108337cf6b83438749665458002e66fb24536600aa795dc86469e25a3.
The trial was removed because it provides no generated-code or metric gain.
Production remains 6c5707e7; source means remain 99.210% / 98.53561470215463%.
Weighted 99% remains unmet. Accepted sixty-second native/full validation
remains authoritative; no new native/full/runtime run is claimed.

Evidence: _work/before-choice-case-body-sixtyfourth.cpp,
choice-case-body-sixtyfourth.json/log and sixtyfourth-restored-focused.json/log.
Compact committed snapshot:
docs/validation/starmap_2026-10-05_choice_case_body_trial.json.

## Sixty-fifth accepted correction: confirmation station reread

Inspected StarMap.cpp OnTouchEnd, primary touch-android.c and bounded raw
Android ARM, accepted sixty-second ARM, and the new focused/full outputs.
Primary cb19e reads the selected station element, cb1a2 stores it in
Level::programmedStation, cb1a4 branches on the saved jump mode, cb1a8 rereads
the element, and cb1ac calls Station::getSystem. Accepted sixty-second reused
the local pointer after the store, omitting that indexed reread. Removed the
local Station pointer and used the member expression again for getSystem.
The new 217c/2180/2182/2186/218a sequence matches this access order.

This is grounded in actual ARM loads, not recovered original C++ syntax.
Same array/index, global store, predicates, calls and exits are retained;
no additional call/check, volatile, barrier, forced register or padding is
introduced. Shared/Level files and verifier remain untouched.

OnTouchEnd source remains 97.5%, strict 76.4% -> 76.9%; instruction inventory
96.4% -> 96.6%, opcode inventory 97.0% -> 97.1%, size 96.5% -> 96.6%,
1037/1003 -> 1037/1004 instructions. Control 98.1%, ordered control 83.6%,
and calls 99.3% remain unchanged. Other 21 focused rows are identical.
Fixed-body source means remain 99.210% / 98.53561470215463%; strict means
improve to 87.285% / 81.7771581467399%. Weighted 99% remains unmet.

Focused ARM: 204/204 TUs, zero failures, 22 comparisons, zero skips. Native
UCRT static-library build compiles StarMap and links libgof2.a successfully.
Full corpus: 4619 comparisons, zero skips, 2083 linked-exact and 960 byte-exact;
coverage/missing/extra counts unchanged. Every focused/full row agrees after
normalizing unit separator spelling. Baseline and previous accepted 0a4d9116
(production 6c5707e7) preservation gates pass with zero lost exact pairs.
Default 99% gate remains red, exit 1. Native runtime has not been tested.

Evidence: _work/before-confirm-station-read-sixtyfifth.cpp,
confirm-station-read-sixtyfifth.json/log/asm, native-sixtyfifth.log,
sixtyfifth-corpus.json/log, sixtyfifth-baseline-gate.json/log,
sixtyfifth-previous-gate.json/log and sixtyfifth-target-gate.json/log.
Previous accepted inputs frozen in checkpoint65-before-focused/corpus.json.
Object SHA-256: fdab4e38098779300af2fbef8ca8c4b2abb3aeb556e5a1e576f59c3c7844cb41.
Full JSON SHA-256: 9b4234e32adfb8cc149365e86664e6406b6ce72182c588906bba89f80a9d5565.
Compact committed snapshot:
docs/validation/starmap_2026-10-05_confirm_station_read.json.

## Sixty-sixth accepted source form: choice confirmation first

Inspected StarMap.cpp, primary touch-android.c and bounded raw ARM, accepted
sixty-fifth assembly and focused/full outputs. Reordered the two existing
result predicates: result 0 holds the entire existing confirmation tail;
else if result 1 clears choiceVisible and alienJumpPending; common return 0
handles fallthrough and other results. The window callback still runs once
before either test. Result 0 retains every field change, predicate, call and
early return; result 1 retains its two clears and return 0; other results have
no writes/calls and return 0. No new comparison, callback, barrier, volatile,
register forcing or padding was introduced. This source-form hypothesis is
not recovered original C++ syntax: primary Android tests result 1 before 0.

OnTouchEnd source 97.5% -> 97.7%, strict 76.9% -> 80.0%; instruction inventory
96.6% -> 96.8%, opcode inventory 97.1% -> 97.4%, ordered control 83.6% ->
84.1%, size 96.6% -> 96.7%. Control inventory 98.1% and calls 99.3% unchanged;
1037/1004 instructions remain. Other 21 focused rows are identical.
Fixed-body source means improve to 99.220% / 98.56482185607662%; strict means
to 87.440% / 82.22986903253063%. Weighted 99% remains unmet.

Two candidate rewrites were excluded read-only: station TransformSetLocal
primary c78b6..c78ce matches previous 2cb6..2cce including receiver/handle
capture before getMatrix; confirmation camera primary cb33e..cb34c matches
previous 2302..2310, including prevCamera read before exitRequested store.
No source edit was made for either already-matching fragment.

Focused ARM: 204/204 TUs, zero failures, 22 comparisons, zero skips. Native
UCRT build log shows StarMap compilation and libgof2.a link; a subsequent
incremental confirmation exited 0. Initial full/native tool observation was
interrupted; existing processes were later absent and completed reports/logs
were verified. Original wrapper exit was not captured; no full run restarted.
Full corpus has 4619 comparisons, zero skips, 2083 linked-exact and 960
byte-exact. Coverage/missing/extra unchanged. All focused/full rows agree
after unit separator normalization. Baseline and previous 67200c06 gates pass
with zero lost exact pairs. Default 99% gate fails, exit 1. Native runtime
has not been tested. Verifier, shared headers/configs and other production
files were not changed.

Evidence: _work/before-choice-zero-first-sixtysixth.cpp,
choice-zero-first-sixtysixth.json/log, native-sixtysixth.log,
native-sixtysixth-confirmed.log, sixtysixth-corpus.json/log,
sixtysixth-baseline-gate.json/log, sixtysixth-previous-gate.json/log and
sixtysixth-target-gate.json/log. Previous inputs checkpoint66-before-focused/
corpus.json. Object SHA-256: 06be7829eb5212aa1cd23cb50dc8771fbdad2a3165605af307e7d4ff296f1d34.
Full JSON SHA-256: f38c856b0ea96dfbe81dc4fc1f99577cec275b9802fdcf32bedaa9e285d8d590.
Compact committed snapshot: docs/validation/starmap_2026-10-05_choice_zero_first.json.

## Sixty-seventh trial: warning combination on new confirmation form (removed)

Inspected StarMap.cpp, primary touch/draw extracts, bounded original ARM and
accepted assembly. Retested fifty-seventh's separate warning 3217 plus common
goto-selected warning delivery on accepted sixty-sixth. Only the warning
region was transplanted; confirmation-zero-first and post-programmedStation
station reread both remained. Runtime predicate/getter ordering, selected text,
choiceVisible, suppressNextClose and subsequent inertia/Help were preserved.
No original C++ syntax recovery is claimed.

OnTouchEnd source 97.7% -> 97.6%, strict 80.0% -> 81.3%; instruction inventory
96.8% -> 97.0%, opcode 97.4% -> 97.5%, control inventory 98.1% -> 97.8%,
ordered control 84.1% -> 84.5%, normalized calls 99.3% -> 97.8%, size 96.7%
-> 97.9%, 1037/1004 -> 1037/1016 instructions. Other 21 rows unchanged.
Rejected because it lowers the requested source metric. Trial object SHA-256:
bd5b7f07db16204bcea8d5734219ffd96edecfaaedb3885b086862e0fa47834b.

Trial and fresh restoration each compile 204/204 ARM TUs with zero failures,
22 comparisons and zero skips. Restored entire focused JSON equals canonical
accepted sixty-sixth, object SHA-256
06be7829eb5212aa1cd23cb50dc8771fbdad2a3165605af307e7d4ff296f1d34.
Production remains 3aebb2c0; source means 99.220% / 98.56482185607662%.
Weighted 99% remains unmet. No new native/full/runtime run is claimed;
accepted sixty-sixth validation remains authoritative.

Read-only draw audit excluded two suspected differences: native c9166..c91a4
and accepted 276..2b4 already retain the same transitionIn byte through alpha
store and mode comparison. Primary path-phase VCMPE c952c / BLE c9534 skips
increment when not greater, including unordered flags; current phase>field_0xec
has that behavior. No transition snapshot or unordered-comparison edit made.

Evidence: _work/before-warning-combined-sixtyseventh.cpp,
warning-combined-sixtyseventh.json/log, sixtyseventh-restored-focused.json/log.
Compact snapshot: docs/validation/starmap_2026-10-05_warning_combined_trial.json.

## Sixty-eighth / sixty-ninth trials: mode ordering (removed)

Inspected StarMap.cpp OnTouchBegin/update and primary Android update extract.
Tested mode 3 before mode 0 separately in each method. OnTouchBegin retains
its oldSystem snapshot and complete bodies; update retains the common prefix,
selects mode after setParamValue, preserves every callback/store and mode-0
return, and leaves unsupported modes without actions. These are source-form
hypotheses, not recovered C++: the primary update extract tests mode != 3 first.
No additional predicate/call, volatile, barrier, padding or flags were added.

Sixty-eighth OnTouchBegin: entire focused report and whole object equal accepted
sixty-sixth, 99.3% source / 97.4% strict / 275/273 instructions. Removed as neutral.
Object SHA-256 06be7829eb5212aa1cd23cb50dc8771fbdad2a3165605af307e7d4ff296f1d34.

Sixty-ninth update: source 98.7% -> 98.0%, strict 82.7% -> 79.6%, instruction
inventory 97.0% -> 96.2%, opcode 98.2% -> 97.5%, control inventory 98.7% ->
97.0%, ordered control 95.4% -> 92.4%, calls 100% unchanged, size 99.6% ->
99.5%, 971/966 -> 971/965 instructions. Other 21 rows identical. Removed as a
source/control regression. Trial SHA-256
5ad6c16d9111b0b7695c10b6da719e73acaf4a4515a4c7f02f4c846dd8c13f79.

Each trial and each fresh restoration compile 204/204 ARM TUs with zero failures,
22 comparisons and zero skips. Both restored entire focused reports equal
canonical accepted input; final restored object SHA-256
06be7829eb5212aa1cd23cb50dc8771fbdad2a3165605af307e7d4ff296f1d34.
Production remains 3aebb2c0; source means 99.220% / 98.56482185607662%.
Weighted 99% remains unmet. No new native/full/runtime run is claimed;
accepted sixty-sixth remains authoritative.

Evidence: _work/before-touch-mode-order-sixtyeighth.cpp,
touch-mode-order-sixtyeighth.json/log, sixtyeighth-restored-focused.json/log,
before-update-mode-order-sixtyninth.cpp, update-mode-order-sixtyninth.json/log,
sixtyninth-restored-focused.json/log. Compact snapshot:
docs/validation/starmap_2026-10-05_mode_order_trials.json.

## Seventieth trial: touch-end galaxy branch first (removed)

Inspected StarMap.cpp, primary/accepted update ARM and focused reports. Reordered
OnTouchEnd's dragging branches: mode==0 enters the complete galaxy body; else
enters the complete station body, including every other nonzero mode. Previously
mode!=0 selected the station body first. Predicates, getters, warning handling,
inertia, Help and the accepted confirmation tail were preserved. This is a
source-form hypothesis, not recovered C++ syntax. No new check/call or padding.

The entire trial focused report equals accepted sixty-sixth: OnTouchEnd remains
97.7% source / 80.0% strict / 1037/1004 instructions, all 22 rows unchanged.
Whole object differs, SHA-256
33585af7b6c3082b8e01cdb56e2ffbb1c53aec2f2a8d48db101f00cc346bd493.
The object difference was not classified; report equality does not prove object
identity or full-corpus preservation for the rejected trial. Rejected because
there is no improvement to the requested source metric.

Trial and fresh restoration each compile 204/204 ARM TUs, zero failures,
22 comparisons and zero skips. Restored entire report equals canonical accepted
input and whole object SHA-256 is
06be7829eb5212aa1cd23cb50dc8771fbdad2a3165605af307e7d4ff296f1d34.
Production remains 3aebb2c0, source means 99.220% / 98.56482185607662%.
Weighted 99% remains unmet. No new native/full/runtime run is claimed;
accepted sixty-sixth remains authoritative.

Camera-bound audit excluded a declaration-order rewrite: primary ca194..ca1cc
and accepted 37b0..37e8 already load panX/panY, then baseX/baseZ, convert Y then
X, add Y then X, convert Y then X, move Y/X to R0/R1 and compare X to 120 in
matching order. No camera-coordinate source change was made.

Evidence: _work/before-end-mode-order-seventieth.cpp,
end-mode-order-seventieth.json/log/o and seventieth-restored-focused.json/log.
Compact snapshot: docs/validation/starmap_2026-10-05_end_mode_order_trial.json.

## Seventy-first inventory refresh / seventy-second accepted race selection

Recomputed unchanged asmdiff source submetrics and allocation-normalized token
inventories for all fixed 20 bodies against the accepted sixty-sixth object.
Every metric reproduced canonical input. The denominator remains 7101 and
aliases remain checked separately. Raw inventories in
_work/all-body-inventories-seventyfirst.json; compact evidence in
docs/validation/starmap_2026-10-05_inventory_refresh.json. Multiset differences
are not missing C++ statements or a proven compiler ceiling. This diagnostic
identified a concrete info race-selection conditional-field-role mismatch.

Changed only info's if/else predicate order from race 2/1/0 to 0/1/2. The same
single getRace call, field mapping (0:+292, 1:+296, 2:+300, other:+304), and image
call/arguments remain. No additional check/call, padding, volatile or barrier.
This C++ form is a hypothesis, not recovered syntax. Its new ARM 16a6..16c6
compares 2/1/0 and conditionally selects +304/+292, with separate +300/+296
paths and common image load, matching primary c8d38..c8df2.

Info source 99.0% -> 99.1%, strict 79.9% -> 80.2%, instruction inventory
98.1% -> 98.4%, opcode 99.0% -> 99.1%, control inventory 98.7% -> 99.1%,
ordered control 90.2% -> 90.6%, calls 100% unchanged, size 99.0% -> 99.1%,
1207/1195 -> 1207/1196 instructions. Other 21 focused rows identical.
Source means now 99.225% / 98.5818194620476%; strict 87.455% /
82.2808618504436%. Weighted 99% remains unmet.

Focused ARM: 204/204 TUs, zero failures, 22 comparisons, zero skips. Native
UCRT static-library build exits 0, compiling StarMap and linking libgof2.a.
Full run exits 0: 4619 comparisons, zero skips, 2083 linked-exact, 960 byte-exact,
coverage/missing/extra unchanged. All focused/full rows agree after unit
separator normalization. Baseline and previous 5ce4b318 (production 3aebb2c0)
gates pass with zero lost exact pairs. Default 99% gate remains red, exit 1.
Native runtime has not been tested. No verifier/shared/config/other production
files were edited.

Evidence: _work/before-race-zero-first-seventysecond.cpp,
race-zero-first-seventysecond.json/log/asm, native-seventysecond.log,
seventysecond-corpus.json/log, seventysecond-baseline-gate.json/log,
seventysecond-previous-gate.json/log, seventysecond-target-gate.json/log.
Previous inputs checkpoint72-before-focused/corpus.json.
Object SHA-256: f5ebf2ce9c0f3b03c50a8a87fb106a406a1488a71210bb0dbc0c61f3a4df9cc5.
Full JSON SHA-256: a36bf65219055df2dcf7fade9c0b089fecaa45d415da3614951eca2094ce7314.
Compact snapshot: docs/validation/starmap_2026-10-05_race_zero_first.json.

## Seventy-third accepted station selection store order

Changed only init's two independent assignments after a station match. The
C++ order selectedStation then lastSelectedStation now generates ARM stores
lastSelectedStation (+416) then selectedStation (+100), matching the primary
c7202/c7206. The complete eight-instruction fragment c71f6..c720c matches new
4b82..4b98, including registers, guards and offsets, apart from addresses.
No intervening callback, additional call/check, barrier or padding. This is a
source-form hypothesis, not recovered original C++ syntax.

Init strict 77.1% -> 77.3%; all source submetrics and other 21 rows unchanged.
Source means remain 99.225% / 98.5818194620476%. Strict means are
87.465% / 82.30198563582594%. The weighted 99% goal remains unmet.
Focused ARM 204/204, zero failures, 22 comparisons, zero skips. Native static
library build and full comparison exit 0; full 4619, zero skips. All focused
rows agree with full rows. Baseline and previous 9a2d1046 gates pass with zero
exact-pair loss (2083 linked / 960 byte); default 99% gate exits 1. Native
runtime has not been tested.

Read-only next-candidate audit: primary c6f10 loads the systemGeoms Array
before the second Station::getSystem at c6f14, then c6f18 loads its data.
Current 4aae/4ab2 loads both Array and data before the call at 4ab6. A captured
Array with its data accessed after the callback is a distinct candidate;
no such production edit is part of this checkpoint.

Evidence: _work/before-init-station-stores-seventythird.cpp,
init-station-stores-seventythird.json/log/asm, native-seventythird.log,
seventythird-corpus.json/log, seventythird-baseline/previous/target-gate.json/log,
checkpoint73-before-focused/corpus.json,
init-target-data-read-audit-seventythird.json.
Compact snapshot: docs/validation/starmap_2026-10-05_init_station_stores.json.
Object SHA-256: e37a65088022af392d978ada394e7fd01803e04ed1e8745ad9576fe445920f58.
Full JSON SHA-256: 5f547fa3aa49c5cb1be385e1ff9e5fa99f5de066b9b00037779b00fad976e2e4.

## Seventy-fourth accepted target geometry data read ordering

In init's mission-target focus path, capture the systemGeoms Array before the
second Station::getSystem, then access its data after the callback. Primary
c6f0e..c6f1c loads the Array at +104, calls getSystem, loads data at +4 and
branches to common position retrieval. New 4aae..4abc has the same operations
and registers apart from relocated calls/branches. Previously data was loaded
before getSystem and element indexing was separate. Both getter calls,
predicates, chosen index, position retrieval, assignment and deletion remain.
No additional call/check, barrier or padding. This source form implements an
observed native read order; original C++ syntax and runtime equivalence have
not been proven.

Init instruction/opcode inventories 97.9% -> 98.0%; instructions 750/736 ->
750/735, size 97.7% -> 97.6%. Source 98.3%, strict 77.3% and control/call
metrics unchanged; other 21 rows unchanged. Source means remain 99.225% /
98.5818194620476%; strict 87.465% / 82.30198563582594%. The weighted 99% goal
remains unmet. Unchanged analyzer reproduced all source submetrics; current
surplus no longer includes the separate indexed-element load. Inventories are
diagnostics, not counts of missing C++ statements or a proven compiler ceiling.

Focused ARM 204/204, zero failures, 22 comparisons, zero skips. Native static
library build and full comparison exit 0; full 4619, zero skips. All focused
rows agree with full rows. Baseline and previous baf37291 gates pass with zero
exact-pair loss (2083 linked / 960 byte); default 99% gate exits 1. Native
runtime has not been tested. Shared files, verifier and configuration unchanged.

Evidence: _work/before-init-target-data-seventyfourth.cpp,
init-target-data-seventyfourth.json/log/asm, native-seventyfourth.log,
seventyfourth-corpus.json/log, seventyfourth-baseline/previous/target-gate.json/log,
checkpoint74-before-focused/corpus.json, init-inventory-seventyfourth.json.
Compact snapshot: docs/validation/starmap_2026-10-05_init_target_data.json.
Object SHA-256: 04cee56d72e89477232ebb4e08ec1f935ec094ed496b80efec143c01159c9e43.
Full JSON SHA-256: adc1704841a6f2832a7ef19df54c12747d9e961d9028af5a0bb756930970c97c.

## Seventy-fifth accepted draw current-system data read ordering

In draw's current-system position copy, capture systemPositions Array before
Status::getSystem and SolarSystem::getIndex, then access the Array data after
both callbacks. Primary c9326..c933c loads Status, the Array at +404, calls
both getters, then loads data at +4 and the indexed element. New 436..44c
has the same operation, field and call order; the stack temporary register
still differs. Previously the data pointer was loaded before the callbacks.
Same getter counts, selected index, Vector copy/assignment and separate
current-route getter calls. No additional call/check, barrier or padding.
Original C++ syntax and native runtime equivalence have not been proven.

Draw strict 89.4% -> 89.7%; source 99.1%, every source submetric, instruction
count and other 21 rows unchanged. Source means remain 99.225% /
98.5818194620476%; strict means 87.48% / 82.34068441064638%. Weighted 99%
remains unmet.

Focused ARM 204/204, zero failures, 22 comparisons, zero skips. Native static
library build and full comparison exit 0; full 4619, zero skips. All focused
rows agree with full rows. Baseline and previous bf375eeb gates pass with zero
exact-pair loss (2083 linked / 960 byte); default 99% gate exits 1. Native
runtime has not been tested. Shared files, verifier and configuration unchanged.

Read-only audits exclude three rewrites without new evidence: station angle
assignment c774c..c778a matches 2b36..2b74, including retries, fresh flag reads,
division, multiplication and assigned-flag test; centeredStation comparison
c78dc..c7908 matches the current field/callback order at 2ce4..2d10; mission
139 ship-class/equipment checks cb81e..cb848 match current 278a..27b4 getter
and condition order. These findings do not establish whole-body equivalence
or a compiler ceiling. No production edit was made for those audited blocks.

Evidence: _work/before-draw-current-data-seventyfifth.cpp,
draw-current-data-seventyfifth.json/log/asm, native-seventyfifth.log,
seventyfifth-corpus.json/log, seventyfifth-baseline/previous/target-gate.json/log,
checkpoint75-before-focused/corpus.json, station-read-audit-seventyfifth.json.
Compact snapshot: docs/validation/starmap_2026-10-05_draw_current_data.json.
Object SHA-256: 1ab6e994ad315ab15146f82a3905f6838557d3e819c6f01f24f79b8c33d972a9.
Full JSON SHA-256: 8dbb63f8358823a852b116ef0d4bda527002f994a8adf600333914be733c2b40.

## Seventy-sixth / seventy-seventh neutral source-form trials

Two independent hypotheses were compiled and removed. OnTouchMove put the
complete galaxy body under mode==0 first and the complete station body under
else, retaining all nonzero modes and the accepted pitch predicates. Draw's
initial transition guard used a uint8_t local initialized with transitionIn,
replaced by transitionOut only when zero, then tested nonzero. This preserved
short-circuit reads, nonzero-byte behavior and later callback-visible reads.
Neither added calls/checks, barriers, padding or compiler/configuration changes.
Both entire focused reports and whole ARM objects were identical to accepted
75. Neither source form is claimed as recovered original C++ syntax.

Each trial and fresh-mtime restore compiles 204/204 ARM TUs, zero failures,
22 comparisons, zero skips, exit 0. Both complete restored reports and the
final whole ARM object equal accepted75. Production source restored identically after newline decoding. Source means remain 99.225% / 98.5818194620476%;
strict 87.48% / 82.34068441064638%. No new native/full/runtime run; accepted75
full/native evidence remains authoritative. Weighted 99% remains unmet.

Read-only review: OnTouchEnd entry caeb2..caee0 and accepted1e8a..1ebc already
have choice callback before ordinary input, same result1/result0 tests and
suppressNextClose read before choiceVisible store; no outer dispatch edit made.
Draw initial guard c9120..c912c has ITT EQ / conditional byte load +313 /
conditional comparison; accepted22e..23c uses explicit BNE then load/compare.
The byte fallback did not alter that compiler decision. No forced predication
or proven compiler ceiling is inferred from these trials.

Evidence: _work/before-move-mode-order-seventysixth.cpp,
move-mode-order-seventysixth.json/log, seventysixth-restored-focused.json/log,
before-draw-transition-byte-seventyseventh.cpp,
draw-transition-byte-seventyseventh.json/log,
seventyseventh-restored-focused.json/log.
Trial/restored object SHA-256:
1ab6e994ad315ab15146f82a3905f6838557d3e819c6f01f24f79b8c33d972a9.
Compact snapshot: docs/validation/starmap_2026-10-05_move_transition_trials.json.

## Seventy-eighth low-cargo confirmation station read trial (removed)

Primary low-cargo confirmation with jumpCost==1 and jumpMapModeA reads the
selected Station element at cb05e before doInstantJump=0 at cb064, then stores
programmedStation at cb066. Accepted75 instead stores the byte at 2094 before
its selected-element read at 20a2 and programmedStation store at 20a6.
Tested a local Station pointer read before the byte store, retaining the same
single indexed read, two stores, callbacks, predicates and all other branches.
Trial 209e/20a4/20a6 reproduces the primary read-before-store relationship.
This is an evidenced ARM ordering, not recovered C++ syntax or a runtime test.
No extra call/check, padding, barrier or shared-file edit.

The entire focused report is identical to accepted75: no source, strict,
inventory, count or control gain. Whole trial object differs, SHA-256
706fc5f802c2db029d34f908de311f453ef2d1ecf59e38a4599179ffb671b7c2.
It was removed because it gives no progress toward the requested metric.
The trial object is retained in _work/low-cargo-station-seventyeighth.o for
future evidence. No full exact-pair preservation is claimed for this trial.

Trial and fresh-mtime restore each compile 204/204 ARM TUs, zero failures,
22 comparisons, zero skips, exit 0. Restored complete focused report and whole
ARM object equal accepted75; restored source is identical after newline
decoding. Source means remain 99.225% / 98.5818194620476%, strict 87.48% /
82.34068441064638%. No new native/full/runtime run; accepted75 full/native
results remain authoritative. Weighted 99% remains unmet.

Evidence: _work/before-low-cargo-station-seventyeighth.cpp,
low-cargo-station-seventyeighth.json/log/asm/o,
seventyeighth-restored-focused.json/log.
Restored object SHA-256:
1ab6e994ad315ab15146f82a3905f6838557d3e819c6f01f24f79b8c33d972a9.
Compact snapshot: docs/validation/starmap_2026-10-05_low_cargo_trial.json.

## Seventy-ninth accepted constructor Array length argument reads

Constructor now passes *this->systemGeoms and *this->systemPositions to the
two existing ArraySetLength(34,...) calls instead of cached allocation locals.
Primary c68b2..c68c6 rereads +104 after the position Array constructor and
+404 after the geometry ArraySetLength callback. New 4216..422a reproduces
those fields, registers, size34 and call order, apart from linker call
relocations. Allocations, constructors and member stores are retained. No
extra call/check, padding, barrier or shared-file/configuration change. These
are native member-read observations; original C++ syntax and native runtime
equivalence have not been proven.

C1/C2 source 99.3% -> 99.5%, strict 95.8% -> 97.6%; instruction and opcode
inventory 98.2% -> 99.1%, instructions 333/333 -> 333/331, size 100% -> 99.4%.
All control/call metrics remain 100%; other 20 rows unchanged. Aliases remain
checked and excluded from means. The denominator stays 20 bodies / 7101.
Source means now 99.235% / 98.59119842275736%; strict 87.57% /
82.42509505703421%. Weighted 99% remains unmet by 0.40880157724264 pp.
Unchanged analyzer reproduces all submetrics and confirms the +104/+404
surpluses are gone. Remaining inventories are not missing C++ statements or
a proven compiler ceiling. Raw diagnostic: _work/ctor-inventory-seventyninth.json.

Focused ARM 204/204, zero failures, 22 comparisons, zero skips. Native static
library build and full comparison exit 0; full 4619, zero skips. All focused
rows agree with full rows. Baseline and previous 657504fb (production ee4e8b03)
gates pass with zero exact-pair loss (2083 linked / 960 byte). Default 99%
gate exits 1. Native runtime has not been tested.

Evidence: _work/before-ctor-array-reads-seventyninth.cpp,
ctor-array-reads-seventyninth.json/log/asm, native-seventyninth.log,
seventyninth-corpus.json/log, seventyninth-baseline/previous/target-gate.json/log,
checkpoint79-before-focused/corpus.json.
Compact snapshot: docs/validation/starmap_2026-10-05_ctor_array_reads.json.
Object SHA-256: a775a751fc3520acc5f684ccd1d9560acd4da2664cbf01f471fd2c4055f7498f.
Full JSON SHA-256: a4461ba66b3ad1c7fd4b6e3aa0b468c80eef2472da8eb13aa2435164d9c50f84.

## Validation

Final focused ARM build compiles **204/204 translation units, zero failures**;
22 symbols compare, zero skipped, seven unique linked-exact bodies and three
unique byte-exact bodies. Clean baseline has two of each.

Native UCRT Debug `gof2` static-library build passed. Existing compiler warnings
remain; no native executable/runtime test is claimed.

The independent gate freezes the symbol set, original weights and alias
relationships and compares full-corpus exact pairs by **(translation unit,
symbol)**. `--allow-partial` relaxes only the 99% threshold, never corpus
integrity or exact-pair preservation. The default 99% gate must remain red
for this checkpoint.

Full corpus: **4619 comparisons, zero skips** in both clean baseline and final.
Overall source-shape 88.99% -> 89.06%; ordered fuzzy
75.97% -> 76.16% (including all corpus entries).
Coverage remains 4274/4524 original functions, 250 missing (7 wrong-type / 243
absent). Final extra symbols: 129, baseline: 130. Existing coverage gaps were
not removed from reporting.

Exact pair preservation gate: **PASS**, zero lost (unit, symbol) pairs.
Linked-exact **2079 -> 2084**; byte-exact **959 -> 960**. Gains are confined to
StarMap: initLights, missionChanged, askForJumpIntoAlienWorld, setStart and render;
missionChanged also becomes byte-exact.

Default >=99.0% gate: **FAIL**, exit 1, `StarMap >=99.0% target NOT reached`.
Explicit partial/preservation gate: **PASS**, exit 0. Negative checks confirmed
that changed original weights, omitted aliases, skipped symbols and a full-corpus
byte-exact pair loss (even with --allow-partial) are rejected.
`git diff --check` passed.

Committed evidence: `docs/validation/starmap_2026-10-02_baseline.json`,
`starmap_2026-10-02_final.json` (focused inputs) and
`starmap_2026-10-02_gate.json` (full exact-pair difference and both means).
Full raw reports and logs remain in this worktree's _work directory.
Latest full log: `_work/ninetysixth-corpus.log`; focused log:
`_work/current-route-depth-bool-ninetysixth.log`; native log:
`_work/native-ninetysixth.log`.
Exact-pair preservation also passed against checkpoints 7fc7701c, dca65361,
50e10b79, b5010fdb, d4e3330a, bc98481c, 42331c92, 52079d64, 7b76861c, 72045054, eeebc3e6, 644c59e5, e07a1f16, 2f20db28, f10955b3, 613af2c6, f9e870cf, 80ea89b1, c8b7e875, cf030a4e, 7e7528b7, 43f3e1c2, fcb1741b, f7cbbae0, 0fe3756d, 03aa0342 and 72465845, without losses. The earlier accepted88 Render repair adds one linked-exact pair; this checkpoint preserves it. The latest comparison against a4fc71cd preserves all 2084
linked-exact and 960 byte-exact pairs. Latest preservation gate and 99%
gate logs are _work/ninetysixth-baseline-gate.json, ninetysixth-previous-gate.json and
ninetysixth-target-gate.log (exit 0, 0 and 1 respectively).

baseline full JSON SHA-256: `ceb8ddf50e03dbadd2278413942c6e1a8e59b1f6d4060d7628de66593632a6d0`.

final full JSON SHA-256: `ac36671ad80d872e4cd8053180fea46d76c6924b13ef1ae919d43048d9c95d08`.


### Reproduction

Use the unchanged local NDK r18b ARM verifier with GOF2_VERIFY_LOCAL_NDK=1 and
GOF2_NDK_ROOT pointing at the installed android-ndk-r18b; retain its existing
compile flags: armv7-none-linux-androideabi21, ARMv7-A Thumb, NEON, softfp,
PIC, RTTI, stack-protector-strong, Oz and libc++. Commands run from this isolated checkout:

```text
python tools/verify/verify.py --unit game/world/StarMap --only ^_ZN7StarMap --report _work/final-focused.json
python tools/verify/verify.py --no-build --report _work/final-corpus.json
cmake --build cmake-build-ucrt --target gof2 -- -k 0
python tests/check_starmap_source_shape.py _work/baseline-focused.json _work/final-focused.json _work/baseline-corpus.json _work/final-corpus.json --allow-partial
python tests/check_starmap_source_shape.py _work/baseline-focused.json _work/final-focused.json _work/baseline-corpus.json _work/final-corpus.json
git diff --check
```

Clean baseline full comparison used separately copied clean objects at
`_work/baseline-verify/base`, with --build-dir _work/baseline-verify --no-build.
No baseline/final object directory was concurrently mutated during comparison.
The latest complete comparison used GOF2_VERIFY_JOBS=2 to reduce verification
concurrency after the rejected skipped-unit run; no compilation flags changed.

## Fixed unique-body measurements

Percentages are the unchanged verifier output. C2/D2 aliases are excluded.

| Android symbol | Original/current instructions | Baseline source | Final source | Final strict | Final exact |
|---|---:|---:|---:|---:|---|
| `_ZN7StarMap16drawOnScreenInfoEib` | 1207/1198 | 78.9% | 99.3% | 80.6% | - |
| `_ZN7StarMap10OnTouchEndEii` | 1037/1004 | 66.6% | 97.8% | 77.7% | - |
| `_ZN7StarMap6updateEi` | 971/966 | 76.6% | 98.7% | 82.7% | - |
| `_ZN7StarMap4drawEv` | 916/913 | 49.7% | 99.4% | 92.8% | - |
| `_ZN7StarMap4initEbP7Missionbi` | 750/735 | 72.9% | 98.3% | 77.3% | - |
| `_ZN7StarMap14initStarSystemEv` | 710/683 | 80.9% | 97.3% | 84.7% | - |
| `_ZN7StarMapC1EbP7Missionbi` | 333/331 | 85.1% | 99.5% | 97.6% | - |
| `_ZN7StarMap12OnTouchBeginEii` | 275/275 | 91.6% | 100.0% | 99.3% | - |
| `_ZN7StarMap11OnTouchMoveEii` | 265/258 | 81.3% | 98.8% | 78.8% | - |
| `_ZN7StarMap7drawKeyEv` | 246/244 | 59.5% | 97.9% | 35.1% | - |
| `_ZN7StarMap6departEb` | 205/205 | 94.9% | 100.0% | 88.3% | - |
| `_ZN7StarMapD1Ev` | 69/70 | 92.0% | 99.4% | 95.0% | - |
| `_ZN7StarMap6renderEv` | 47/47 | 95.7% | 100.0% | 100.0% | linked |
| `_ZN7StarMap10initLightsEv` | 21/21 | 87.8% | 100.0% | 100.0% | linked |
| `_ZN7StarMap24askForJumpIntoAlienWorldEv` | 21/21 | 97.1% | 100.0% | 100.0% | linked |
| `_ZN7StarMap8setStartEii` | 17/17 | 88.9% | 100.0% | 100.0% | linked |
| `_ZN7StarMap14isInPlanetModeEv` | 5/5 | 100.0% | 100.0% | 100.0% | byte |
| `_ZN7StarMap14setJumpMapModeEbb` | 3/3 | 86.7% | 100.0% | 66.7% | - |
| `_ZN7StarMap14missionChangedEv` | 2/2 | 90.0% | 100.0% | 100.0% | byte |
| `_ZN7StarMap8renderBGEv` | 1/1 | 100.0% | 100.0% | 100.0% | byte |

Aliases: `_ZN7StarMapC2EbP7Missionbi` = C1; `_ZN7StarMapD2Ev` = D1.


## Eightieth continuation: current-object remainder audit (2026-10-05)

Previous production/checkpoint: `fb5748ae`. No production edit or compiler
experiment was made in this continuation. The live ARM object SHA-256 is
`a775a751fc3520acc5f684ccd1d9560acd4da2664cbf01f471fd2c4055f7498f`,
matching the fully validated seventy-ninth acceptance. Tracked production was
clean before this audit.

Re-disassembled current and target StarMap objects with the existing NDK
objdump. Read-only use of the unchanged asmdiff reproduced **all eight source
submetrics for all 20 fixed bodies**, then the gate's fixed corpus calculation
reproduced **99.235 / 98.59119842275736 source-shape** and
**87.570 / 82.42509505703421 strict**. C2/D2 remain excluded from means; no
metric, original instruction weight or corpus change. Token multiset surplus
is diagnostic evidence, not a count of missing C++ statements or proof of a
compiler ceiling.

New raw-fragment checks exclude unsupported edits:

- End initial inertia: primary `cb0a2..cb112`, current `1f1a..1f8a` already
  have the same signed-absolute selections, strict `>3` comparisons, mode
  read, momentum/dragging/velocity stores. Inverting the float predicates or
  reordering source stores is not justified by this fragment.
- init campaign/mode: `c6e6e..c6e82` versus `4762..4776` already have the
  same compare15, conditional galaxy flag, default mode3/conditional mode0
  and member stores. Only the saved Status slot differs.
- Station initial ArraySetLength receivers: `c75b8..c75be`,
  `c7650..c765a`, `c7688..c7694` use the cached freshly constructed receivers.
  The constructor's accepted fresh-member reads must not be copied blindly
  into these calls.
- Station position allocation loop: `c7b08..c7b2e` versus `2efa..2f20`
  already holds the Array receiver across Vector allocation, zeros three
  components and stores the element through its data+4 field.
- System name table: primary `c7ac8` and current `2eba` both use LDRH with
  index shift2. Existing uint32 table slots preserve the native stride;
  changing the table to uint16 would alter it.

The remaining positive contributions to the weighted deficit relative to99
are End **0.18985pp**, station initialization **0.16998pp**, init **0.07393pp**,
update **0.04102pp**, key **0.03811pp**, Move **0.00746pp**. Other bodies offset
part of that deficit; these numbers are diagnostic contributions, not separate
required per-body thresholds. Prioritize unresolved warning dispatch and
station initialization operations rather than the five verified fragments.
Previously rejected warning and Matrix/Array ABI workarounds remain rejected.
No source-equivalence or ceiling claim follows from the inventory.

Evidence: `docs/validation/starmap_2026-10-05_remaining_audit.json` contains
all inventories, metrics, fixed weights, object hash and raw audit anchors.
No new native/full build was needed because production and the entire ARM
object are unchanged. The accepted seventy-ninth native library, 204-TU
focused check, zero-skip 4619-comparison corpus and no-exact-loss gates remain
the authoritative validations. Weighted99 gate remains red. Native executable
runtime equivalence remains untested. Goal remains active.


## Eighty-first/eighty-second trials: branch-local warning receivers (removed)

Accepted production is `fb5748ae`; preceding report checkpoint is `f1f0f794`.
Raw primary warning branches read ChoiceWindow this+92 separately before the
shared GameText getText: cb7ae/3214, cb7ba/532, cb7c6/3216, cb7f8/3213,
cb804/3217, cb852/3215, cb876/3218. Earlier common-goto warning trials did not
explicitly capture that receiver in each branch. This justifies testing the
source form, not claiming that the original C++ syntax was recovered.

Trial81 kept the accepted sentinel dispatch and captured the fresh member
receiver in each successful warning branch, using that receiver at common
warning delivery. All calls, predicates and order were retained. Focused ARM
build/comparison exited0: 204 TUs compiled, zero failures, 22 comparisons,
zero skips. **Entire focused JSON and ARM object were identical to accepted79**
(hash a775a751fc3520acc5f684ccd1d9560acd4da2664cbf01f471fd2c4055f7498f).
LLVM consolidated the branch-local receiver reads. No metric gain.

Trial82 combined those receiver captures with the previous separate3217 /
common-goto warning delivery form. This is distinct from57/67 at source level.
Focused build/comparison again exited0, compiled204/failed0, compared22/skip0.
End source-shape **97.7 ->97.6**, strict **80.0 ->81.3**, instruction inventory
96.8->97.0, opcode97.4->97.5, control98.1->97.8, call count99.3->97.8,
size96.7->97.9, current instructions1004->1016. Other21 rows were unchanged.
The End row equals rejected67 across every field. ARM object changed to
ca424edf938c226586cedf8ef91ef79c0cd9fe6afe2045a888a4eef4b9abde1a;
no full-corpus exact-pair preservation is claimed for this rejected object.

Both source forms were removed. Wrote the accepted source backup with a fresh
mtime and ran another focused build to terminal exit0: compiled204/failed0,
22 comparisons/zero skips. Normalized source, **entire focused JSON and entire
ARM object** match accepted79. Tracked production has no difference. The
accepted79 native static library, full4619 zero-skip report and baseline/previous
no-exact-loss gates remain authoritative; no redundant native/full run was
started. Means remain **99.235 /98.59120**; weighted99 remains red. Native
runtime equivalence remains unverified.

Evidence: before-warning-receivers-eightyfirst.cpp,
warning-receivers-eightyfirst.json/log,
warning-receivers-combined-eightysecond.cpp/json/log/o,
eightysecond-restored-focused.json/log under _work. Compact evidence:
`docs/validation/starmap_2026-10-05_warning_receiver_trials.json`.
Receiver captures alone are now excluded as a warning-dispatch improvement;
this is not proof that the remaining source-shape target is impossible.


## Eighty-third accepted Begin jump-distance receiver captures

Previous report checkpoint: `095fae50`; previous production: `fb5748ae`.
Inspected the live StarMap source/object, full primary Begin assembly and init
station-search assembly. Init fallback search c7296..c72cc already matches
4a42..4a78, including the post-element increment before comparison; no search
loop edit was justified.

A separate Begin fragment had a real load-order difference: primary
caa86..caa9c loads pathFinder+80 and systems+84 before Status getSystem/getIndex,
then reads selectedSystem+96 after the getters. Previous 4086..4098 called the
getters before reading the two pointer members. Now two source locals capture
those pointers before the getters, while selectedSystem remains fresh after
them. New 4086..409c matches the primary fragment's operations, registers,
field offsets and order; linked PLT/relocation addresses differ as expected.
No call/check was added or removed, no barrier/padding or compiler flag change.
Native load order is proven; original C++ spelling and runtime equivalence
are not claimed as recovered.

Begin source **99.3 ->99.6**, strict **97.4 ->98.5**, original/current
instructions275/273->275/275, inventory99.3->99.6, opcode99.3->99.6,
size99.3->100. Control98.5, ordered/control inventory98.5 and call count100
remain unchanged. Other21 focused rows are identical. Its remaining unsigned
visibility bound compares size,index / BLS in primary ca97e..ca980 and
index,size / BCS in new3f7e..3f80: equivalent conditions with reversed operands,
not a missing bounds check.

Fixed20/7101 source means **99.250 /98.60281650471764**; strict
**87.625 /82.4676946908886**. C2/D2 aliases checked and excluded. Focused ARM
build exited0: compiled204/failed0, compared22/skipped0. Native UCRT gof2
static library exited0: StarMap compiled and libgof2.a linked.

Full comparison session69269/process35420 was observed live and subsequently
returned terminal exit0; no restart. Frozen source/ARM objects were used.
Full4619 comparisons/zero skips, linked2083/byte960, source89.06/strict76.16,
fuzzy-perfect2203. Coverage unchanged:4274/4524, missing250(7wrong/243absent),
extra129. All22 focused/full rows agree. Baseline and previous accepted
exact-pair gates exited0, zero losses; default weighted99 gate exits1 as
required. Goal remains active, shortfall0.39718349528236pp weighted. Native
executable runtime equivalence remains untested.

ARM object SHA-256:
`041c36b7c1189d035a71cc89265c3c46b8ae3c082263260c4135abe11d7d92d4`.
Full JSON SHA-256:
`2fffeff58c1e4ef4f3af19647d2585a98a84df0b423f5e72838809b1ec490301`.
Evidence: _work/begin-original-eightythird.asm, current-eightythird.asm,
before-begin-distance-captures-eightythird.cpp,
begin-distance-captures-eightythird.json/log/asm, native-eightythird.log,
eightythird-corpus.json/log, eightythird-baseline/previous/target-gate.json/log,
checkpoint83-before-focused/corpus.json. Compact committed evidence:
`docs/validation/starmap_2026-10-05_begin_distance_captures.json`.


## Eighty-fourth accepted Begin visibility-bound operand order

Previous accepted production/report: `090af981`. Inspected current source and
accepted83 object, primary Begin ca97c..ca980 and current3f7c..3f80. The primary
uses load size R1, CMP R1,SL and BLS skip; accepted83 used CMP SL,R1 and BCS skip.
Change the source condition `i < vis->size()` to `vis->size() > i`. Both operands
are unsigned and the size accessor is inline: bounds, non-null guard,
short-circuit and visibility-byte read are preserved. New3f7c..3f80 matches the
primary registers and opcodes. No extra check/call, compiler flag, barrier or
padding. Original C++ syntax and runtime equivalence remain unproven.

Begin source **99.6 ->100.0**, strict **98.5 ->99.3**, count275/275 unchanged.
Instruction/opcode inventory99.6->100, control/ordered/control inventory
98.5->100, call count and size100 unchanged. Other21 focused rows identical.
Read-only disassembly through unchanged asmdiff independently reproduces all
eight source metrics at100. Strict sequence still moves two independent
instructions (selectedSystem store and scratchVector address calculation).
Thus source100 is metric evidence, not linked/byte exact or proven behavior.

Fixed20/7101 means **99.270 /98.6183072806647** source and
**87.665 /82.49867624278271** strict; C2/D2 checked/excluded. Focused build
exited0: compiled204/failed0,22 comparisons/zero skips. Native UCRT gof2 static
library exited0: StarMap compiled and libgof2.a linked. Full session98257/PID8704
was observed live and returned terminal exit0, without restart; source and ARM
objects stayed frozen. Full4619/skip0,2083 linked/960 byte exact,source89.06 /
strict76.16,perfect2203,coverage4274/4524,missing250(7wrong/243absent),extra129.
All22 focused/full rows agree. Baseline and previous exact-pair gates exited0,
no losses. Default99 gate exits1: weighted shortfall **0.3816927193353pp**.
Goal active; native executable runtime equivalence remains untested.

ARM object SHA-256:
`2836b8f80e9bce8c17dd7458b789291c80f037eaeef2ad972cbc21e20f3a6863`.
Full JSON SHA-256:
`a0d737dc5fc2260032c0849d162a72a74ec3f0986865a503be9a7ca81ed422ac`.
Evidence under _work: before-begin-visibility-bound-eightyfourth.cpp,
begin-visibility-bound-eightyfourth.json/log/asm,
begin-shape-check-eightyfourth.json, native-eightyfourth.log,
eightyfourth-corpus.json/log, eightyfourth-baseline/previous/target-gate.json/log,
checkpoint84-before-focused/corpus.json. Primary anchors use
begin-original-eightythird.asm. Compact committed evidence:
`docs/validation/starmap_2026-10-05_begin_visibility_bound.json`.


## Eighty-fifth trial: projection depth stores in branches (removed)

Accepted production/report: `1cf057a4`. Inspected live source and object plus
primary update. In both loops, native depth selection uses default-1 and
IT NE/select+1 (c9c0a..c9c12, c9cae..c9cb6); accepted84 uses default+1 and
IT EQ/select-1 (3232..323a,32d6..32de). Both implement zero->-1 and every
nonzero->+1. Previous30 tested a separate local depth default; this distinct
trial placed the fresh Array read and z store directly inside each if/else
branch, preserving exactly one read/store on every path. No extra runtime
call/check or compiler flag, barrier or padding.

Focused trial reached terminal exit0: compiled204/failed0,comparisons22/skips0.
**Entire focused JSON and entire ARM object are identical to accepted84**,
SHA2836b8f80e9bce8c17dd7458b789291c80f037eaeef2ad972cbc21e20f3a6863.
The compiler consolidated both stores to its existing IT EQ selection; no
metric gain. This result excludes this source form on the current baseline,
not proof of a compiler ceiling or recovery of original C++ syntax.

Removed the trial, wrote source backup with fresh mtime, and rebuilt focused
ARM to terminal exit0: compiled204/failed0,22 comparisons/skip0. Normalized
source, entire focused JSON and ARM object match accepted84. No production
difference remains. Native/full builds were not repeated: accepted84 native
static library/full4619 zero-skip/exact-pair gates remain authoritative.
Means stay99.270/98.61831 source and87.665/82.49868 strict; weighted99 still
fails. Goal active; native executable runtime equivalence unverified.

Additional read-only checks: system projection c9be0..c9c22 /3208..324a already
holds Canvas before getPosition and rereads systemPositions after both
getPosition and GetScreenPosition. Projection mode dispatch c9bac..c9c2c /
31d4..3254 already reads mode before lastDt store, keeps the value on bypass
and rereads after callbacks. Neither fragment justifies a new snapshot edit.

Evidence: _work/current-eightyfifth.asm,
before-projection-depth-store-eightyfifth.cpp,
projection-depth-store-eightyfifth.json/log,
eightyfifth-restored-focused.json/log. Compact committed snapshot:
`docs/validation/starmap_2026-10-05_projection_depth_trial.json`.


## Eighty-sixth accepted system target flag materialization (2026-10-06)

Previous report 2d0b6d29, production 1cf057a4. The previous interrupted turn
only read source; no trial was applied. Inspected StarMap.cpp,
info-original-direct.asm c876c..c877a, live current-eightyfifth.asm and the
new focused object. Primary uses MOVS R1,0; CMP R0,-1; IT GT; MOVGT R1,1;
ORRS.W R0,R5,R1; BEQ fallback. Previous current used a sign-bit EOR and
ORRS R0,R5. Capture signed targetIndex after the same getters, start
bool targetMatch with missionTarget, then set it true for targetIndex>=0.
New1050..105e reproduces primary opcodes, registers, immediate and order.
Fallback addresses differ. No extra runtime calls/checks, barriers, padding,
flags or shared declarations. Same negative sentinel, boolean mapping and
callback order. Original C++ spelling and native runtime equivalence unproven.

Info strict **80.2 ->80.4**, source **99.1 unchanged**, count1207/1196 unchanged.
Instruction inventory98.4->98.5 and opcode99.1->99.2 improve, while control
inventory/flow99.1->98.9 and ordered control90.6->90.4 decrease. This is a
restored native fragment, not a source-score increase or exact whole body.
Unchanged asmdiff independently reproduced all eight new source metrics.
Other21 focused rows are identical. Fixed20/7101 means source remain
**99.270 /98.6183072806647**, strict **87.675 /82.53267145472469**.
C2/D2 checked/excluded. Weighted99 shortfall remains0.3816927193353pp.

Focused terminal exit0: compiled204/failed0,22 comparisons/skip0. Native
UCRT static gof2 build terminal exit0: StarMap compiled/libgof2.a linked.
Full session15764/PID32936 was observed live and returned terminal exit0,
without restart; source and ARM object stayed frozen. Full4619/skip0,
2083 linked/960 byte exact, source89.06/strict76.16, perfect2203,
coverage4274/4524, missing250(7wrong/243absent), extra129. All22 focused/full
metric rows agree; unit path separators are normalized only for this check.
Baseline and previous exact-pair gates exit0 with zero losses. Default99 gate
exits1 as required. Goal active; native executable behavior untested.

ARM object SHA-256:
`c64021892af31b2758afac98c6c90b82b72b7ced6168b83f630da3cb0494293c`.
Full JSON SHA-256:
`4ab44c08b7761e492db7c1914cc7aaf5fe72901a687158c50ed6f01e666a10fa`.
Evidence under _work: before-system-target-flag-eightysixth.cpp,
system-target-flag-eightysixth.json/log/asm, info-shape-check-eightysixth.json,
native-eightysixth.log, eightysixth-corpus.json/log,
eightysixth-baseline/previous/target-gate.json/log,
checkpoint86-before-focused/corpus.json. Compact committed evidence:
`docs/validation/starmap_2026-10-06_system_target_flag.json`.


## Eighty-seventh passenger continuation repeat (removed)

Accepted production/report437b3b7f. Inspected live StarMap.cpp/object and
primary cb464/cb466 vs current242c/242e: signed CMP9/BLE vs CMP10/BLT,
mathematically equivalent including negative counts. Repeated a count>9
continuation to the fresh mission94 test; preserves all callbacks and warning
fallthrough without extra checks/calls or arithmetic. The initial brief
mistakenly described this form as new: the full trial21 report at lines880ff
already records the same high-capacity continuation. This is a repeated
experiment on accepted86, not a new source-recovery hypothesis.

Trial session34199 terminal exit0: compiled204/failed0,22 comparisons/skip0.
Entire focused JSON and whole ARM object equal accepted86. Removed the label
and nesting; wrote accepted backup with fresh source mtime and rebuilt in
session49844 to terminal exit0,204/0,22/0. Normalized source equals HEAD,
entire focused JSON and whole object equal accepted86. Object SHA-256:
`c64021892af31b2758afac98c6c90b82b72b7ced6168b83f630da3cb0494293c`.
No production difference remains. Accepted86 native/full4619 zero-skip and
no-exact-loss gates remain authoritative; no redundant native/full run.
Source99.270/98.61831, strict87.675/82.53267 unchanged. Weighted99 unmet;
goal active, native executable behavior untested. No compiler ceiling proved.

Additional read-only raw audit: transition completion XYZ c9f68..ca040 /
358c..3664 already matches repeated GetMaxValue calls, member snapshots,
subtractions, positive-test negate/select, VCMPE/VMRS and BHI exit sequence.
Allocation-normalized FP difference alignment points to path-animation and
pitch-clamp scheduling, not missing completion predicates. That alignment is
not evidence of missing C++ statements or permission to add redundant checks.
Read full prior pitch/path trial descriptions before another experiment.

Evidence: _work/before-passenger-positive-fallback-eightyseventh.cpp,
passenger-positive-fallback-eightyseventh.json/log,
eightyseventh-restored-focused.json/log, update-fp-audit-eightyseventh.json.
Compact committed snapshot:
`docs/validation/starmap_2026-10-06_passenger_repeat.json`.


## Eighty-eighth accepted render blend argument repair (2026-10-06)

Previous report28e181da, production437b3b7f. Inspected current StarMap.cpp,
RenderEnums.h (read only), accepted86 object and a fresh primary render
extract from verified Android HD2.0.16 ARMv7 library c8058..c80e8. Primary
c80ae loads Canvas, c80b0 MOVS R1,#2, c80b2 calls SetBlendMode, then planetGeom
and starSystemRoot render. Current60 loaded0. Change the single StarMap.cpp
argument BlendMode_dummy to existing BlendMode_2, whose shared enum value is2.
No shared file modified. Numeric argument2 is recovered from raw primary;
original C++ spelling/enum name and meaning of mode2 are not claimed recovered.
This fixes a wrong native argument, not an allocation/scheduling hypothesis.
Same callbacks/order. No artificial calls, flags, barriers, padding or aliases.

Render source **99.6->100**, strict **97.9->100**, count47/47 unchanged,
becomes **linked-exact**, still not byte-exact. Unchanged asmdiff independently
reproduces all eight source metrics100, equal normalized47-instruction
sequence and linked_equal=True, bytes_equal=False. Other21 focused rows
identical. Fixed20/7101 source **99.290 /98.62095479509928**, strict
**87.780 /82.54657090550626**. C2/D2 checked/excluded. Weighted99 shortfall
**0.37904520490072pp**, still incomplete. Runtime visual effect unverified.

Focused session42117 terminal exit0:204 compiled/0 failed,22 comparisons/skip0.
Native UCRT gof2 static-library build terminal exit0: StarMap compiled and
libgof2.a linked. Full session91239/PID30172 observed live and terminal exit0,
without restart; source/ARM object stayed frozen. Full4619/skip0,2084 linked/
960 byte exact, perfect2204, source89.06/strict76.16,coverage4274/4524,
missing250(7wrong/243absent),extra129. All22 focused/full metric rows agree
(normalize unit path separators only). Baseline/previous preservation gates
exit0 with no lost exact pairs. New linked pair game/world/StarMap render:
baseline2079->2084, previous2083->2084. Byte pairs959->960 baseline and
960->960 previous. Default99 gate exits1 as required. Goal active; no native
executable/runtime test or Godot equivalence claim.

ARM object SHA-256:
`a6cd529383016bc46bcd25d1c94198ffce6674237a2807b96a320dd9a8046ca8`.
Full JSON SHA-256:
`4b6070d6d0bb3f16b4c587147717dcf887f1ad331214ec0d49cfd9d12294ccd8`.
Evidence under _work: render-original-eightyeighth.asm,
before-render-blend-eightyeighth.cpp,render-blend-eightyeighth.json/log/asm,
render-shape-check-eightyeighth.json,native-eightyeighth.log,
eightyeighth-corpus.json/log,eightyeighth-baseline/previous/target-gate.json/log,
checkpoint88-before-focused/corpus.json. Compact committed evidence:
`docs/validation/starmap_2026-10-06_render_blend.json`.


## Eighty-ninth read-only register-constant argument audit (2026-10-06)

Accepted production/report a4a3633f. Previous goal turn made a verified native
argument correction and render became linked-exact. Current tracked production
is clean and the entire ARM object still hashes to accepted88:
`a6cd529383016bc46bcd25d1c94198ffce6674237a2807b96a320dd9a8046ca8`.
No source experiment or production edit in this continuation.

Inspected StarMap.cpp, fresh primary c67a4..cba2c raw disassembly, accepted88
current object disassembly, drawkey-original-full-fiftyfifth.asm and full
trial32 history. DrawKey text IDs/layout reads match; its temporary lifetime
form already had a neutral trial and was not repeated. Init keyTextIds uses
32-bit LDR/index*4 in primary c730c, matching the current int table; no
16-bit table change justified.

A conservative read-only diagnostic parses all20 fixed body headers and
aligns direct call names. It tracks immediate/register moves in straight-line
regions, clears state on branches/IT, invalidates overwritten registers and
clobbers caller registers after calls. It aligns699 direct calls and finds231
pairs with constants known on both sides. It excludes stack/float arguments
and does not recover indirect callees. These counts are diagnostic coverage,
not an exhaustive ABI or behavior proof; repeated callee alignment can drift.
No verifier/tool/config modification or arithmetic/padding was introduced.

Two candidate differences are both at one misaligned SetColor site: primary
c8b48 has R1=255,R2=128,R3=0, while aligned current1552 is white on the
unselected-station path. Raw primary c8b38/BNEc8b3c and current1304/BNE1308
select selectedStation==index; matching current1314 passes255/128/0 and
continues to Station::getName, as primary. Thus both R2/R3 differences are
repeated-callee alignment artifacts. No confirmed unresolved immediate
argument mismatch remains from this diagnostic, and no color edit is justified.
Do not treat this narrow pass as proof of all call arguments.

Authoritative accepted88 ARM204/native static/full4619 zero skips and
baseline/previous exact-pair gates remain applicable: no source/object change.
Source99.290/98.62095 and strict87.780/82.54657 remain unchanged. Weighted99
still fails; goal active and native executable/runtime equivalence unverified.
Next argument inspection requires stack or floating-point evidence; this
straight-line integer-register diagnostic supplies no further edit candidate.

Evidence under _work: argument-audit-primary-eightyninth.asm,
call-argument-audit-eightyninth.json, render-blend-eightyeighth.asm.
Compact committed evidence:
`docs/validation/starmap_2026-10-06_argument_audit.json`.

## Ninetieth follow-up: native float32 angular parameters (2026-10-06)

Primary Android GOF2 HD2.0.16 ARMv7 evidence, with PC-relative literal reads
from ELF executable sections, exposes eight genuine constant-argument
mismatches that the prior register-immediate audit could not see. Only
StarMap.cpp numeric literals change; arithmetic and callback order remain.

| Rounded value before | Replacement literal | Primary float32 bits | Source uses |
| --- | --- | --- | --- |
| 3.1416f | 3.1415927f | 0x40490fdb | 5 |
| 6.2832f | 6.2831855f | 0x40c90fdb | 5 |
| 0.000015259f | 0.0000152587890625f | 0x37800000 | 7 |
| +/-0.3927f | +/-0.3926991f | 0x3ec90fdb / 0xbec90fdb | 2 |
| -0.098175f | -0.098174773f | 0xbdc90fdb | 3 |

Verified native call sites include MatrixSetRotation at c6dfe; background
rotations at c739c/c73d0/c7404; constructor marker rotation at c6b14; and
station root rotation at c7aa4. Negative root Z rotation in update/Move is
loaded at ca626/cac62 from literal words ca7bc/cae7c: both 0xbdc90fdb.
These numeric float32 parameters are recovered from primary machine code;
the original C++ spelling and exact-source formula recovery are not claimed.

The conservative stack/float diagnostic aligns 699 direct calls, compares
33 known stack pairs and 222 known register pairs. Before: eight genuine
float argument differences plus two repeated-SetColor alignment artifacts.
After: only the same two false alignments remain. Actual orange/white paths
were reviewed in the eighty-ninth audit. The diagnostic does not resolve all
arithmetic, conditional paths, indirect callees or stack offsets above 8;
it is not exhaustive ABI proof.

Focused validation: terminal exit0, all204 ARM TUs built, zero failures,
22 comparisons/zero skips. The entire focused JSON equals a4a3633f despite
the changed object, because these literal changes do not change the reported
normalized metrics. Source-shape remains 99.290% /98.6209547951%; strict
87.780% /82.5465709055%. This corrects parameters without claiming score gain.

Native StarMap compiled, but the first archive operation failed because A:
ran out of space. NTFS compression of this checkout's own _work preserved
file contents and freed about200MB. The same native build then successfully
created libgof2.a (terminal exit0); no production rollback was needed.

Full verifier session6891, live PID31632, terminated exit0 with source and ARM
objects frozen. All4619 comparisons, zero skips; all22 StarMap metric rows
agree with focused after separator normalization. Full JSON is identical to
accepted88: linked2084/byte960, no exact-pair loss against clean baseline or
previous a4a3633f. Baseline/previous preservation gates exit0; default99 gate
exit1. The 99% objective remains unmet; no native executable/runtime test.

ARM object SHA256: 7d9424ca64863cded41ccad7c4560a4c0fce34bf48c53135cd169ef303e33617.
Full JSON SHA256: 4b6070d6d0bb3f16b4c587147717dcf887f1ad331214ec0d49cfd9d12294ccd8.

Compact committed evidence: docs/validation/starmap_2026-10-06_angular_constants.json.
Raw evidence under _work: before-angular-literals-ninetieth.cpp,
stack-float-argument-audit-ninetieth-before/after.json,
rotation-r3-proof-ninetieth.json, current-ninetieth.asm,
argument-audit-primary-eightyninth.asm, angular-literals-ninetieth.json,
angular-literals-ninetieth-build.log, native-ninetieth.log,
native-ninetieth-retry.log, ninetieth-corpus.json/log,
ninetieth-baseline/previous/target-gate.json/log,
checkpoint90-before-focused/corpus.json. The earlier literal-pool summary
survives in angular-literals-ninetieth.log; its same-named JSON was overwritten
by the focused report, so no separate pre-edit pool JSON is claimed.

## Ninety-first follow-up: VFP audit and rejected Y-only step form (2026-10-06)

Read-only audit of all20 fixed bodies compares actual VFP PC-relative literal
loads in primary Android ARM and accepted b848db14. Every per-body multiset
of literal width, bits and occurrence count agrees (42 loads each).
This strengthens the angular-constant evidence without claiming equivalence
for computed arithmetic, immediate VMOV/scalar LDR values or native runtime.
Raw audit: _work/vfp-literal-audit-ninetyfirst.json.

A narrow trial changes only path-centering Y selection: compute yStep before
conditions, initialize sy=0, assign step on greater-than or else less-than.
X remains untouched. Native basis: ca6da division, ca6de compare, ca6e6 BGT,
ca6e8/ca6ea ITPL zero. Same result for finite inputs and unordered comparison;
this does not establish whole-function float-to-int/NaN safety. The previous
thirteenth merged and nested X/Y trials were reviewed; this is Y-only.

Focused session76642 terminated exit0:204 TUs built, zero failures,22 symbols,
zero skipped. Update source98.7 ->98.6; strict82.7 ->72.7; current instructions
966 ->958 (original971). Inventory97.0 ->97.5 and opcodes98.2 ->98.3 improve,
but control98.7 ->98.3, ordered control95.4 ->89.8 and size99.6 ->98.7 worsen.
Other21 focused rows are identical. Rejected; no production change retained.

Restored by fresh UTF8 source write and rebuilt in session23360, terminal exit0,
204/0/22/0. Entire focused JSON and entire ARM object match accepted90; source
matches b848db14. SHA256 remains
7d9424ca64863cded41ccad7c4560a4c0fce34bf48c53135cd169ef303e33617.
No redundant native/full rerun: accepted90 validations remain canonical.
Metrics remain99.290% /98.6209547951%; default99 gate remains red. No compiler
ceiling is proved and the goal remains active.

Compact evidence: docs/validation/starmap_2026-10-06_path_y_trial.json.
Raw evidence under _work: before-path-y-selection-ninetyfirst.cpp,
path-y-selection-ninetyfirst.json/log/asm,
ninetyfirst-restored-focused.json/log,vfp-literal-audit-ninetyfirst.json.

## Ninety-second follow-up: rejected target-owner flag (2026-10-06)

The target-station ownership condition in init was written as an explicit
Boolean: compare with borrowed playerStation, clear the flag for nullptr,
then delete only when owned. Native c6f7c..c6f82 compares the borrowed pointer
before the conditional null test. Unlike the twenty-seventh bitwise-AND
trial, this uses sequential flag stores, with the same two predicates/calls.

Session91670 terminated exit0:204 TUs, zero failures,22 comparisons/zero skips.
Entire focused JSON and complete trial object equal accepted90, SHA256
7d9424ca64863cded41ccad7c4560a4c0fce34bf48c53135cd169ef303e33617.
The compiler still chooses null-first tests. No edit retained. Fresh UTF8
restore and session37169 rebuild terminated exit0 (204/0/22/0), with complete
focused JSON restored. Source matches b848db14 (checked through the next
trial's pre-edit backup against Git). No separately sampled post-restore object
hash is claimed; the neutral trial object had already matched accepted90.
The next production candidate's native/full validations are independently run.

Compact evidence: docs/validation/starmap_2026-10-06_target_owner_trial.json.
Raw: _work/before-target-owner-flag-ninetysecond.cpp,
target-owner-flag-ninetysecond.json/log,ninetysecond-restored-focused.json/log.

## Ninety-third follow-up: separate native campaign-target branches (2026-10-06)

The station/system campaign target branches previously merged into one Boolean
OR/branch after the station-or-system calculation. Primary Android ARM has
separate station ORRS/BNE/fallback-B at c8736..c873a and a system flag/ORRS/BEQ
at c876c..c877a, both entering the same wanted-icon store. Source now branches
to the single icon store independently, then evaluates the same fallback
campaign/field80/target-minus1 conditions only when neither branch matched.
Only StarMap.cpp changes. Same predicates, getters, their ordering and icon
store; no additional callbacks, redundant conditions, padding, flags or ABI
workarounds. C++ labels/spelling are a reconstruction, not recovered source.

Independent read-only checks with unchanged asmdiff reproduce all8 source
submetrics. Two complete16-byte machine fragments equal primary bytes:
station c872c versus current1014 (SUB/CLZ/LSRS/ORRS/BNE/B), and system c876c
versus current1054 (materialized flag/CMP/IT/ORRS/BEQ). This is local fragment
byte equality; the entire Info body remains non-linked/non-byte-exact.

Info source99.1 ->99.3, strict80.4 ->80.6; count1207/1196 ->1207/1198.
Inventory98.5 ->98.7, opcodes99.2 ->99.3, control/ inventory98.9 ->99.1,
ordered control90.4 ->90.6, size99.1 ->99.2; calls100 unchanged.
All other21 focused rows are identical. Fixed20/7101 source99.300 /98.6549500070,
strict87.790 /82.5805661174; aliases C2/D2 checked and excluded from means.
Weighted99 shortfall0.345049993pp remains. No compiler ceiling is proved.

Focused session22126 terminal exit0: all204 ARM TUs compiled, zero failures,
22 comparisons/zero skips. Native static-library build terminates exit0.
Full session58824, observed live PID29992, terminates exit0 with source and
ARM objects frozen. All4619 comparisons, zero skips; all22 focused/full rows
agree after unit-separator normalization. Linked2084/byte960 pairs preserved
without losses against clean baseline and previous b848db14. Existing coverage
remains4274/4524,250 missing (7 wrong type/243 absent),129 extras. Baseline and
previous preservation gates exit0, default99 gate exit1. Runtime untested.

Object SHA256: d114978a2da759daf551e4c0c541c0798cb5e19ee2e928b7b42c90ccad3446a7.
Full JSON SHA256: 80581c6f944c2965ad667f28475fe9ad5d507682a217528fae7fcf47a4e15aed.
Source SHA256: 644d2cb0e955cb3ee4aa3c8ec26df5f3c38f425e37da9af9fcdd83e7cbebd372.

Compact evidence: docs/validation/starmap_2026-10-06_target_icon_branches.json.
Raw under _work: before-target-icon-branches-ninetythird.cpp,
target-icon-branches-ninetythird.json/log/asm,target-icon-proof-ninetythird.json,
target-icon-argument-audit-ninetythird.json/log,info-inventory-ninetythird.json,
native-ninetythird.log,ninetythird-corpus.json/log,
ninetythird-baseline/previous/target-gate.json/log,
checkpoint93-before-focused/corpus.json. The fresh Info inventory still has
one BGE-versus-BGT difference at mission120 (primary c8544/c8546 versus current
e30/e34); the prior twenty-fourth negative-enum goto trial was neutral, so it
was not repeated. Other token surpluses remain diagnostic, not permission for
extra reads/checks or proof of a compiler ceiling. Goal remains active.

## Ninety-fourth follow-up: common confirmation close exit (2026-10-06)

Only the result0 confirmation guard in OnTouchEnd changes. Previously two
separate early-return blocks cleared suppressNextClose for an already-set
flag and for a selected station equal to Status's current station. They now
use one short-circuit OR guard and one clear/return block. Same conditions,
member reads and getter order; no extra callback or predicate. The local
Station pointer was eliminated without changing its lookup argument.

Primary cAEdc clears field484 then cAEe0 returns; station equality at cAFec
jumps to that same clear. Candidate1ec0/1ec4 and203c now share the equivalent
exit. Native cAFc4 versus current2014 has identical first16 bytes of the
station-lookup header. Raw getIndex ->Status::getStation ->getIndex ordering
and captured first result in R6 match; same-station equality jumps to clear.
This is local instruction/CFG evidence, not whole-function runtime equivalence.
The fortieth flag-read-timing trial was reviewed; this edits guard topology,
not that already-correct load/store order.

Source97.7 ->97.8 improves; strict80.0 ->77.7 worsens and is explicitly retained
as a tradeoff for source/CFG alignment. Count1037/1004 unchanged. Inventory96.8
->97.0, opcodes97.4 ->97.5, control/ inventory98.1 ->98.7, ordered control84.1
->87.9; calls99.3 unchanged. Normalized size96.7 ->96.5 worsens despite unchanged
reported raw counts (the unchanged metric applies its own normalization).
All other21 focused rows are identical. Independent unchanged asmdiff reproduces
all8 source submetrics. Complete End remains non-linked/non-byte-exact.

Fixed20/7101 source99.305 /98.6695535840; strict87.675 /82.2446838473.
Aliases C2/D2 remain checked/excluded. Weighted99 shortfall0.330446416pp.
The known-argument audit still aligns699 direct calls,33 stack pairs and222
register pairs, with only the two previously reviewed SetColor alignment
artifacts; no new known argument discrepancy. This conservative diagnostic
does not recover all values or prove ABI/runtime equivalence.

Focused session66186 terminated exit0,204 ARM TUs/zero failures,22 comparisons/
zero skips. Native gof2 static-library build terminated exit0. Full session64815,
observed live PID33372, terminated exit0 with source and ARM objects frozen.
All4619 comparisons, zero skips; all22 focused/full rows agree after separator
normalization. Baseline and previous d1d0ccfe gates preserve all2084 linked and
960 byte exact pairs, zero losses, exit0. Default99 gate remains red, exit1.
The original20 bodies/7101 instructions, aliases and compiler profile are
unchanged. No runtime/native executable test; goal remains active.

ARM object SHA256: 2e91d58070c32a49baf16e9baaa179219178ed2da974a1ebe06860eabe64876b.
Full JSON SHA256: 83248e1a06667fbe6569fdc55185f0596ef6ba7ce31467360d90d1317d5ea666.
Source SHA256: 6bd55273e01d6866104d648ed446531b6748d3783024871b63008a106584f1ef.

Compact evidence: docs/validation/starmap_2026-10-06_confirm_close.json.
Raw under _work: before-confirm-clear-ninetyfourth.cpp,
confirm-clear-ninetyfourth.json/log/asm,confirm-clear-proof-ninetyfourth.json,
confirm-clear-argument-audit-ninetyfourth.json/log,native-ninetyfourth.log,
ninetyfourth-corpus.json/log,ninetyfourth-baseline/previous/target-gate.json/log,
checkpoint94-before-focused/corpus.json. The source/CFG improvement is kept
with strict and normalized-size regressions stated, without calling these
metrics behavior equivalence or declaring a compiler ceiling.

## Ninety-fifth follow-up: rejected station-array lifetime form (2026-10-06)

Primary `c76e0..c76e8` loads stationGeoms for the loop bound. The zero
iteration branches at c76f0 directly to c7914; nonzero processing refreshes
the Array at c790c after all callbacks. Tested the same lifetime in source:
capture during each bound, refresh after nonzero processing, then access
data/element for addChild. The failed-bound Array was also used for the exit
setVisible; accepted94 already performs that exit reuse, so it is not a
new recovery. This differs from prior29's neutral top-break loop spelling.

Focused204/0,22/0 exited0 (session87604). Entire report and complete ARM
object equal accepted94. Compiler canonicalizes both pointer lifetime forms.
Rejected; restored by fresh UTF8 write and focused204/0,22/0 exited0
(session87661). Entire restored report/object equal accepted94, source
SHA256 `6bd55273e01d6866104d648ed446531b6748d3783024871b63008a106584f1ef`,
object SHA256 `2e91d58070c32a49baf16e9baaa179219178ed2da974a1ebe06860eabe64876b`.
Source means remain99.305/98.66955358400226; strict87.675/82.24468384734544.
No production retained; accepted94 native/full remain canonical, no redundant
rerun after identical restoration. Fixed20/7101,7 linked/3byte unchanged;
weighted99 still unmet, no ceiling or runtime-equivalence claim.

Evidence: `docs/validation/starmap_2026-10-06_station_array_trial.json`.
Raw: `_work/before-station-loop-array-ninetyfifth.cpp`,
`station-loop-array-ninetyfifth.json/log`, `ninetyfifth-restored-focused.json/log`.

A separate read-only VFP immediate audit complements91's PC-relative-load
audit. Actual ELF halfwords were decoded to immediate8 and expanded to
float32 sign/exponent/fraction; disassembler values were independently
checked against those bits. All20 per-body unique bit sets match: native
29 versus current29 VMOV.f32 immediate sites. Only update
has different multiplicities: native repeats +/-0.25 on station-centering
branches while current shares them; current repeats -30 on path selection
while native shares it. This is not a newly recovered numeric correction.
Prior27/35/91 branch/step trials cover those differences; no new trial or
production change. General-register constants/scalar loads/derived values
and runtime ABI are outside this audit, and no score ceiling follows.
Evidence: `docs/validation/starmap_2026-10-06_float_immediates.json`.

## Ninety-sixth follow-up: native current-route depth snapshot (2026-10-06)

Only draw's second route loop changes. Primary c93bc..c93dc computes a
zero/one both-behind result immediately after scratchVector2 assignment,
before the currentRoutes-null test at c93f6 and the flag test at c93fa.
Accepted94 instead tests currentRoutes at4e2/4e4 before depth at4e6..500.
The new source computes bool bothBehind=false, assigns the second ordered
negative-depth predicate only when the first depth is ordered negative, then
uses the existing currentRoutes && !bothBehind && !jumpMapModeB guard.
Same depth predicates, short circuit between depth reads, routes and jump-map
conditions, calls and stores. Depth evaluation now precedes the route-pointer
test, as primary. No additional condition, callback, padding, register forcing,
barrier, volatile, shared header, compiler flag or verifier change.
Native pointer/depth ordering and actual intermediate are reconstructed;
original C++ spelling and whole-function FP exception/ABI/runtime equivalence
are not claimed. Unordered less-than remains false; this does not prove
end-to-end float-to-int/NaN safety.

A first direct operand reorder was rejected: draw source99.1->99.0, strict
89.7->90.0, raw916/907->910. The compiler duplicates a route-null guard and
moves one before the second depth compare. Focus204/0,22/0 exited0
(session2379). Object235b2e55c706ad160714f58e8ec0191ed30b34a3ca61af5700f147b8398ad76e.
Replaced with the depth snapshot, not retained alongside it.

Snapshot focus204/0,22/0 exited0(session83026). Draw source99.1->99.4,
strict89.7->92.8, raw916/907->913. Inventory98.9->99.2, opcode99.1->99.3,
control98.6->98.9, ordered97.5->98.9, size99.0->99.6; calls100 unchanged.
All other21 rows identical. Read-only unchanged asmdiff independently agrees.
Draw remains non-linked/non-byte-exact. Fixed20/7101, aliases checked but
excluded; means source99.32/98.7082523588227, strict87.83/82.64457118715673.
Seven unique linked/three byte preserved; weighted99 remains0.2917476411773
percentage points short. No compiler ceiling claim.

Actual ELF executable bytes c93bc..c93dc and4cc..4ec match for34bytes:
94ed200ab5eec00af1ee10fa14d594ed230a0020b5eec00af1ee10fa48bf01200be0.
This is the complete local depth flag block including BPL/ITMI selection,
not whole draw equality. Conservative call-argument audit:699 aligned calls,
33 known stack pairs,222 register pairs; no new known mismatch. The two
already reviewed Info SetColor false path alignments remain; full Info row
unchanged. Unresolved arithmetic/conditional/indirect calls are not proved.
Native UCRT gof2 compiled StarMap.cpp and linked the static library, exit0;
no native executable/runtime test.

Scope correction for prior45: its report describes a neutral first-route bool
experiment but cites c93c8..c93dc, which actually belongs to the current-route
second loop. Only its pretrial source/report/hash were saved, not the temporary
trial source. Do not infer which loop that removed trial modified from the
mislabelled address. Current96 source and independent raw block proof are
complete and authoritative for this retained second-loop change.

Source SHA256:cf1300c403b2e61ed58faa7e1e9e1b207dde1c84861477b0dc29862ad1e7b3a3.
ARM object SHA256:39cdb475d869112b63837bd97ac2b6d260d312c988705c9b6cf9c4b69e92735a.

Evidence: docs/validation/starmap_2026-10-06_current_route_depth.json.
Raw _work:before-current-route-depth-first-ninetysixth.cpp,
current-route-depth-first-ninetysixth.json/log/asm,
current-route-depth-bool-ninetysixth.json/log/asm,
current-route-depth-proof-ninetysixth.json,
current-route-argument-audit-ninetysixth.json/log,
native-ninetysixth.log,ninetysixth-corpus.json/log,
ninetysixth-baseline/previous/target-gate.json/log,
checkpoint96-before-focused/corpus.json.

Full4619/0 exited0(session5341); all22 focused/full rows agree after
separator normalization for comparison only. Full2084 linked/960byte,
perfect2204,source89.06/strict76.16; coverage4274/4524,missing250
(7wrong-type/243absent),extras129 unchanged. Source and ARM-object hashes
remained frozen through comparison. Baseline and previous94 preservation
gates exit0 with no lost pairs; default99 gate exits1 for the unmet weighted
threshold. Canonical focused/full/gate updated only after these validations.
Full JSON SHA256:ac36671ad80d872e4cd8053180fea46d76c6924b13ef1ae919d43048d9c95d08.
Goal remains active.

## Ninety-seventh follow-up: read-only receiver/branch checks (2026-10-06)

No production edit or compiler trial. Inspected StarMap.cpp, accepted96
current-route-depth-bool-ninetysixth.asm, primary argument-audit-primary-
eightyninth.asm, and prior38's freelance-store report before considering
changes. Freelance system capture precedes getType (native c8816/c881c,
current1108/110e); cargo115/1 and signed enumeration branch agree. Prior38
already merged the marker store. Actual executable ELF bytes for station
equality, shared marker store and following mode guard agree for20bytes:
native c885a..c886c/current114c..115e. This local block equality is not
whole Info linked/byte equality and is not a new score gain.

Init saves Canvas before CameraGetCurrent and uses the same receiver for
CameraGetLocal (native c6f8e/c6f94/c6f9c; current47fa/4800/4808). It reads
fresh Canvas after MatrixSetTranslation (c6fc6/current4832). Station-mode
init likewise saves Canvas before the two camera getters (c702e/c7034/c703c,
current489a/48a0/48a8), then reads fresh Canvas before CameraSetLocal
(c7054/current48c0). Both matrix copies use60bytes. Route visibility bounds
also already agree on operand and short-circuit order: nativec925e/c9260
vs36e/370, and nativec938a/c938c,c9392/c9394 vs49a/49c,4a2/4a4.
No candidate mismatch was established in these inspected blocks.

Source and ARM object SHA256 still equal accepted96. Report-only work reuses
its focused/native/full validations; no new build/corpus run is claimed.
Metrics remain source99.32/98.7082523588227; weighted99 remains unmet.
This excludes these particular edits, not all unknown call effects, runtime
ABI issues, alternative source forms or future score improvements.
Evidence: docs/validation/starmap_2026-10-06_receiver_flow_audit.json.
Goal remains active.

## Ninety-eighth follow-up: transition zero-guard trial (removed)

Current tracked checkpoint bdbcab1d was clean before the trial; production
remains accepted96 b67a5300. Inspected StarMap.cpp draw and primary/current
ARM plus prior67/77/97 evidence. Draw's phase comparison c952c/c9534 already
agrees with current634/63c, including unordered skip; completed-edge loop
bounds also agree (c9a08..c9a18/currentb14..b24). No edits to those blocks.

Initial transition guard differs in form: primary c9120..c912c uses ITT EQ
with conditional transitionOut load/compare; accepted96 22e..23c uses BNE
then load/compare. Trial used transitionIn==0 && transitionOut==0 goto
draw_alpha_ready, followed by the original transition body in its own scope.
Preserved short-circuit reads, alpha stores, callback paths and later fresh
reads. No new callback, condition, padding, register/barrier/volatile forcing,
flag, shared header or verifier changes. This direct zero-guard continuation
is distinct from prior77's uint8_t fallback form; both compile neutrally.

Trial exit0(session19609):204 compiled/0 failed,22 comparisons/0 skips.
Entire focused JSON and whole ARM object equal accepted96. Trial source hash
2090faeee195eb1d56871b99ddf207a5f44db746f0028e247a54ff4b9d5644c4.
Removed trial with fresh UTF8 write; focused restore exit0(session4217),
204/0,22/0. Entire JSON and whole object again identical to accepted96;
source SHA256cf1300c403b2e61ed58faa7e1e9e1b207dde1c84861477b0dc29862ad1e7b3a3;
object SHA25639cdb475d869112b63837bd97ac2b6d260d312c988705c9b6cf9c4b69e92735a.

No production change retained. Accepted96 native/full validations remain
authoritative; no redundant rerun or new native runtime test is claimed.
Fixed20/7101 source99.32/98.7082523588227 remains unchanged, seven unique
linked/three byte preserved; weighted99 unmet. No compiler ceiling proved.
Evidence: docs/validation/starmap_2026-10-06_transition_zero_trial.json and
_work/before-transition-zero-goto-ninetyeighth.cpp,
transition-zero-goto-ninetyeighth.json/log,ninetyeighth-restored-focused.json/log.
Goal remains active.

## Ninety-ninth follow-up: refreshed remaining inventory (read-only)

Tracked checkpoint322d99e8 is clean. Recomputed all20 current source metrics
and allocation-normalized inventory surpluses using unchanged asmdiff and
current delinked target/accepted96 ARM object. Every metric agrees with the
canonical focused report. Fixed20/7101, C2/D2 excluded; means99.32/98.7082523588227,
weighted shortfall0.2917476411773pp. Deficits relative to99 by body: End0.17524pp,
Station0.16998pp, init0.07393pp, update0.04102pp, Key0.03811pp, Move0.00746pp.
Bodies above99 offset some of this deficit. These are aggregate token counts,
not source statements to add and not a compiler ceiling. Prior80 inventory
was stale for several later changes; refreshed evidence uses accepted96.

Checked End transition-in easing range calls: primarycb772..cb79c and
current274e..2778 both retain one SetRange address across three indirect
calls, loading fresh ease receiver and scratch components per axis. No
function-pointer snapshot repair is warranted. Matrix residuals still
include native inline initialization versus current C1 calls at46e6/2b3c.
Read Matrix.h/Matrix.cpp only: default ctor is the sole declared ctor.
Actual native tail literal c7468 contains [1,0,1,1] at matrix offset40,
confirming existing m10/m13 fixes rather than a new constant mismatch.
Shared header/code, raw-storage, shadow-constructor or compiler changes
remain outside this task; token counts do not authorize them.

No production edit or compiler trial; source/object hashes remain accepted96.
Accepted96 native/full and restored98 focused validations remain canonical;
no new runtime proof. Updated ranking prioritizes real End branch/receiver
discrepancies; do not repeat rejected warning receivers,3217 grouping,
passenger bounds or transition guard forms merely because tokens remain.
Evidence: docs/validation/starmap_2026-10-06_remaining_inventory.json; raw
_work/all-body-inventories-ninetyninth.json. Goal remains active.

## Hundredth follow-up: proven data classification sensitivity (read-only)

No verifier or production change. Actual ELF matrix tails c7468/c7d48 are
16bytes each,0000803f000000000000803f0000803f, floats[1,0,1,1]. Primary
c6db2 ADDW -> c6dbc VLD1.64 and c76c8 ADDW -> c76d0 VLD1.64 prove these
are constant data. Stripped target entries decode each tail as5 MOVS/3 SUBS;
these match the entire such surplus in both init and Station inventories.
Do not add eight corresponding C++ operations to chase these data tokens.

Targeted in-memory diagnostic reclassifies only these two16-byte tails and
End's40-byte literal island cb360..cb388, then uses unchanged normalizer and
metric functions. Every End word has a real incoming PC load outside the
island; discard cb204's apparent reference because cb204 is itself data.
Prior31 already proved false origins cb364/cb374/cb37c can erase real calls.
This diagnostic restores those call tokens by excluding data as origins.
It is neither a general normalizer repair nor an official score change.

Diagnostic End97.8->97.7 (tokens1037->1041), init98.3->98.5 (750->745),
Station97.3->97.5 (710->706). Using fixed weights7101 gives diagnostic
99.335/98.73476975073933, still below weighted99. Thus even this data-only
classification adjustment is insufficient. Official20/7101 remains
99.32/98.7082523588227; official reports/gates untouched. Not a compiler
ceiling, permission for verifier edits, padding, or completion evidence.
Evidence: docs/validation/starmap_2026-10-06_proven_data_sensitivity.json;
raw _work/proven-data-sensitivity-hundredth.json and
end-real-pool-references-hundredth.json.

## Hundred-and-first follow-up: modular yaw arithmetic trials (removed)

Inspected End confirmation/no-confirmation paths and update's integer
yaw-distance sequence. End alien-jump guards and cached jump-map byte already
agree (caff0..cb01c/current204c..2078); no new End edit. Native update
ca1f6 RSB,ca210 SUBS,ca214/ca216 MI conditional NEG operate on32-bit values.
TrialA uses uint32 subtraction/negation plus int32 conversion for yawDistance;
trialB also applies this to target=0x8000-stationAngle and both yawStep
subtractions. No extra branch/call/barrier, forced register, padding, flags,
shared header or verifier edit. Distinct from prior37's abs(yaw-target).

Both exit0(A session78049,B79536),204 compiled/0 failed,22 comparisons/0
skips. Entire focused JSON and whole ARM object remain equal to accepted96.
Removed both forms: ordinary yaw[-65535,65535]/stationAngle[0,65535] implies
target[-32767,32768] and difference[-98303,98302], so old signed arithmetic
already fits in that range. Unsigned-to-int32 conversion is implementation-
defined C++14, supported by intended two-complement targets; neither trial
proves float-to-int, pitch/dt overflow or whole-function runtime safety.
No normal-range gameplay fix or recovered original C++ spelling is claimed.

Fresh source restore exit0(session46965),204/0,22/0; entire focused report and
whole object equal accepted96. Source hashcf1300c403b2e61ed58faa7e1e9e1b207dde1c84861477b0dc29862ad1e7b3a3,
object39cdb475d869112b63837bd97ac2b6d260d312c988705c9b6cf9c4b69e92735a.
Accepted96 native/full remains canonical; no redundant rerun or new runtime
claim. Source means99.32/98.7082523588227 unchanged; weighted99 unmet.
Evidence: docs/validation/starmap_2026-10-06_yaw_wrap_trials.json; raw
_work/before-yaw-wrap-hundredfirst.cpp,yaw-distance-wrap-hundredfirst.cpp,
yaw-full-wrap-hundredfirst.cpp,yaw-wrap-hundredfirst.json/log,
yaw-full-wrap-hundredfirst.json/log,hundredfirst-restored-focused.json/log.
No compiler ceiling proved. Goal remains active.


## Hundred-and-second follow-up: clamp, viewport and help audit

Read StarMap.cpp plus accepted96/current and primary ARM assembly. Move pitch
clamp retains two ordered comparisons/three selectors versus native four
comparisons/three selectors (cac74..cacd4/current4f18..4f62). Inspected selected
values agree, including unordered input selecting -8192; no redundant predicate
added for score. This does not prove equal floating-point exception counts.

Info viewport c839c..c83f8/currentc98..cf4 already reads X before width and Y
only after X acceptance, with matching ordered BMI/BGT lower/upper tests.
Prior22 repaired this ordering. End help cb902/current28a6 already captures
Layout before GameText and String calls and uses that saved receiver afterward.
Actual ELF bytes cb90c/current28b0 match for six bytes of IT EQ/conditional
text628 selection; local proof only, not whole End equality.

No production edit, compiler trial or verifier change. Source and whole ARM
object hashes remain accepted96; canonical focused/native-static/full results
are unchanged. Means99.32/98.7082523588227 on20/7101; weighted99 remains unmet.
No executable/runtime or compiler-ceiling claim. Evidence:
docs/validation/starmap_2026-10-06_clamp_viewport_help_audit.json.


## Hundred-and-third follow-up: reverse transition and spin audit

End reverse-transition caf40..caf9e/current20ec..214a already preserves saved
axis receivers, fresh minimum receivers and reuse of the three indirect getter/
setter addresses. The 52 ELF bytes caf6c/current2118 (Y/Z receiver and argument
sequence) match exactly; this excludes GOT setup and is not whole End proof.
Update galaxy spin ca3a8..ca3fa/current3a48..3a9a and station spin
ca56c..ca5c6/current3ba0..3bfa agree in GT absolute selection, MI speed<10,
addition and GT spin>100 zero selection; station multiplication precedes cap.
Unordered speed selects10 and unordered spin remains unordered in both.
No new receiver or min/max spelling trial justified for these blocks.
No production/verifier edits or new compile. Source/object equal accepted96;
canonical checks preserved, means99.32/98.7082523588227 unchanged, weighted99
unmet. Native executable/runtime not validated; no compiler ceiling proved.
Evidence: docs/validation/starmap_2026-10-06_transition_spin_audit.json.


## Hundred-and-fourth follow-up: automatic selection store order

Inspected init station cleanup and native c70c8..c70d6. Cleanup already agrees:
initial stations receiver held for ArrayReleaseClasses, fresh member read for
destruction, null store before new allocation. No cleanup edit justified.
The following param3 conditional block instead had selectedSystem written
before lastSelectedSystem in current496c/4970, opposite nativec70d2/c70d6.
Reverse these independent source statements. Emitted18-byte block4962..4970
now matches nativec70c8..c70d6 exactly. There is no intervening call; values and
conditions are unchanged. This is emitted-order restoration, not proof of
original C++ spelling or whole-function/runtime equivalence.

Compared7017 old .text assembly entries with actual new ELF bytes: only the
four-byte stores at496c and4970 changed (swapped cbf86000/cbf89c01). The other
7015 entries are unchanged. Focus exit0(session29188),204 compiled/0 failed,
22 comparisons/0 skips: init strict77.3->77.4, source98.3 unchanged; all other
21 rows identical. Source means99.32/98.7082523588227 unchanged; strict means
87.835/82.65513307984791 (fixed20/7101). Source/object hashes in compact evidence.

Native STATIC gof2 exit0, StarMap compiled and libgof2.a linked. Full exit0
(session64351,PID11176):4619 comparisons/0 skips,2084 linked/960 byte exact,
2204 fuzzy perfect,4274/4524 coverage,250 missing(7 wrong type/243 absent),
129 extras. All22 focus/full metrics agree. Baseline and previous accepted96
preservation gates exit0 with no lost pairs. Threshold gate exit1 solely
because weighted99 remains unmet. Canonical reports refreshed. No native
executable/runtime validation; no compiler ceiling claim. Goal remains active.
Evidence: docs/validation/starmap_2026-10-06_auto_selection_stores.json.
Raw _work/before-auto-selection-stores-hundredfourth.cpp,
auto-selection-stores-hundredfourth.json/log/asm,
auto-selection-byte-diff-hundredfourth.json,native-hundredfourth.log,
hundredfourth-corpus.json/log,hundredfourth-{baseline,previous,target}-gate.json/log.


## Hundred-and-fifth follow-up: centering delta/read-order trials (removed)

Update camera-boundary integer predicates already agree. Native centering
ca444..ca47c reads height, then Y/X and width conversion, computes Y/X deltas
before X comparison. Proved globals using actual ELF PC literals plus dynamic
relocations: ca7c4=145e34+ca44c=210280(Globals::h),
ca7c8=145e2a+ca44e=210278(Globals::w). Width/height meanings are correct;
the discrepancy is ordering, not an axis swap.

TrialA reorders these source reads and preserves deltaY/deltaX locals before
X branching, using locals for divisions only; later overshoot member reads
are unchanged. This is a delta-only experiment, distinct from prior35's early
step-selection and91's Y step selector. Exit0(session61743),204/0,22/0;
update source98.7->98.5, strict82.7->79.8, raw966->961, control98.7->98.0,
ordered95.4->91.4,size99.6->99.1; other21 rows identical. Rejected.
TrialB changes read order only. Exit0(session84128),204/0,22/0; entire report
and whole ARM object equal accepted104. Neutral and removed.
Fresh restore exit0(session28003),204/0,22/0; source and whole object hashes
and entire focused report equal accepted104. Canonical native-static/full
checks preserved; no redundant rerun or executable/runtime claim.
Source99.32/98.7082523588227 unchanged; weighted99 unmet. No ceiling proved.
Evidence: docs/validation/starmap_2026-10-06_centering_delta_trials.json.
Raw _work/before-centering-deltas-hundredfifth.cpp,
centering-deltas-hundredfifth.cpp/json/log,
centering-read-order-hundredfifth.cpp/json/log,
hundredfifth-restored-focused.json/log. Goal remains active.


## Hundred-and-sixth follow-up: Key temporary scheduling review

Read StarMap.cpp, Layout.h, AEString.h and native/current drawKey bodies.
Nativec8178 drawBox, c817c..c8192 saved-coordinate arithmetic, c8196 StringD1;
current1a6a drawBox,1a70 StringD1, then coordinate arithmetic before first
DrawImage2D. All relevant values were captured before String construction.
Remaining difference here is pure arithmetic scheduling across destruction;
normal UI-coordinate formulas agree, not a whole-function/overflow proof.
Do not repeat prior32 neutral comma-expression extension or prior55 worsened
flat textX spelling. Named-string lifetime proposal is also unsuitable:
Layout takes String by value; declared copy constructor calls Set(other.data),
virtual destruction adds a lifetime, and no move constructor exists. That
would add copying beyond the original single temporary. No such edit/trial.
No forced scheduling/barrier/padding/shared header change. Source and whole
ARM object equal104; canonical validation preserved. Current96 drawKey dump
still valid after104 (only496c/4970 changed);105 restored exact104 object.
Source99.32/98.7082523588227 unchanged, weighted99 unmet. No runtime or compiler
ceiling proof. Evidence: docs/validation/starmap_2026-10-06_key_temporary_audit.json.


## Hundred-and-seventh follow-up: concrete shared Matrix proposal (not applied)

Remaining Matrix difference is inline native initialization versus accepted
StarMap calling the shared constructor. Prepared a reversible patch moving
its unchanged body from Matrix.cpp to Matrix.h; no field/layout/StarMap-local
fix or verifier/flag change. Shared files remain untouched. No compile or
predicted improvement claimed. This exceeds agreed task scope, so requested
explicit permission before applying. Pending question is not authorization.
MatrixC2 itself is not exact in either baseline/current report (both duplicated
unit entries73.5 source/43.2 strict). This does not prove callers preserve
exactness or exported symbols survive. Proposed acceptance requires focus,
native STATIC/full corpus, baseline/previous exact-pair preservation and
symbol/coverage inspection, with rejection on loss or metric regression.
Source/object still accepted104;99.32/98.7082523588227, weighted99 unmet.
No runtime/ceiling claim. Goal active; waiting for shared-scope decision.
Evidence/proposal: docs/validation/starmap_2026-10-06_matrix_inline_proposal.json
and .patch. No shared production files modified.


## Hundred-and-eighth follow-up: shared constructor footprint (unapplied)

Previous turn prepared a concrete shared Matrix proposal; no human scope
approval has arrived. Automated goal continuation is not that approval.
Read-only ARM nm -A -u scan of204 current base objects finds MatrixC1 references
in33 units, including Level,Gun,PlayerTurret,TargetFollowCamera. Every one of
those units has linked-exact rows;31 have byte-exact rows (552 linked/235 byte
rows across those units, not distinct function/call-site proof). Unit paths
normalized for repeated separators only for this comparison. A reference in
an object plus exact rows in the same unit is a potential preservation surface,
not evidence those exact functions call Matrix or that the proposal regresses.
Full corpus preservation remains mandatory; no prediction of improved score.
StarMap source/object and both shared Matrix files still match107 recorded
hashes. No production change, build, test rerun or native runtime claim.
Goal active, weighted98.7082523588227 <99. Awaiting explicit shared-scope decision;
proposal remains unapplied. Evidence:
docs/validation/starmap_2026-10-06_matrix_ctor_footprint.json.

## Hundred-and-tenth follow-up: authorized shared Matrix trials rejected

Human authorization on2026-10-06: "доделывай разрешаю". Tested the concrete
shared-constructor proposal in engine/math/Matrix.h and both engine/math and
game/core Matrix.cpp implementations. The first ARM/native attempt exposed
the second out-of-line definition in game/core/Matrix.cpp (203 compiled/1
failed; native exit1). Removed that identical duplicate for the trial, then
ARM compiled204/failed0 and native STATIC linked successfully. Verifier exit0
alone was not treated as a clean compile. No StarMap/field-value/layout/flag/
normalizer change or forced inlining was used.

Ordinary in-class assignment initialization improved init source98.3->98.9
and initStarSystem97.3->98.3; the other20 focused rows stayed unchanged.
Fixed20-body means became99.4 unweighted/98.87160963244614 weighted source,
87.75/82.4868469229686 strict. Weighted99 still failed. Full4618/0 compared
rows retained4274/4524 unique coverage and960 byte-exact pairs, but lost TWO
previously linked-exact pairs: AEMath::MatrixGetInverse and CameraC2. Linked
count2084->2082;40 existing rows regressed in source shape. Baseline exact-
preservation gate exit1. Two duplicate MatrixC2 comparison rows disappeared;
a read-only scan of204 base objects still found one weak MatrixC2 definition
in Level, so the unique constructor coverage did not disappear. The proposal
was rejected, rather than accepting a StarMap gain with wider regressions.

Restored all three shared files byte-for-byte and rebuilt: ARM204/0 and
native STATIC exit0. Focused report and ALL4619 full function rows equal the
previous accepted104 reports, skipped0, linked2084/byte960, coverage4274/4524,
missing250 (7 wrong type/243 absent), extras129. Both baseline and previous
preservation gates exit0 with --allow-partial; that option does not relax
exact-pair preservation. Canonical final reports remain accepted104.

Also tested an ordinary array member initializer with the SAME15 values,
removing both out-of-line definitions. ARM204/0; source means remained
99.32/98.7082523588227, strict fell to87.755/82.49402900999858. Rejected at
the focused gate because it supplies no source improvement. No full or native
build was claimed for this rejected alternate. Restored/rebuilt once more;
ALL final204 ARM object hashes equal the fully validated restored set. Final
focused report equals accepted104; native STATIC rebuilt with exit0. The full
report therefore still describes every underlying byte-identical object;
unchanged full comparisons were not rerun. Default99 threshold gate exit1,
weighted threshold only; both preservation gates remain green.

New read-only primary evidence distinguishes the standalone constructor from
previous local StarMap-tail evidence. Dynamic weak MatrixC2 at0x6e8d8, size0x40:
ADR0x6e8d8 addresses0x6e908, VLD1 loads16 bytes, ADD at0x6e8fa selects offset40,
VST1 at0x6e8fe stores to m10..m13. Literal bytes
0000803f000000000000803f0000803f prove those four values are1,0,1,1. Thus the
original standalone constructor also has m10=1/m13=1, whereas current shared
defaults are0/0. The approved unchanged-value experiment did not apply a
shared behavior fix; existing local StarMap repairs already set both to1.
This recovers data/store behavior, not original C++ spelling. Native STATIC
checks do not prove runtime or whole ABI correctness.

The authorized experiment is finished and rejected. Production stays104,
99.32/98.7082523588227 source,87.835/82.6551330798479 strict,7 linked/3 byte
unique StarMap bodies. The overall99 objective remains unmet; this is not a
proof that99 is impossible. Evidence:
docs/validation/starmap_2026-10-06_matrix_inline_trial.json.

## Hundred-and-eleventh follow-up: explicit warning state (neutral, removed)

Revalidated committed110 restoration before investigating OnTouchEnd. Previous
goal turn is progress: the authorized Matrix experiment supplied full-corpus
regression evidence and eliminated a concrete shared-header route. No running
job was assumed from old logs. Production remains104.

Tested a separate bool for seven successful warning selections, retaining the
ordered mission/Station/Ship predicates, numeric text IDs and single common
delivery. The two remaining mission142 guards and final delivery test used
the bool instead of signed warningText sentinel comparisons. This differs
from prior53's zero sentinel and81's captured receivers; it does not predicate
warning delivery on a potentially null pointer. It is a source-form heuristic,
not recovered C++ spelling. No calls, barriers, shared edits or flags added.

ARM204 compiled/0 failed,22 compared/0 skipped. ENTIRE focused report and
whole ARM object equal accepted104, SHA256
54a78574bcc14daefc8678850bb6a4e8bf3bed2caa2fa3b5139ef72c9e30484b.
Compiler output does not distinguish this explicit state from the sentinel.
Removed the neutral change. Fresh-mtime restoration reproduces ALL204 objects
from the fully validated110 restoration, canonical focused report and original
source hash; native STATIC rebuild exit0. The full110 report remains valid for
every byte-identical object; no unchanged full rerun or runtime claim.

Existing inertia IT GT predicates, ring float/int conversion and prior78
low-cargo read-order trial were checked to avoid repeating unsupported or
already neutral variants. No such edits made. Source99.32/98.7082523588227,
strict87.835/82.6551330798479,7 linked/3 byte unique bodies; weighted99 remains
unmet. Goal retains the full99 objective. Evidence:
docs/validation/starmap_2026-10-06_warning_bool_trial.json.

## Hundred-and-twelfth follow-up: primary playing-time ABI/division audit

Revalidated committed111 and unchanged production104. Previous goal turn is
progress: explicit warning-state compilation ruled out a new source form.
No live build/wait assumed. Read StarMap.cpp, Status.h/cpp, AERandom.h and
primary/current ARM fragments. No production edit or new build.

Final station seed transfers BOTH getPlayingTime return words into setSeed's
64-bit argument. Native c7cf8..c7cfd and current3102..3107 have EXACT6 bytes
024620460b46: r2=r0, r0=saved AERandom receiver r4, r3=r1. No narrowing or
extra sign extension. This was already noted in older source review; fresh
ELF bytes revalidate it, rather than treating it as a newly found bug.

New evidence addresses draw's stripped division destination. Native pulse
calls c9426/c942e and c9504/c950c target19ae54. The nearest-symbol label
__cxa_demangle+0x6e24 does not identify that helper's behavior. Directly
disassembled19ae54..19ae97: nonzero divisor path calls imported
__gnu_ldivmod_helper at19ae7c via69420 PLT; returned remainder is loaded into
r2/r3. Dynamic symbols contain this signed helper, without a matching
__gnu_uldivmod_helper import. Caller passes10/0 then99/0, uses64-bit
101-minus-remainder and signed __aeabi_l2f, as current __aeabi_ldivmod callers
already do. Keep signed int64_t playingTime arithmetic; an unsigned source
rewrite based on a stripped/misleading symbol label is unsupported.

Source and whole ARM object equal104. ALL204 base object hashes equal the
full-validated110 restored set; canonical4619/0 full report and111 native
STATIC evidence remain valid. No redundant tests rerun, runtime/source-
spelling/whole ABI claim or metric change. Source99.32/98.7082523588227,
weighted99 remains unmet; full objective retained. Evidence:
docs/validation/starmap_2026-10-06_playing_time_abi_audit.json;
raw _work/playing-time-division-wrapper-112.asm.

## Hundred-and-thirteenth follow-up: station Matrix basis/copy trial removed

Revalidated112 before inspecting station initialization. Previous goal turn
is progress: direct inspection of the hidden signed division wrapper excludes
an unsupported unsigned playing-time rewrite. Original used-flag clearing
c7698..c76ad and accepted2ad0..2ae5 already cache Array pointer/length and use
the same data load, byte store, increment and != loop; no clearing-loop edit.
The earlier Array<bool> trial lost an existing byte-array constructor exact
pair, so it was not repeated or concealed with a dummy instantiation.

Native station Matrix literals are loaded before iteration and stored into
each local rotation. Tested a normal C++ invariant Matrix basis with the SAME
15 defaults and existing m10/m13 repairs; each station created rotation by
ordinary implicit Matrix copy. No raw storage, shadow constructor, shared
edit, forced scheduling, flag/verifier change or invented field value. This
is a source-form hypothesis, not recovered original C++ spelling.

ARM204 compiled/0 failed,22 compared/0 skipped. Only Station changed:
source97.3->96.9, strict84.7->75.7, raw683->685, instruction inventory96.7->95.8,
opcode97.4->97.3, control96.9->96.6, call99->98, size96.3->96.6. Trial relocation
2b4e is an added __aeabi_memcpy; MatrixC1 is at2aec. The implicit copy does not
reproduce primary inline field initialization. Fixed20-body means fall to
99.3/98.66825799183215 source and87.385/81.7552598225602 strict. Rejected at
focus; no full/native validation is claimed for this rejected trial.

Restored the exact source backup and rebuilt. Restoration reproduces ALL204
full-validated object hashes and canonical focused report; native STATIC
rebuild completed with exit0. Fresh ELF extraction also proves EXACT22 clearing
loop bytes: d4f8000100220023016802e04668f25401339942fad1. The unchanged full
report remains valid; no redundant full rerun or runtime claim. Accepted production
remains104,99.32/98.7082523588227 source,87.835/82.6551330798479 strict. Overall
weighted99 remains unmet. Evidence:
docs/validation/starmap_2026-10-06_station_matrix_copy_trial.json.

## Hundred-and-fourteenth accepted branch-local warning delivery

Revalidated113 before changing OnTouchEnd. Previous turn is progress: ordinary
Matrix-copy trial was rejected with fresh compilation evidence and restoration;
the used-flag loop is now proven exact22 bytes. Reviewed46's common-goto and
57/67/82's separate3217 forms before selecting a distinct source representation.

Seven successful warning branches now deliver their existing warning directly,
set suppressNextClose and jump to the unchanged choice-visible/inertia/Help
continuation. Removed numeric sentinel and both later sentinel guards. Runtime
still executes ONE delivery after the same ordered predicates/getters, with
the same IDs532/3213/3214/3217/3215/3216/3218. Normal dialog and confirmation
paths are unchanged. No pointer-null control assumption, extra runtime call,
barrier, template padding, shared edit, flag or verifier change. Branch-local
source calls are legitimate alternatives, not proof of original C++ spelling.

ARM204 compiled/0 failed,22 compared/0 skipped. Only End row changes:
source97.8->98.6, strict77.7->80.3, raw1004->1036, instruction inventory97->98.3,
opcode97.5->98.8, size96.5->99.7. Control98.7->98.3 and call count99.3->97.8
worsen; source-shape and strict totals improve. Other21 rows including aliases
are identical. Fixed20 means99.36/98.82508097451064 source and
87.965/83.0348260808337 strict;7 linked/3 byte unique bodies unchanged.

Native STATIC exit0. Full4619 compared/0 skipped, linked2084/byte960, coverage
4274/4524, missing250 (7 wrong type/243 absent), extras129. ALL22 full/focus
rows agree. Baseline AND previous exact-pair gates exit0; zero losses in both
linked and byte pairs. Default99 gate exit1 solely because weighted98.82508
remains below99. Canonical final reports updated after those checks.

Formatted branch bodies and removed the experiment suffix from the continuation
label before committing. Fresh ARM204/0 and native STATIC exit0; the entire
focused report and ALL204 object hashes match the fully validated candidate,
so its full report remains valid. This is source-shape progress, not native
runtime/whole ABI proof or completion of99. Source/object hashes and raw gate
inputs are recorded in the compact evidence snapshot.
Evidence: docs/validation/starmap_2026-10-06_warning_branch_delivery.json.

## Hundred-and-fifteenth accepted warning suppression tail

Revalidated114 (dde477ce) source/object and committed evidence. Previous goal
turn is progress:114 improved source shape and passed full/native/baseline and
previous preservation gates. This continuation keeps the direct warning
deliveries and tests the placement of their suppression store.

Variant A: all seven successful deliveries jump to one suppression store,
followed by existing choice visibility/inertia/Help. Both ordinary station
dialogs jump past that store; warning predicates/getters/text IDs and runtime
actions remain unchanged. No new calls/checks, padding, shared edits, flags or
verifier changes. Primary stores suppression after set and before visibility;
this shared-tail source form is heuristic, not recovered original spelling.

Variant B keeps3217's suppression store local and shares the store for six
other warnings, reflecting primary's special3217 block. ARM204/0; entire
focused report AND object equal114 (cd625a949c83d404b31b81b8b9e33d4a1e3e97fd768c47bbadd11b78dc961e82),
so B was removed. This is distinct from older shared-getText/separate3217
trials because all seven source deliveries remain branch-local.

A improves End source98.6->98.8, control98.3->98.7 and calls97.8->99.3; strict
80.3->79.8, instruction inventory98.3->98.2, opcode98.8->98.7 and size99.7->98.9
decrease honestly. Raw1036->1028; other21 focused rows unchanged. Fixed20
means99.37/98.85428812843263 source,87.94/82.96180819602874 strict,7 linked/3 byte
unique bodies. Weighted99 remains unmet; strict improvement is not claimed.

Restored A after B and reproduced its entire report/object with fresh ARM204/0;
native STATIC exit0. Full4619/0, linked2084/byte960, coverage4274/4524, missing250
(7 wrong type/243 absent), extras129. ALL22 focus/full rows agree; baseline and
previous gates exit0 with zero exact-pair losses. Default99 gate exit1 solely
for weighted threshold. Canonical final reports updated after validation.
Source SHA256ac000659c5ae6147d7dc447d23ad4958e7d6626e24fb003d443a1925f208067c;
object0856e8255c8ddc4c590d88f2c08a08a452ce38db895419c0260ab251c983e7e4.
No native runtime/whole ABI or source-spelling claim. Evidence:
docs/validation/starmap_2026-10-06_warning_suppression_tail.json.

## Hundred-and-sixteenth accepted passenger branch split

Starting from47a245be, split the combined mission91/94 passenger warning OR
into two adjacent else-if branches. Both retain the ordered mission/station/
ship getters, deliver existing warning3214 and join the shared suppression
tail. No additional runtime calls or repeated prefix checks. This source form
is a hypothesis, not recovered original C++ spelling; unlike older21/87
high-capacity continuation trials, it actually changes the emitted comparison.

Fresh ARM disassembly confirms primary cb464 CMP r0,#9 / cb466 BLE.W and
candidate2440 CMP r0,#9 /2442 BLE.W. Candidate mission94 compares to zero
at247e and branches BEQ.W at2480; both warnings join2784. This establishes
the comparison shape, not byte equality of the entire function. No padding,
barriers, forced registers, shared files, flags or verifier changes.

A secondary trial split equipment13/17 checks after one campaign/station
prefix, retaining independent mission135 fallback. Its entire focused report
and ARM object were identical to the passenger candidate; removed as neutral.
Restoring the passenger-only candidate reproduced its report/object exactly.

Only OnTouchEnd changes: source98.8->98.9, strict79.8->80.0, instruction
inventory98.2->98.4, opcode98.7->98.8, control98.7->99.1, ordered control
87.9->88.3. Size98.9 and call99.3 remain unchanged. Other21 focused rows,
including aliases, are identical. Fixed20/7101 means99.375/98.86889170539361
source and87.95/82.9910153499507 strict;7 linked/3 byte unique bodies.

ARM204 compiled/0 failed; native STATIC exit0. Full4619 compared/0 skipped,
linked2084/byte960, coverage4274/4524, missing250 (7 wrong type/243 absent),
extras129. All22 focus/full rows agree. Baseline and previous preservation
gates exit0 with zero exact-pair losses. Default99 gate exits1 solely because
weighted98.86889 remains below99. Canonical reports updated after validation.

Source SHA2560ba76574f515702be1871c8762d806136892580610cf216dab0329d70a54fe53;
object3fa0a66974b3f8a5eda66c16e3ac1cf3e2063c37f029c4fe45c1855382a7a60f.
Native executable/runtime and whole ABI correctness remain unverified.
Evidence: docs/validation/starmap_2026-10-06_passenger_branch_split.json.

## Hundred-and-seventeenth follow-up: warning topology trials (neutral, removed)

Revalidated116 (6e6b6ecd) and its current canonical reports. Previous goal turn
is progress: passenger branch split changed the emitted CMP/BLE comparison,
improved the score and passed native/ARM/full/preservation gates.

Three bounded source-form trials preserve ordered getters and runtime actions:
(1) one mission142/station prefix with separate33/197/35 equipment failures
delivering3216; (2) jumpCost==1 with swapped582/579 delivery bodies; (3) an
independent139 if after105's successful delivery/goto, as presented by primary
decompilation. These are source-spelling hypotheses, not recovered C++.

Each ARM build compiled204/failed0. ALL22 StarMap rows and the whole StarMap
object are identical to116 in every trial; mission142 scoped report additionally
compares14 unchanged helper rows. Removed all neutral edits. Fresh restoration
ARM204/0 reproduces the ENTIRE canonical focus report and ALL204 object hashes.
Source/object hashes match116; native STATIC rebuild exits0.

Accepted116 full4619/0, linked2084/byte960 and prior preservation evidence stay
valid for this exact same204-object set. No new full run or changed canonical
report is claimed. Means remain99.375/98.86889170539361 source and
87.95/82.9910153499507 strict, fixed20/7101. Weighted99 is unmet; native
executable/runtime and whole ABI remain unverified. These branch spellings
are now measured as neutral; further work must examine remaining instruction/
call inventory differences instead of repeating them.

Evidence: docs/validation/starmap_2026-10-06_warning_topology_trials.json.

## Hundred-and-eighteenth accepted constructor store order

Fresh current drawKey diff and prior32/55/106 evidence confirm coordinate
grouping already agrees; no repeated coordinate/lifetime edit was applied.
Investigated constructor load/store ordering against the primary instead.
Capture Layout+0x88 before assigning mode, then set hitRadius from the value.
Swap the source order of the choiceVisible and markerGeom zero assignments.
These preserve values and calls; source spelling is heuristic, not recovered.

Only constructor C1/C2 strict score changes97.6->98.8; all source submetrics
and other20 rows stay identical. Primary marker zero before choice-visible
zero now agrees in the emitted code. Hit-radius region is closer but mode
store still differs in ordering. No whole-function byte equality is claimed.
Primary tail c6b68..6f contains float0.025/0.018 literal bytes, decoded by the
diff as four instructions versus two word entries. This is data, not missing
gameplay. No padding, verifier change, forced register or shared edit.

ARM204 compiled/0 failed; native STATIC exit0. Full4619/0, linked2084/byte960,
coverage4274/4524, missing250 (7 wrong type/243 absent), extras129. All22
focus/full rows agree; baseline/previous preservation gates exit0 with zero
losses. Default99 gate exits1 for the weighted threshold. Source means stay
99.375/98.86889170539361 on fixed20/7101. Canonical reports updated only after
full validation. Native runtime/whole ABI remain unverified; goal incomplete.
Evidence: docs/validation/starmap_2026-10-06_constructor_store_order.json.

## Hundred-and-nineteenth accepted easing-value captures

Revalidated118 (477275ea) and current primary/update diff. Previous turn is
progress: constructor ordering correction passed full/native and preservation
gates. Avoided prior30/85 neutral projection-depth selection forms.

During camera transition, primary c9efe/c9f06/c9f0e calls GetValue X/Y/Z,
retaining first2 returns in r4/r5. Only then c9f10 STRD writes X/Y and c9f16
writes Z, before c9f1c Vector assignment. Previous source stored each value
into tmp between getters. Capture three float locals in the same order and
aggregate local tmp after the third call. Keep existing scratchVector assignment;
remove the earlier function-scope tmp. Candidate now matches this sequence
with different stack offsets. Source spelling remains a hypothesis.

Only update changes: source98.7->98.8, strict82.7->83.3, raw966->967,
instruction inventory97->97.1, opcode98.2->98.5, size99.6->99.7. Other21
focused rows including aliases are identical. No new calls/checks, padding,
barriers, shared edits, flags or verifier changes. Float precision, getter
order and member assignment remain unchanged.

Diagnostic normalized inventories show74 calls and6 indirect BLX in both
bodies. Remaining control differences are primary/candidate B13/12, BGT4/2,
BPL6/7; other branch counts agree. Floating comparison/select differences
remain. This points to the next native control-flow audit; it does not justify
adding redundant comparisons or arithmetic merely to fill inventories.

ARM204 compiled/0 failed; native STATIC exit0. Full4619/0, linked2084/byte960,
coverage4274/4524, missing250 (7 wrong type/243 absent), extras129. All22
focus/full rows agree. Baseline/previous gates exit0 with zero exact-pair
losses; default99 gate exits1 for the weighted threshold. Fixed20/7101
canonical reports updated after validation; exact scores and hashes in the
compact evidence snapshot. Weighted99 remains unmet. Native executable/runtime
and whole ABI remain unverified.
Evidence: docs/validation/starmap_2026-10-06_easing_value_captures.json.

## Hundred-and-twentieth follow-up: completion/Y OR forms (removed)

Revalidated119 (2b04e4f2) current source/object/report. Previous turn is progress:
easing captures improved source shape and passed full/native/preservation.

Trial A replaces abs(pitchDistance) in the short-circuit completion test with
an explicit sign conversion and nested pitch-bound test AFTER the yaw bound,
following the primary decompilation's topology. ARM204/0. Entire focused JSON
and whole StarMap object equal119; neutral and removed.

Trial B initializes path-centering sy0, computes the division only inside
currentY>targetY OR currentY<targetY. This preserves zero for equal/unordered
comparisons but does not prove floating-status/runtime equivalence. Unlike91,
it has a shared conditional division rather than precomputed step/two assigns.
ARM204/0; only update regresses: source98.8->98.5, strict83.3->80.6, raw967->963,
inventory97.1->96.9, opcode98.5->98.3, control98.7->98.0, ordered95.4->91.4,
size99.7->99.3. Other21 rows unchanged; rejected.

Fresh restoration ARM204/0 reproduces the entire canonical report and ALL204
object hashes; source bytes equal the119 backup. Native STATIC rebuild exit0.
Accepted119 full4619/0,2084 linked/960 byte and preservation evidence remain
valid for this identical204-object set. No new full run or canonical change.
Source means99.38/98.88256583579778, strict88.04/83.12933389663428, fixed20/7101.
Weighted99 unmet; native runtime/whole ABI unverified. Do not repeat these
measured forms as new recovery candidates.
Evidence: docs/validation/starmap_2026-10-06_update_completion_y_or_trials.json.

## Hundred-and-twenty-first follow-up: dialog topology/call audit (removed)

Revalidated120 (2efe1998) and production119. Previous turn is progress:
completion/Y forms were measured and restoration proved across204 objects.

A: explicit419 current-station exit to existing visibility tail instead of
else wrapper. Entire focus/object identical119; neutral, removed. B: share419/
579 set delivery with assigned text ID and captured receiver, as primary
LABEL48 presents. No extra runtime calls; other dialogs skip common delivery.
Only End worsens source98.9->98.8, strict80->79.6, raw1028->1027; removed.

Fresh GNU callsite audit excludes known cb360..cb388 data island and separates
register-indirect calls. Primary136 named direct/19 indirect; accepted119 has
134/19. Named differences are one GameText::getText and one one-argument
ChoiceWindow::set static site. This does not prove omitted runtime actions.

C: put3217's set, suppression AND visibility locally, jumping past common
stores. Distinct from115B's local suppression only. Direct callee multiset now
equals primary136/19 exactly. End strict80->80.5, inventory98.4->98.5,
opcode98.8->98.9, size98.9->99.7 improve; control99.1->98.7 and normalized
call score99.3->97.8 worsen. Source98.9->98.7, raw1028->1036; rejected under
the unchanged objective. All21 other focused rows identical in B/C.

Fresh normalized CALL counts are primary133/accepted134, while actual named
direct counts above are136/134. This agrees with prior100's three primary
calls filtered around the literal island. No verifier edit, extra call/padding
or claim of native runtime equivalence. C++ source spelling remains heuristic.

Every trial ARM204/0. Final restoration ARM204/0 reproduces the entire canonical
focus report, source bytes and ALL204 object hashes; native STATIC exit0.
Accepted119 full4619/0, linked2084/byte960 and preservation gates remain valid
for identical objects; no new full run/canonical change. Source means remain
99.38/98.88256583579778, strict88.04/83.12933389663428, fixed20/7101.
Weighted99 unmet; native executable/runtime and whole ABI unverified.
Evidence: docs/validation/starmap_2026-10-06_dialog_topology_call_audit.json.

## Hundred-and-twenty-second follow-up: info word flags (removed)

Revalidated121 (b7c3777e), current production119 and fresh info diff. Prior turn
is progress: dialog experiments/call audit supplied measured exclusions and
proved restoration. Investigated campaign marker local flag representations.

A changes missionTarget and targetMatch from bool to int0/1, preserving ordered
getters, conditions and stores. Native word registers do not establish the
original C++ flag type; this is a hypothesis. ARM204/0; only info changes:
source99.3->98.7, strict80.6->79.6, raw1198->1194, inventory98.7->98.0,
opcode99.3->98.7, control99.1->98.0, ordered90.6->89.1, size99.2->98.8.
Other21 rows identical. Rejected. B keeps missionTarget bool and makes only
targetMatch int0/1. ARM204/0; entire focus and whole object identical119.
Neutral; removed. No extra runtime calls/checks or padding.

Read-only pulse review: the extra current UXTB does not justify changing the
shared SetColor declaration. Primary dynamic symbol at7c938 is
_ZN11AbyssEngine11PaintCanvas8SetColorEhhhh, matching existing four unsigned
char parameters. Layout::getPulseValue reads playing time/Sinf twice and
selects b/-b from a's sign; no caller-wide range proof or narrowing rewrite
was applied. No shared ABI, flags or verifier changes.

Fresh restoration ARM204/0 reproduces the entire canonical focused report,
source bytes and ALL204 object hashes; native STATIC exit0. Accepted119
full4619/0,2084 linked/960 byte and preservation gates stay valid for identical
objects. No new full run or canonical change. Means99.38/98.88256583579778
source,88.04/83.12933389663428 strict, fixed20/7101. Weighted99 remains unmet;
native executable/runtime and whole ABI unverified.
Evidence: docs/validation/starmap_2026-10-06_info_word_flags.json.
