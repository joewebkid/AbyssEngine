# PlayerTurret Android ARMv7 source-shape gate — 2026-10-02

Status: **PASSED** — both unique-body averages >=99.0%, native build, all ARM
units and full-corpus signature/exact-preservation gates passed.

## Scope and reproducible baseline

Isolated worktree `A:/gof2-player-turret-99`, branch
`codex/player-turret-99-20261002`, clean base `f60694dd` (latest completed
integration found locally). The dirty main checkout and other worktrees are untouched.
PlayerTurret.cpp/.h, this report, family evidence, the standalone gate and
the necessary minimal COMDAT alias parser fix are changed. No compiler flags, shared configs, Gun,
TargetFollowCamera or StarMap edits.

Read root AGENTS.md, docs/CONTEXT_INDEX.md, PROJECT_MAP.md, CURRENT_STATUS.md,
TASK_QUEUE.md, reverse_engineering_context.md; native AGENTS.md, README.md,
CONTRIBUTING.md, PROJECT_STATUS.md and VALIDATION.md. Inspected PlayerTurret.cpp/.h,
KIPlayer and Player field/call declarations, AEGeometry/Transform/Vector/Matrix,
ParticleSystemManager, Globals, AERandom, Standing, PlayerEgo and the direct
PlayerTurret calls in Level.cpp. The extracted Android PlayerTurret.c is a
navigation aid; its decompiler types and rounded floats are not authoritative.
Primary evidence is the actual Android 2.0.16 ARMv7 ELF, SHA256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
No iOS/PC result is mixed into these numbers.

The documented September snapshot was 21 symbols / 95.5% source / 12 linked /
4 byte. Fresh clean f60694dd focused verification instead gives 21 / 96.72% /
12 / 4, and is independently reproduced using git-extracted baseline source
and header (only the relative AEString include spelling is adapted for the
scratch include directory). Its focused JSON equals the original fresh result.
All 204 baseline ARM TUs compiled with zero failures.

## Immutable body denominator and weighting

21 exported symbols map to 19 original addresses. C1/C2 at 0x157040 are one
body, D1/D2 at 0x157340 are one body; D0 at 0x1573ac is a separate body.
Collide and outerCollide have identical code at different addresses and remain
separate bodies. No difficult body is excluded. The committed manifest pins
all symbols, aliases, addresses and instruction weights.

Unweighted unique-body source mean: **96.726316% -> 99.800000%**.
Instruction-weighted source mean: **89.230019% -> 99.206660%**.
Using verifier n_target including normalized literal/data tokens as weights:
**89.229670% -> 99.202747%**. Both weighted interpretations pass >=99.0%.

The instruction weights count normalized decoded Android instruction tokens
excluding `.word/.short/.byte/.raw` data directives; 1,066 total instructions.
Verifier n_target has 1,092 tokens. Weights are fixed from the original binary,
not candidate size. Scores use the verifier's stored per-function one-decimal
values without removing low scores or changing its normalization. Even a
conservative -0.05 percentage-point rounding bound leaves weighted means
99.156660% (instructions) and 99.152747% (all verifier tokens), above 99.0%.

Strict fuzzy unique-body mean: **88.789474% -> 96.968421%**;
instruction-weighted strict: **62.029362% -> 92.733771%**.
This is source-shape recovery, not 99% strict fuzzy or whole-game completion.

## Per-body evidence

All percentages below are source-shape / strict fuzzy. Aliases are displayed
together and excluded only as duplicate addresses in the averages.

