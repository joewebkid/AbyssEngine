# StarMap Android ARM recovery - 2026-10-02

## Status and scope

**PARTIAL: the requested 99.0% target is not reached.** The fixed 20 distinct
implementation bodies measure **98.940% unweighted / 97.97972% original-instruction
weighted source-shape**. The clean baseline measures **83.81% / 73.61763%**.
Strict ordered fuzzy matching improves from **48.29% / 28.05016%** to
**85.610% / 77.88157%**, respectively. Source-shape is an inventory/control-flow
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

## Validation

Final focused ARM build compiles **204/204 translation units, zero failures**;
22 symbols compare, zero skipped, six unique linked-exact bodies and three
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
75.97% -> 76.15% (including all corpus entries).
Coverage remains 4274/4524 original functions, 250 missing (7 wrong-type / 243
absent). Final extra symbols: 129, baseline: 130. Existing coverage gaps were
not removed from reporting.

Exact pair preservation gate: **PASS**, zero lost (unit, symbol) pairs.
Linked-exact **2079 -> 2083**; byte-exact **959 -> 960**. Gains are confined to
StarMap: initLights, missionChanged, askForJumpIntoAlienWorld and setStart;
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
Latest full log: `_work/tenth-corpus.log`; focused log:
`_work/update-center-tenth.log` (copied report: tenth-focused.json); native log: `_work/native-tenth.log`.
Exact-pair preservation also passed against checkpoints 7fc7701c, dca65361,
50e10b79, b5010fdb, d4e3330a, bc98481c, 42331c92, 52079d64 and 7b76861c, with no new gains or losses since
7fc7701c. The latest comparison against 7b76861c preserves all 2083
linked-exact and 960 byte-exact pairs. Latest preservation gate and 99%
gate logs are _work/tenth-gate.json, tenth-previous-gate.json and
tenth-target-gate.log (exit 0, 0 and 1 respectively).

baseline full JSON SHA-256: `ceb8ddf50e03dbadd2278413942c6e1a8e59b1f6d4060d7628de66593632a6d0`.

final full JSON SHA-256: `2fddeb0840b39c9345d5b92a5bb7dded34acd4b9458ffb5f0dbdaaa3ade48008`.


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

## Fixed unique-body measurements

Percentages are the unchanged verifier output. C2/D2 aliases are excluded.

| Android symbol | Original/current instructions | Baseline source | Final source | Final strict | Final exact |
|---|---:|---:|---:|---:|---|
| `_ZN7StarMap16drawOnScreenInfoEib` | 1207/1221 | 78.9% | 97.9% | 64.9% | - |
| `_ZN7StarMap10OnTouchEndEii` | 1037/1003 | 66.6% | 97.5% | 76.4% | - |
| `_ZN7StarMap6updateEi` | 971/961 | 76.6% | 98.3% | 82.0% | - |
| `_ZN7StarMap4drawEv` | 916/909 | 49.7% | 98.3% | 87.6% | - |
| `_ZN7StarMap4initEbP7Missionbi` | 750/737 | 72.9% | 98.0% | 76.1% | - |
| `_ZN7StarMap14initStarSystemEv` | 710/680 | 80.9% | 96.3% | 81.7% | - |
| `_ZN7StarMapC1EbP7Missionbi` | 333/332 | 85.1% | 98.0% | 84.2% | - |
| `_ZN7StarMap12OnTouchBeginEii` | 275/273 | 91.6% | 99.3% | 97.4% | - |
| `_ZN7StarMap11OnTouchMoveEii` | 265/255 | 81.3% | 98.4% | 79.2% | - |
| `_ZN7StarMap7drawKeyEv` | 246/243 | 59.5% | 97.8% | 34.8% | - |
| `_ZN7StarMap6departEb` | 205/205 | 94.9% | 100.0% | 88.3% | - |
| `_ZN7StarMapD1Ev` | 69/70 | 92.0% | 99.4% | 95.0% | - |
| `_ZN7StarMap6renderEv` | 47/47 | 95.7% | 99.6% | 97.9% | - |
| `_ZN7StarMap10initLightsEv` | 21/21 | 87.8% | 100.0% | 100.0% | linked |
| `_ZN7StarMap24askForJumpIntoAlienWorldEv` | 21/21 | 97.1% | 100.0% | 100.0% | linked |
| `_ZN7StarMap8setStartEii` | 17/17 | 88.9% | 100.0% | 100.0% | linked |
| `_ZN7StarMap14isInPlanetModeEv` | 5/5 | 100.0% | 100.0% | 100.0% | byte |
| `_ZN7StarMap14setJumpMapModeEbb` | 3/3 | 86.7% | 100.0% | 66.7% | - |
| `_ZN7StarMap14missionChangedEv` | 2/2 | 90.0% | 100.0% | 100.0% | byte |
| `_ZN7StarMap8renderBGEv` | 1/1 | 100.0% | 100.0% | 100.0% | byte |

Aliases: `_ZN7StarMapC2EbP7Missionbi` = C1; `_ZN7StarMapD2Ev` = D1.
