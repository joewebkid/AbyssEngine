# TargetFollowCamera: ARM source-shape gate, 2026-10-02

## Result

The requested >=99.0% gate is met for the unchanged **40 unique Android
bodies / 42 exported symbols**, including constructor C1/C2 and destructor
D1/D2 aliases. No difficult body was removed from the denominator.

| Scope | Baseline source mean | Final source mean | Baseline instruction-weighted | Final instruction-weighted |
| --- | ---: | ---: | ---: | ---: |
| 40 unique bodies | 93.690% | **99.965%** | 88.725% | **99.269%** |
| 42 symbols, aliases counted | 93.795% | **99.967%** | 89.029% | **99.340%** |

These figures use the same corrected verifier on both revisions. The weights
are fixed original executable instruction counts, excluding literal-data
directives: 1,255 instructions across unique bodies (1,390 with aliases).
Constructor weight is 134; `update` weight is 655. Every baseline and final
symbol also has the same original `n_target`. We average the verifier's
recorded one-decimal `source_match`, without rounding the gate to an integer.
The terminal's `100.0%` family display is only coarse rounding of 99.9667%.

Final family strict fuzzy is 96.73% over symbols and 96.8675% over unique
bodies. **32 linked-exact / 23 byte-exact symbols**, versus baseline 27/19.
`update` remains **98.6% source-shape / 39.9% strict**, with neither exact
flag; the constructors are 100% source-shape / 87.9% strict, also non-exact.
This is a source-shape family gate, not 99% byte-identical camera recovery.

## Clean base and reproducibility

Base: `f60694dd0ca578831cec832e7026b32644642d97`, the completed Level mission
generation checkpoint, confirmed as the latest completed local branch tip
when this work began. The dirty primary checkout was not modified. Work is
isolated at `A:/gof2-tfc-99`, branch `codex/target-follow-camera-99`.

Toolchain: existing NDK r18b Windows clang 7.0.2, ARMv7 Thumb/NEON softfp,
canonical `tools/verify/match_flags.sh`, `-Oz`. No flag tuning, volatile
fields, forced registers, inline assembly, or gameplay approximations were
introduced. Original binary remains in ignored `_work/bins/`.

The initial fresh run with the unmodified verifier measured **93.6857%**
over 42 symbols, rather than the historical 92.5% in `VALIDATION.md`, and
confirmed exactly 27/19. The initial full run had 4,619 comparisons,
2,079 linked-exact / 959 byte-exact pairs, 204/204 compiled TUs, zero skipped
units, and seven false wrong-type misses. Its snapshot remains at
`_work/full_baseline.json` and the raw focused snapshot is checked in.

The frozen [body registry](validation/target_follow_camera_bodies_2026-10-02.json)
records original addresses, all alias names, and weights. The
[baseline](validation/target_follow_camera_baseline_2026-10-02.json),
[final](validation/target_follow_camera_final_2026-10-02.json), and
[gate](validation/target_follow_camera_gate_2026-10-02.json) snapshots retain
every per-function signal and the full-report input hashes.

## Evidence inspected

- Root `CONTEXT_INDEX`, `PROJECT_MAP`, `CURRENT_STATUS`, `TASK_QUEUE`, reverse
  engineering context pack, root/native `AGENTS.md`, native `README`,
  `CONTRIBUTING`, `PROJECT_STATUS`, `VALIDATION`.
- Target header/implementation; `AEGeometry.h`, `AERandom.h/.cpp`,
  `Globals.h`, `Matrix.h/.cpp`, `AEMath.h/.cpp`, `Vector.h`; camera callers in
  `MGame.cpp`, `PlayerEgo.cpp`, and ownership leads in `CutScene.cpp`.
- Android SDK extraction
  `gof2_recovered_sdk_first_pass/extracted/flight_physics_camera_collision/TargetFollowCamera.c`
  and raw Android disassembly of `0x15a4c8..0x15b658`. The extraction lacks
  some FP call arguments; raw instructions resolve those ambiguities.
- Available `gof2_ios_ida_full_dump.c` and
  `gof2_gidra_ios_engine_full_source.c` were searched for `TargetFollow`,
  `followcamera`, and `dampingfunktion`: no named class body was found. No
  matching PC camera source was available in the local reference inventory.
  These sources do **not** independently prove this Android implementation.