| Original address / symbol(s) | ARM instructions | Baseline source / strict | Final source / strict | Linked / byte final |
|---|---:|---:|---:|---|
| 0x157040 `_ZN12PlayerTurretC1EiP6PlayerP10AEGeometryfff` (alias `_ZN12PlayerTurretC2EiP6PlayerP10AEGeometryfff`) | 250 | 93.4 / 78.5 | 98.5 / 98.3 | False / False |
| 0x157340 `_ZN12PlayerTurretD1Ev` (alias `_ZN12PlayerTurretD2Ev`) | 33 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x1573ac `_ZN12PlayerTurretD0Ev` | 5 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x1573bc `_ZN12PlayerTurret8setLevelEP5Level` | 20 | 95.9 / 87.2 | 100.0 / 100.0 | True / False |
| 0x1573f0 `_ZN12PlayerTurret10setScalingEf` | 4 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x1573fc `_ZN12PlayerTurret11setPositionERKN11AbyssEngine6AEMath6VectorE` | 13 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x157418 `_ZN12PlayerTurret14setTurretRangeEi` | 2 | 100.0 / 100.0 | 100.0 / 100.0 | True / True |
| 0x15741e `_ZN12PlayerTurret7setHostEP8KIPlayerRKN11AbyssEngine6AEMath6VectorE` | 4 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x15742c `_ZN12PlayerTurret7getHostEv` | 2 | 100.0 / 100.0 | 100.0 / 100.0 | True / True |
| 0x157432 `_ZN12PlayerTurret5resetEv` | 10 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x157450 `_ZN12PlayerTurret6updateEi` | 270 | 87.8 / 41.2 | 99.3 / 93.7 | False / False |
| 0x15774c `_ZN12PlayerTurret15handleSentryGunEi` | 20 | 100.0 / 100.0 | 100.0 / 100.0 | False / False |
| 0x157782 `_ZN12PlayerTurret12handleTurretEi` | 19 | 100.0 / 100.0 | 100.0 / 100.0 | False / False |
| 0x1577b8 `_ZN12PlayerTurret9pickEnemyEv` | 131 | 75.0 / 66.1 | 99.1 / 96.2 | False / False |
| 0x157908 `_ZN12PlayerTurret14handleRotationEiP10AEGeometryS1_` | 234 | 86.5 / 40.9 | 99.3 / 81.1 | False / False |
| 0x157bcc `_ZN12PlayerTurret6renderEv` | 19 | 100.0 / 100.0 | 100.0 / 100.0 | True / False |
| 0x157c00 `_ZN12PlayerTurret7collideEfff` | 2 | 100.0 / 100.0 | 100.0 / 100.0 | True / True |
| 0x157c04 `_ZN12PlayerTurret12outerCollideEfff` | 2 | 100.0 / 100.0 | 100.0 / 100.0 | True / True |
| 0x157c08 `_ZN12PlayerTurret6reviveEv` | 26 | 99.2 / 73.1 | 100.0 / 73.1 | False / False |

## Accepted forms and source anchors

* Flags are inherited bytes +0x25 (enabled) and +0x3f (sentry), not the former
  fabricated +0x124/+0x125 bool members. +0x124 holds the last frame delta;
  cached position is +0x2c, host-world offset +0x90, aim +0x9c, revive clear
  +0xd8. Header assertions cover these and every +0x128..+0x164 tail field;
  sizeof remains 0x168 on ARM32. Existing calls outside PlayerTurret use none
  of the removed bool member names.
* Constructor @0x157040 calls KIPlayer(mesh, -1, ...), sets enabled/+0x3e,
  uses Globals::Canvas and native child offsets (0,565,-528)/(0,60,0),
  YXZ=2, helper Transform boundingRadius +0xe0 = 1000000, and native pi yaw.
  The 16-byte table @0x157310 is (0,0,0,-1), confirming particleSystemId=-1.
  hostOffset direct member initialization is accepted at 98.5% source /98.3%
  strict versus 98.1/96.0 for individual body assignments.
* Reset @0x157432 was already linked-exact and is unchanged. Revive @0x157c08
  now clears +0xd8, activates via KIPlayer and resets visibility as in ARM.
