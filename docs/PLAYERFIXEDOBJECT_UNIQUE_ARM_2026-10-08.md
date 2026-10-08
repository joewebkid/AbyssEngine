# PlayerFixedObject Android ARM recovery — 2026-10-08

The fixed family improves from **91.307% to 94.079% unweighted** and
**85.779% to 97.095% original-instruction-weighted** source-shape. It does
**not** meet both 98% gates. These are assembly-comparison scores, not game
completion or native runtime certification.

## Fixed baseline and evidence

Worktree: `gof2hd-98-playerfixed`; branch:
`codex/playerfixedobject-arm-98-20261008`. The worktree was clean at
`20bf2679a88ada953d3b393bc42a0aa54a557264` before any source edits.

Before editing, a fresh 206-TU ARM build and focused verifier run froze
**29 original addresses / 31 symbols / 1841 original normalized instructions**.
C1/C2 at `0x153a7c` and D1/D2 at `0x153d70` each count once; D0 at
`0x153e14` is a separate body. Every original address, alias, fixed weight,
baseline score and final score is recorded in
[the machine-readable family record](validation/playerfixedobject_unique_arm_2026-10-08.json).
Weights are the baseline verifier's `n_target`; no bodies or weights were
added, removed or changed after inspecting the scores. Means use the
verifier's recorded per-body scores.

Primary input is Android ARMv7 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Inspected the original instructions and literal words at `0x153a7c–0x154f7c`,
Android IDA bodies in `analysis/gof2_libgof2hdaa_full_ida.c`, ELF dynamic
symbols/relocations, and the compiled ARM objects. No iOS/PC or Java body
was substituted for Android evidence.

Source inspection covered `PlayerFixedObject.cpp/.h`, `KIPlayer.h`,
`Player.h`, `BoundingVolume.cpp/.h`, `Vector.cpp/.h`, `AEString.h`,
`AEGeometry.h`, `Globals.cpp/.h`, `Status.h`, `Explosion.h`,
`TargetFollowCamera.cpp/.h`, and call sites in `Level`, `MGame`, `Hud`, and
`PlayerEgo`. The verifier, canonical flags and build instructions were read
but not modified. `Ship.cpp/.h` and the shared status/reconciliation notes
were not changed.

## Source-backed repairs

- Constructor extension vectors now receive aggregate zero initialization
  before the member String constructor, matching the original store order.
  Removed redundant manual String data release/reset in constructor and
  destructor; normal member lifetime performs the original String calls.
  Destructor rereads collision-array ownership after class release calls.
- `getName` uses the original `(String const&, false)` constructor.
  `setName` returns the assigned String object's address through the existing
  `void*` declaration, rather than converting it to a wide-character buffer.
  Original `0x154f16` branches through veneer `0x19c548` to the String
  assignment PLT at `0x5f2b0`. Inspected callers discard this return value;
  no signature was changed to obtain a tail-call score.
- `reset` uses real Vector assignments and scoped temporaries, removing the
  null function-pointer placeholder. `moveForward` obtains position from its
  actual geometry, not an uninitialized stack buffer. Float position setters
  call the original geometry float overload. Projection uses the selected
  BoundingVolume and the caller's vector.
- `update` uses actual Globals status, audio, options, achievements and
  canvas references. Restored bomb multiplier `0.98`, force cutoff `0.05`,
  death kind/destroyed-byte arguments, pirate-station false flag, damage
  `9999999`, and Status counter `+0x118`. Cargo accesses remain one byte at
  `+0x4c`, preserving adjacent storage.
- Restored distinct wreck/base explosion paths, geometry positions and audio
  float argument `0.0`; Explosion update selects the camera-pointer overload
  with null. Restored animation's false update flag and emitter disable on
  transition to state 4, expiry's `KIPlayer::setActive(false)`, and material
  type 0 (`0x824c`). Material output uses the original low 16 bits.
- Rumble uses distance `30000` and fade denominator `-2000`; dormant target
  ranges use `-50000/+50000` and the captured `getEnemy` result. The original
  final dormant test omits a lower Z bound; that asymmetry is retained.
  Cargo rotation literals are read directly at `0x1548d0/0x1548d4`:
  `0x37800000/0x40c90fdb`, rather than rounded IDA decimal approximations.
- Exhaust visibility uses `childTransform` (`geometry +0x14`), and collision
  gates read the original `+0x8c` byte rather than KIPlayer's `+0x40` flag.
  Selectable wreck animation supplies its length for both range endpoints,
  as proven by the ARM register/stack arguments. Render follows the original
  state dispatch. Added override annotations and ARM offset assertions.
- `setWreckedMeshId` uses the original `Globals::globals` instance for both
  collision lookups. The previous `gGlobals` placeholder is initialized to
  null and never receives the live instance in the inspected startup path.

## Focused result and honest ceiling