- `DeepOpen/src/AE/TargetFollowCamera.java` confirms the camera lineage but
  uses another GraphNode/CameraController architecture and is not ARM ABI or
  coefficient proof. The earlier selective-transfer report was used as a
  layout lead, not copied as implementation authority.

The class has no vptr: ARM stores camera id at +0 and target at +4, and no
`_ZTV18TargetFollowCamera` entry exists in the original symbol map. This is
not a virtual camera interface. Existing ARM assertions enforce size 0x178
and offsets rumbleStrength +0x120, rollAngle +0x130, fixed +0x138 and
localMatrix +0x13c. The two 0x3c Matrix members construct once. Target geometry
is borrowed: `MGame` allocates/deletes the camera and passes player geometry;
the empty D1/D2 destructor does not delete it. No ownership or vtable slot was
invented from Java/iOS.

## Accepted source forms

- Constructor `0x15a4c8`: zero vector slots in member initialization, remove
  redundant Matrix temporary assignments, restore initial target/camera
  `MatrixTransformVector` calls and the native active/rumble/look-at flags,
  zero roll and handling=1. Parameter names now identify the first offset as
  target offset and the second as camera offset, without changing their ABI.
- Three mode getters return the native byte, without a boolean normalization
  sequence. Each gains raw-byte exactness. `setPosition(Vector const&)`
  copies the first eight bit-pattern bytes and the final component through
  `memcpy`, avoiding aliasing casts and reproducing native LDRD/STRD.
- `translate` retains its six field additions before `update(1000)` rather
  than adding a non-native call to `translateNoUpdate`.
- `setFastForwardMode` keeps the native two-way flag test, calls
  `setShipHandling(shipHandling)`, then rebuilds both coefficient banks again.
  The apparently redundant rebuild is present in ARM and was not optimized
  away in source. This method gains linked exactness.
- Fixed/look-at branches select target or explicit identity orientation for
  up, while target position remains from the target matrix. The fixed path
  retains the original direct position copy plus Vector assignment.
- First person restores identity-return assignment, rotation by 3.1416 about
  Y, `firstPersonMatrix * rotation`, position interpolation, strict `>` hide
  threshold, translated matrix slots, and transformed direction. The final
  first-person target subtracts **position minus MatrixGetDir**, established
  from actual ARM argument registers, not the ambiguous decompiler call.
- Chase uses signed 64-bit time powers and shared double conversions;
  first person uses wrapping 32-bit powers. Both multiply by a float
  reciprocal promoted to double. The 64-bit products now use unsigned
  arithmetic before signed interpretation to preserve native wrap without
  signed-overflow UB. Clang emits identical function bytes and relocation
  masks for this defined-arithmetic form.
- Local identity values are written explicitly: orientation m[10] and scale
  m[12..14] are 1. The shared Matrix constructor currently initializes some
  of these differently and is deliberately outside this camera patch.
- Random and camera routes use native `Globals::rnd` / `Globals::Canvas`;
  non-original random null/range guards are removed. Per-axis rumble strength
  and shake amount reloads, and shake multiplication order, follow ARM.

## Trials and rejected forms

| Trial, original verifier unless stated | 42-symbol source mean | Update source | Decision |
| --- | ---: | ---: | --- |
| Fresh baseline | 93.69% | 85.0% | Fixed starting family |
| Initial semantic repair | 99.16% | 88.9% | Continue; weighted gate not met |
| Separate 64-bit helper, explicit identities | 99.72% | 94.6% | Reject helper outline/copy form |
| Direct globals and shared in-body 64-bit powers | 99.80% | 97.8% | Keep arithmetic; continue matrix audit |
| Remove redundant matrix copies, correct direction | 99.81% | 98.3% | Accepted source |
| Same source, corrected literal/alias decoding | 99.97% | 98.6% | Accepted verifier, both bases remeasured |

A single 12-byte `memcpy` for setPosition emitted VFP loads/stores and scored
84%; scalar float assignment scored 74.3%. The final bit-copy form is byte
exact. Caching one zero Vector in the constructor prevented the second
native zero-initialized temporary; separate temporaries recover the intended
lifetimes. A shared `tfcRandom`/64-bit damping helper was outlined at `-Oz`,
introducing non-native call bodies. Direct native service calls and the one
shared chase arithmetic block avoid these extra calls. No force-inline
attribute was used. Unconditional Matrix copies followed by conditional
copies added duplicate memcpy calls; conditional identity replacement is the
accepted form.