* Rotation @0x157908 copy-initializes the enemy Matrix, multiplies
  (geometry * main) * turret, uses +/-0.05 deadband, yaw on main, pitch on
  turret, bounds -599/99 and frame delta +0x124. Android literal words confirm
  1/4096 and +/-6.2831854820251465f rather than rounded decompiler 6.2832.
  Lead-distance literal @0x157ba4 is 1500.0f, correcting the old 3000.0f.
  It calls Player::shoot(int,long long,bool,Matrix) with the Matrix by value,
  then Transform::Update(delta,false). Direct original PLT references independently
  confirm _ZN6Player5shootEixbN11AbyssEngine6AEMath6MatrixE at callsite
  0x157b22 and _ZN11AbyssEngine9Transform6UpdateExb at 0x157b40. This preserves the ARM sret/64-bit
  alignment argument contract instead of the old four-argument shoot overload.
* PickEnemy @0x1577b8 reads the +0x69 byte, inherited sentry flag, cached +0x2c
  and both getKIPlayer calls; restores asymmetric factions 8/10/9 and
  pairs 0/1, 3/2. Existing previousEnemy rejection is retained verbatim.
* Update @0x157450 uses Globals::sound/rnd/status instead of null static shims;
  copies host Matrix, writes body Matrix without extra geometry copy;
  emits velocity=(0,0,0), lifetime=-1 and starts explosion with fresh zero.
  Writes cargo +0x4c as a byte before and after createCrate, and clears the
  PlayerEgo +0x14 descriptor's target at +0x1c. The former Player reinterpret
  and KIPlayer wingmanTarget +0xe8 were wrong. The descriptor is named only by
  its observed layout; no unsupported ownership/class identity is asserted.
* Caller check: Level::createStaticObject configures turret meshes 6772/6774/14363,
  invokes virtual setLevel, and explicitly clears inherited +0x25 for mesh14365.
  The new enabled accessor therefore consumes the flag actually written by its
  caller. Level spawn loops continue to assign shipGroup, geometry rotation,
  world position and scaling; no caller or shared header was patched.
* Native state re-read/join and enemy/friend byte stores preserve changes a
  called function may make; the source-shaped control joins raised update
  98.2/58.8 -> 99.3/93.7. They are grounded in the original branches, not
  added volatile variables, arbitrary register assignments or metric-only work.
* SetLevel @0x1573bc reloads level->field_74 after addSystem before disabling
  emission. It rises 95.9/87.2 -> linked-exact 100/100.

## Necessary verifier exception (documented scope extension)

The unmodified f60694dd full corpus reported 7 wrong-type Array C2/D2 names
whose displayed demangled signatures are identical. The native object contains
their COMDAT aliases, but asmdiff.disassemble restricted alias matching to
.text and keyed labels by address alone. Section-relative address0 collides
between separate COMDAT bodies. Minimal fix: recognize .text.* function
sections and key alias lookup by (section,address), with a regression
fixture containing two different bodies at address0 in different sections.
The failing original full report is preserved in _work/full_arm_baseline.json.
This is a proven tooling necessity to satisfy the requested green full-corpus
gate, not a source-shape scoring change. Both baseline and final PlayerTurret
JSON are byte-for-byte equal as parsed JSON before/after the parser fix.
No normalization/formula, delink, compiler settings or other family source
is changed. The patch is isolated here and matches the small already-pending
section/alias fix in the dirty main checkout; integration should apply it once.
Full clean-baseline and candidate corpora are rerun with the identical fixed
parser in separate object/target directories.

## Trials rejected or superseded

* Initial body Vector assignment for hostOffset generated an extra operator=
  call; rejected in favor of member initialization.
* Separate hostOffset component assignments: source 98.1 / strict96.0 ctor;
  superseded by the meaningful class initializer form 98.5/98.3.
* Flat combined hp/state branch and independent faction blocks produced update
  98.2/58.8; superseded by original state join and byte-result lifetime.
* The first package retained rounded rotation constants from the decompiler;
  superseded by ELF literal values even though source-shape masks literal pools.
* No further compiler/volatile/register trial is warranted after both means pass.
* A scratch baseline compilation initially failed because its copied header
  used a relative include outside the scratch tree. That full attempt was
  cancelled, not accepted as a clean baseline; the corrected focused rerun
  exactly reproduces the fresh baseline before restarting the corpus run.