| Unique body | Baseline source | Final source | Final exact evidence |
| --- | ---: | ---: | --- |
| Constructor C1/C2 | 91.6% | 97.4% | neither |
| Destructor D1/D2 | 91.9% | 88.8% | neither |
| `reset` | 54.2% | 100.0% | linked |
| `update` | 79.9% | 96.7% | neither |
| `getProjectionVector` | 85.3% | 100.0% | linked |
| `getName` | 93.0% | 100.0% | linked |
| `setDeadButSelectable` | 99.4% | 100.0% | linked |
| `setName` | 40.0% | 19.4% | neither |
| `setMoving` | 40.0% | 40.0% | raw-byte and linked |

Focused exact counts improve **13 linked / 10 byte → 17 linked / 10 byte**;
no baseline exact pair is lost. Strict fuzzy mean across all 31 symbols
improves `76.50% → 86.61%`. The four gains are `reset`, `getName`,
`getProjectionVector`, and `setDeadButSelectable`.

The unchanged byte-exact `setMoving` is decoded on the target as `strbal.w`
and `bx<und>` while our identical six bytes decode as `strb.w` and `bx`.
The preceding delinked reset literal data includes an apparent IT instruction;
this is consistent with objdump IT-state leakage into the next symbol.
The fixed verifier nevertheless reports **40%**. Preserving this exact body
alone imposes an unweighted ceiling of `(28*100+40)/29 = 97.931034%`, even
if all other bodies scored 100%. No extraction, normalization, formula or
flag changes were made to lift that ceiling.

Other remaining differences are not all register allocation: `setName` has
a call/prologue rather than the original interworking tail veneer; D1 lacks
the original exception-unwind cleanup under current destructor contracts;
update retains control/instruction differences and the BoundingVolume
virtual-update discrepancy below. Restoring String ownership improves D1
strict fuzzy `49.1% → 92.9%` even while its source score falls. No dead calls,
barriers, signature changes or fabricated cleanup paths were added to make
these metrics look better.

## Validation and remaining risks

- Native UCRT64 `gof2` static-library build: passed after the final edit.
  Existing unrelated pointer-precision warnings remain.
- All **206/206** source-mapped ARM TUs: compiled, zero failures, canonical
  NDK r18b `-Oz` flags unchanged. A clean full build was followed by a rebuild
  after correcting the two literal values and the live Globals reference.
- Focused report: 31 symbols, zero skips; every fixed weight unchanged.
- Verifier tests: **36 run, one platform-dependent skip, no failures**.
  Native ABI/lifetime and ParticleSettingsRef smokes both report OK. These
  existing host smokes do not exercise a running PlayerFixedObject game path.
- Full-corpus `--fail-on-wrong-type`: passed. Both revisions compare
  **4323/4524 originals / 4689 rows**, with **zero wrong-type misses and
  zero skipped units**. Mean source-shape `91.80% → 91.82%`; strict fuzzy
  `79.73% → 79.80%`; linked-exact **2305 → 2309**; byte-exact **997 → 997**.
  Sets were compared by `(unit, symbol)` with normalized path separators:
  **zero lost rows, linked pairs or byte pairs**, and no coverage decrease.
  All four linked gains belong to this family. Hashes of every source-mapped
  ARM object were compared: only `game/ship/PlayerFixedObject.o` changed;
  the other **205 objects are identical** to baseline.
- Original PlayerFixedObject vtable at `0x20de90` is `0x68` bytes: the two
  header words plus all **24 method slots** match baseline and final objects.
  Size `0x1bc` and extension/inherited-byte storage offsets pass ARM asserts.
- An existing **BoundingVolume ABI discrepancy** remains outside this patch:
  original vtable at `0x20dd8c` has `getCollisionNormal/update/collide/
  outerCollide/pure-virtual` in slots 0–4; current declaration instead has
  virtual D1/D0 in slots 0–1 and update in slot 4. Its baseline/final vtables
  are unchanged. Therefore PFO's typed update calls still select `+0x10`
  instead of original `+0x04`. Repair requires auditing the shared collision
  declarations/derived classes, rather than bypassing current objects' vtables
  solely for this family's metric.
- The pre-existing `BV_staticProjectCollisionOnSurface` compatibility call
  remains because the shared member declaration currently has a different
  return contract. Linked-exact comparison masks relocation values; it is not
  proof that this compatibility path forms a native executable.
- Original unconditional wreck matrix setup before its null check is retained.
  Its runtime ownership/preconditions, whole-game execution and ARM-device
  runtime are not certified. Legacy int-valued pointer interfaces also remain
  a risk for a future 64-bit port. No Godot validation was substituted.

All generated reports, objects, native build files and diagnostic scripts
are under this **C:** worktree's `_work/playerfixed-arm98/`. Original binary
and NDK were read only through the instructed **A:** paths. Artifacts include
`baseline-focused.json`, `frozen-family.json`, `baseline-full.json`,
`final-focused.json`, `final-full.json`, `full-pair-gate.json`,
`abi-probe.json`, `final-arm-build.log`, `final-native-build.log`, and
`verifier-tests.log`. No push, PR or merge is part of this patch.