## Verifier correction and limits

The stripped original decodes vector constants as bogus Thumb opcodes.
Constructor `ADD pc -> VLD1.64 {d16-d17}` reads four literal words; update's
similar loads read explicit identity constants. The verifier now recognizes
bounded, uninterrupted address-register-to-vector-load proofs, exact byte
ranges and instruction boundaries. It rejects overwritten address registers,
intervening branches/pop, out-of-function data, and incoming branches into
the candidate pool. PC-relative address displacements are locations, not
game constants. Raw function bytes and relocation masks are unchanged.

COMDAT aliases are indexed by (section,address), rather than one address
across all sections; `.text.*` aliases now participate. Eight unit tests cover
positive data, unsafe/ambiguous cases, address normalization and separate
same-address COMDAT sections. Both untouched baseline and final are fully
recompared with this same verifier. This correction is not hidden inside the
reported source improvement: **the old verifier gives only 98.60% weighted
over symbols / 98.78% over unique bodies**, so it would not pass the gate.

## Validation

- UCRT64 `gof2`: successful `libgof2.a` build, including final source revision.
- NDK ARM: 204/204 TUs, zero failed. A final rebuild after comments/parameter
  names and defined 64-bit arithmetic leaves all 204 object SHA256 hashes
  identical to the objects used in the final full report.
- Full corrected baseline/final: 4,631 comparison pairs, 4,281/4,524 unique
  originals, zero skipped and zero wrong-type; 243 external/absent originals
  remain. These are not a complete native executable linkage claim.
- Corrected full linked-exact: **2100 -> 2105**, byte-exact **964 -> 968**;
  lost pair sets are empty. Against the initial unmodified-verifier full
  snapshot, **all 2079 linked-exact and 959 byte-exact pairs also survive**.
  Comparison keys are `(unit,symbol)`, not only aggregate counts.
- Focused family: all original 27 linked-exact and 19 byte-exact survive;
  five new linked-exact and four new byte-exact symbols.
- Host smoke: real TargetFollowCamera, AEMath operations and AERandom;
  constructor/offsets, flags, handling/fast-forward, translateNoUpdate,
  position copying, boost clamp, and active-hit preservation pass. Geometry
  and Matrix construction are stubs; renderer is an unexecuted trap. The
  first attempt exposed duplicate Matrix operators in the two existing math
  TUs; the focused harness uses AEMath and supplies only Matrix construction.
- Eight verifier tests and `git diff --check`: pass.

Native host smoke command (MSYS2 UCRT64 compiler in PATH):

```powershell
g++ -std=c++17 -O0 -Isrc -Ithird_party/fmod/inc -Ithird_party/gl -Ithird_party/jni tools/verify/target_follow_camera_smoke.cpp src/game/ship/TargetFollowCamera.cpp src/engine/math/AEMath.cpp src/engine/core/AERandom.cpp -o _work/target_follow_camera_smoke.exe
./_work/target_follow_camera_smoke.exe
```

Recheck the saved denominator and exact-pair gate:

```powershell
python tools/verify/target_follow_camera_gate.py --registry docs/validation/target_follow_camera_bodies_2026-10-02.json --baseline docs/validation/target_follow_camera_baseline_2026-10-02.json --final docs/validation/target_follow_camera_final_2026-10-02.json --full-baseline _work/full_baseline_corrected.json --full-final _work/full_final.json --output _work/tfc_gate_recheck.json
python -m unittest discover -s tools/verify -p 'test_*.py'
```

## Remaining uncertainty

No Android ARM32 executable or rendered flight camera was run. Mode transitions,
collision interaction and visual camera feel need native runtime validation.
The original constructor leaves fastForward, rumbleStrength, some shake state
and padding unspecified; this patch does not invent initialization. Valid
geometry/global services and native frame-time ranges remain caller
preconditions. Decimal coefficient formulas are unchanged; 100% shape does
not certify their numerical bit identity. The large update's block placement,
register/stack allocation, and branch choices still diverge: ordered control
flow 69.9%, inventory 97.1%, instruction inventory 98.3%, opcode inventory
98.4%, call count 100%, size agreement 99.3%. No remaining compiler difference
is claimed as recovered runtime semantics. Godot was not changed.