## Validation commands and artifacts

NDK r18b clang7.0.2, canonical -Oz match_flags.sh; local NDK configured through
GOF2_VERIFY_LOCAL_NDK/GOF2_NDK_ROOT/NDK environment only. Native is MSYS2 UCRT64
Debug CMake/Ninja target gof2. Original game data stays ignored in _work/bins.

Focused: `python tools/verify/verify.py --build-dir _work/final-verify
--no-build --unit game/ship/PlayerTurret --only '^_ZN12PlayerTurret'
--report _work/player_turret_final.json`.

All ARM: `bash tools/verify/build_objs.sh _work/final-verify`.
Full corpus: `python tools/verify/verify.py --build-dir _work/final-verify
--no-build --fail-on-wrong-type --report _work/full_arm_final_parser_fixed.json`.

Standalone family gate: `python tools/test_player_turret_gate.py
 docs/validation/player_turret_baseline_2026-10-02.json
 docs/validation/player_turret_final_2026-10-02.json`.
Optional --full-baseline/--full-final compares every original exact pair in
unfiltered reports. Negative self-checks reject missing update and weighted
failure despite a high short-function average.

## Final gate results

* Native UCRT64 Debug gof2 static library: PASS, including final 1500 lead fix.
* All ARM translation units: **204/204**, zero compile failures; all new layout
  assertions are evaluated by the ARM compiler.
* Focused family: **21 symbols, 13 linked-exact, 4 byte-exact**, zero skips.
  The original 12 linked and 4 byte pairs are all preserved; setLevel is gained.
* Full baseline with corrected parser: **4,631 comparisons**, 4,281/4,524
  original names compared, 243 absent, **0 wrong-type, 0 skipped units**;
  **2,100 linked-exact / 964 byte-exact**. --fail-on-wrong-type exits0.
* Full final with identical parser: same **4,631 / 4,281 / 243 / 0 / 0**;
  **2,101 linked-exact / 964 byte-exact**. --fail-on-wrong-type exits0.
* Full pair-set comparison: **zero linked losses and zero byte losses** versus
  BOTH the original-parser baseline (2,079 linked /959 byte) and fixed-parser
  baseline. Sole new linked pair versus fixed baseline is setLevel.
* Regression gate including COMDAT test and negative body/weighted tests: PASS.
  Full reports are unfiltered and have equal row count; no scope/signature gate
  is faked by a family report. Source-shape formula and compiler flags unchanged.
* git diff --check: PASS. No push, PR or merge.

Committed family JSON, immutable manifest and compact full-gate summary live in
`docs/validation/player_turret_*_2026-10-02.json`. Large full reports/logs stay
local at `_work/full_arm_baseline_parser_fixed.json`,
`_work/full_arm_final_parser_fixed.json`, `_work/player_turret_gate_full.log`,
`_work/arm_final_build.log`, `_work/native_final.log`. Their report hashes and
source hashes are recorded in the compact summary. The original failing parser
report is retained as `_work/full_arm_baseline.json`.

## Remaining limits and semantic risks

Source-shape tolerates allocation/scheduling differences and masks literal
pool values/call addresses; actual constants and changed call contracts were
therefore audited separately. Ctor remains98.5%, rotation99.3%, update99.3%,
pickEnemy99.1%; none is claimed byte-exact. Ctor exception cleanup and local
allocation differences remain; revive strict73.1% is register/epilogue drift.
The old aim-point int-storage reinterpret view remains an inherited-model
strict-aliasing boundary, outside this fixed family header's control.
The descriptor's actual owner/type and virtual calls are not runtime-certified.
KIPlayer/AEGeometry/engine initialization contains unrelated unresolved shims;
this native library compile does not establish a playable native executable.
No ARM32 executable or station-to-flight game run is claimed. Android ARM
ABI/layout compilation and binary instruction evidence are the completed
validation layers for this package; a device/runtime test remains separate.
