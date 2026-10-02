# StarMap Android ARM recovery - 2026-10-02

## Status and scope

**PARTIAL: the requested 99.0% target is not reached.** The fixed 20 distinct
implementation bodies measure **99.160% unweighted / 98.44298% original-instruction
weighted source-shape**. The clean baseline measures **83.81% / 73.61763%**.
Strict ordered fuzzy matching improves from **48.29% / 28.05016%** to
**86.985% / 80.79548%**, respectively. Source-shape is an inventory/control-flow
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
Latest full log: `_work/twentyeighth-corpus.log`; focused log:
`_work/station-textx-twentyeighth.log`; native log: `_work/native-twentyeighth.log`.
Exact-pair preservation also passed against checkpoints 7fc7701c, dca65361,
50e10b79, b5010fdb, d4e3330a, bc98481c, 42331c92, 52079d64, 7b76861c, 72045054, eeebc3e6, 644c59e5, e07a1f16, 2f20db28, f10955b3, 613af2c6, f9e870cf, 80ea89b1, c8b7e875, cf030a4e, 7e7528b7, 43f3e1c2, fcb1741b, f7cbbae0, 0fe3756d, 03aa0342 and 72465845, with no new gains or losses since
7fc7701c. The latest comparison against 72465845 preserves all 2083
linked-exact and 960 byte-exact pairs. Latest preservation gate and 99%
gate logs are _work/twentyeighth-gate.json, twentyeighth-previous-gate.json and
twentyeighth-target-gate.log (exit 0, 0 and 1 respectively).

baseline full JSON SHA-256: `ceb8ddf50e03dbadd2278413942c6e1a8e59b1f6d4060d7628de66593632a6d0`.

final full JSON SHA-256: `1f9a2b70a89efd170a9e5e47ce81ff7a7df431479becce4f599c585bbafcd77f`.


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
| `_ZN7StarMap16drawOnScreenInfoEib` | 1207/1198 | 78.9% | 98.8% | 75.3% | - |
| `_ZN7StarMap10OnTouchEndEii` | 1037/1003 | 66.6% | 97.5% | 76.4% | - |
| `_ZN7StarMap6updateEi` | 971/966 | 76.6% | 98.5% | 82.1% | - |
| `_ZN7StarMap4drawEv` | 916/907 | 49.7% | 99.0% | 89.1% | - |
| `_ZN7StarMap4initEbP7Missionbi` | 750/736 | 72.9% | 98.3% | 77.0% | - |
| `_ZN7StarMap14initStarSystemEv` | 710/683 | 80.9% | 97.3% | 84.7% | - |
| `_ZN7StarMapC1EbP7Missionbi` | 333/333 | 85.1% | 99.3% | 95.8% | - |
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