## Per-body results

The table below counts aliases once. Full 42-symbol rows, including all five
source-shape component signals and both exact flags, are in the linked JSONs.

| Address | Body (aliases grouped) | Instructions | Before source | After source | After strict | Exact |
| --- | --- | ---: | ---: | ---: | ---: | --- |
| 0x15a4c8 | TargetFollowCamera(unsigned int, AEGeometry*, AbyssEngine::AEMath::Vector, AbyssEngine::AEMath::Vector) (C1/C2) | 134 | 91.8% | 100.0% | 87.9% | — |
| 0x15a688 | aproximateCooefficientsForAproximationOfDampingFunktion(float, double&, double&, double&, double&, double&) | 110 | 100.0% | 100.0% | 100.0% | — |
| 0x15a998 | ~TargetFollowCamera() (D1/D2) | 1 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a99a | setActive(bool) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9a0 | setTarget(AEGeometry*) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9a4 | getTarget() | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9a8 | setTargetOffset(AbyssEngine::AEMath::Vector const&) | 2 | 100.0% | 100.0% | 100.0% | linked |
| 0x15a9ae | getTargetOffset() | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9b2 | setCamOffset(AbyssEngine::AEMath::Vector const&) | 10 | 100.0% | 100.0% | 100.0% | linked |
| 0x15a9cc | getCamOffset() | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9d0 | setPosition(float, float, float) | 3 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9d6 | setPosition(AbyssEngine::AEMath::Vector const&) | 5 | 74.3% | 100.0% | 100.0% | byte + linked |
| 0x15a9e4 | getPosition() | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15a9e8 | translate(float, float, float) | 23 | 25.5% | 100.0% | 100.0% | — |
| 0x15aa48 | update(int) | 655 | 85.2% | 98.6% | 39.9% | — |
| 0x15b280 | translateNoUpdate(float, float, float) | 22 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b2d8 | enableFirstPersonCam(bool) | 29 | 100.0% | 100.0% | 100.0% | — |
| 0x15b32c | hideShipForFirstPersonCam() | 2 | 70.9% | 100.0% | 100.0% | byte + linked |
| 0x15b332 | setFirstPersonMatrix(AbyssEngine::AEMath::Matrix&) | 2 | 100.0% | 100.0% | 100.0% | linked |
| 0x15b338 | setBoostPercentage(float, int) | 8 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b34a | setRumblePercentage(float, int) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b350 | setFixed(bool) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b358 | setLocal(AbyssEngine::AEMath::Matrix) | 40 | 100.0% | 100.0% | 100.0% | linked |
| 0x15b3d4 | getLocal() | 6 | 100.0% | 100.0% | 100.0% | linked |
| 0x15b3e4 | setLocked(bool) | 46 | 100.0% | 100.0% | 100.0% | linked |
| 0x15b464 | setRotationAroundTarget(bool) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b46c | rotateAroundTarget(float, float, float) | 17 | 100.0% | 100.0% | 100.0% | linked |
| 0x15b498 | zoomTarget(float) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b49e | setLookAtCam(bool) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b4a4 | calculateCoefficents(float) | 29 | 100.0% | 100.0% | 100.0% | linked |
| 0x15b50a | setFastForwardMode(bool) | 32 | 58.1% | 100.0% | 100.0% | linked |
| 0x15b574 | setShipHandling(float) | 16 | 100.0% | 100.0% | 100.0% | — |
| 0x15b5c8 | isInFastForwardMode() | 2 | 70.9% | 100.0% | 100.0% | byte + linked |
| 0x15b5ce | isInLookAtMode() | 2 | 70.9% | 100.0% | 100.0% | byte + linked |
| 0x15b5d4 | resetShipHandling() | 5 | 100.0% | 100.0% | 100.0% | — |
| 0x15b5ec | hit() | 12 | 100.0% | 100.0% | 83.3% | — |
| 0x15b60e | hitSmall() | 11 | 100.0% | 100.0% | 63.6% | — |
| 0x15b62c | useTargetsUpVector(bool) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b632 | roll(float) | 5 | 100.0% | 100.0% | 100.0% | byte + linked |
| 0x15b644 | setRoll(float) | 2 | 100.0% | 100.0% | 100.0% | byte + linked |
