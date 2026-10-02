# Player ARM recovery — 2026-10-02

Status: **partial; unweighted source-shape passes 99.0%, instruction-weighted source-shape remains below 99.0%. The joint gate is NOT met.**

## Isolation and evidence

Branch `codex/player-arm-99-20261002`, worktree `A:/gof2-player-arm-99-20261002`.
Base `f60694dd0ca578831cec832e7026b32644642d97`; main's dirty source was neither copied nor edited.
This is the requested common clean checkpoint; later independent Gun/PlayerTurret/TargetFollowCamera/StarMap branch commits were not integrated.
The 2026-09-27 Player report and its 96.80% figure describe uncommitted changes absent from this checkpoint.
Fresh checkpoint results, not that historic snapshot, are the baseline here.

Inspected workspace AGENTS.md, CONTEXT_INDEX, PROJECT_MAP, CURRENT_STATUS, TASK_QUEUE,
reverse_engineering_context; native AGENTS.md, README, CONTRIBUTING, PROJECT_STATUS and VALIDATION;
Player.cpp/h, callers in PlayerEgo and Level, relevant Gun/FModSound/Globals/Status/math/Array headers and bodies.
Primary evidence: Android HD **2.0.16 ARMv7** `libgof2hdaa.so`, its original disassembly and the
matching extracted SDK `combat_weapons_damage/Player.c`. The SDK is the same evidence lineage,
not an independent verification. No iOS or PC version is used to prove these ABI changes;
the previous report records absent Player bodies in the checked iOS dumps and PC resource-only inputs.

## Fixed denominator and results

Before editing, original addresses from `android_2.0.16.symbols.tsv` fixed **99 unique bodies / 101 symbols**.
Only C1/C2 at 0x000a2570 and D1/D2 share addresses; their aliases receive no extra weight.
All original target instruction counts stayed unchanged. Weight is baseline `n_target`, including
the verifier's normalized tokens/literal pools, not bytes or our generated instruction count.
Total fixed weight: 2534.

| Metric, 99 unique bodies | Baseline | Final |
|---|---:|---:|
| Source-shape, unweighted | 95.029293% | 99.119192% |
| Source-shape, instruction-weighted | 87.325020% | 97.741279% |
| Strict fuzzy, unweighted | 86.264646% | 94.315152% |
| Strict fuzzy, instruction-weighted | 63.474665% | 77.238319% |
| Linked-exact unique bodies | 47 | 57 |
| Byte-exact unique bodies | 43 | 49 |

Historical-style symbol figures (101, aliases included): source 95.07% ->
99.10%, strict 86.42% -> 94.37%,
linked-exact 48 -> 58, byte-exact 43 -> 49.
They are supplemental, not the gate denominator.

## Accepted changes

- Sound lifecycle uses Globals::sound and options byte +0x0f (0xa3a14..0xa3a64, 0xa4014..0xa42dc).
  Replaced null placeholders and recovered item/group sound ID tables from Android rodata
  0x1fbb50 (233 entries) and 0x1fbf00 (11 entries). calcWeaponSounds uses Globals::items,
  reloads it across Item calls, and registers the selected gun's indexed sound ID.
- Both shoot wrappers copy the member's 60-byte Matrix by value; removed out-of-bounds m[15] writes.
  Main selector at 0xa3d5c uses argument a for the slot, b for item ID, and passes pos/flag to Gun.
  Restored ignition mask 0x10000003, 0.008 shake increment, supplied Matrix for positional shot audio,
  and reload of the gun array after ignite callbacks. Constructor initializes its Matrix member once.
- EMP recovery at 0xa4110 uses duration +0xe4 and elapsed +0xe8, byte disabled flag,
  and full-width elapsed reset in damageEmp. Cumulative EMP damage +0xdc is int32, not uint8;
  resetDamageDoneByPlayer now clears all four bytes. ARM asserts lock these offsets and sizeof 0x114.
- update uses actual MatrixGetPosition and Vector arithmetic; velocity is
  Player::velocity * (current - previous) / dt; engine sound index comes from +0xf4.
  PlayEngineSound stores its integer sound ID, passes null velocity and restart=true.
  Decay is dt * -0.001 * 0.025, clamped with the original ARM NEON vmax operation;
  host uses std::max. ARM NaN behavior remains relevant; host smoke covers finite decay.
- damage at 0xa31c0 restores normal/hardcore thresholds 0.33/0.1 and 0.66/0.4,
  friendTurnedEnemy(shipGroup), killWanted(wanted ID), text 1664, enemy low byte,
  destroyed byte, 0.065 shake, and the two-byte wanted-state value at Status+0x110 via memcpy.
- reset at 0xa2a70 clears damaged, EMP damage and turnedEnemy, preserves spawnedFlag,
  and uses byte writes where the original does. damageGamma at 0xa2f62 uses <=0 (ARM `ls`),
  retaining NaN; removed the unsupported float return. Inspected Level caller discards its return.
  update/setHitVector/replaceGuns also use void: no recovered meaningful return is fabricated.
- removeAllGuns avoids the non-original reallocation between release and destruction.

## Continuation after c5874214

- setEnemies now calls the real Gun::setEnemies with the copied array, matching original R1;
  removed the one-argument placeholder. ArrayAdd copies the complete source array in one operation.
  addEnemy uses the same original bulk copy instead of repeated reallocations.
- Constructor initializes damaged=0 alongside active=1 and uses the actual math MatrixGetPosition.
- Removed three discarded getSystem calls absent from the original damage/damageEmp bodies.
  damageEmp reads/writes the EMP byte and writes only the medal byte at Status+0x138;
  its asteroid predicate likewise reads the original low byte.
- isAsteroid/isGasCloud read only the low byte; gunAvailable checks the original unsigned slot bound
  and the actual pointer, preventing negative index and host pointer-width truncation.
- playShootSound reloads Globals::sound after isPlaying; stopShooting's channel guard encloses
  the existing body, preserving behavior and the original common epilogue shape.
- Focused stages: `_work/player_resume_set_enemies.json`, `_work/player_resume_damage_calls.json`,
  `_work/player_resume_ctor.json`, `_work/player_resume_sound_reload.json`,
  `_work/player_resume_add_enemy.json`, `_work/player_resume_byte_bounds.json`,
  `_work/player_resume_stop_guard.json`.

## Trials and unresolved dependencies

- Initial package: source shape 98.268687% unique / 95.862076% weighted, no exact loss.
  Explicit update vector intermediates plus NEON vmax improved update 96.0 -> 97.8%.
  Globals item reload and selected-Gun local improved calcWeaponSounds 94.9 -> 98.4%.
- heal loop cast form was tried: identical 88.4% shape, no claimed improvement.
  An isolated disassembly at 0xa42dc proves an unconditional PUSH, while the delinked target
  reports pushcc, apparently from objdump IT state leaking from a preceding decoded literal pool.
  There is also a real compiler prologue/shrink-wrap difference. Common verifier normalization
  was not changed, and neither reported mean is recalculated to hide that limitation. No volatile, dead branch,
  register binding, compiler-flag change or alias inflation was used.
- damageGamma's semantic/return fix changes shape 91.6 -> 90.3%; accepted for the proven NaN
  predicate and removal of an unsupported return, not for metric improvement. No exact was lost.
- A typed Hud* local trial was rejected: PlayerEgo::getHUD is still int in this checkpoint.
  Restoring that foreign API is outside Player ownership. Existing 64-bit pointer truncation
  through getHUD/getCampaignMission and the legacy getPosition MatrixGetPosition shim remain risks. The setEnemies shim was removed.
- **Gun blocker:** original Player at 0xa3e8a tests R0 after Gun::shoot; shootAt likewise returns
  success at 0xa3b20. The checkpoint Gun headers/bodies declare void. Player still cannot
  reproduce the original success/failure-dependent sound, timer, shake and return flow safely.
  No fabricated bool-return ABI alias was introduced. The baseline unconditional post-shot
  path still exists and is explicitly incomplete, including newly restored constants; it must
  not be interpreted as validated success behavior. Gun owner must
  provide a separately verified return contract, then Player bodies/callers can be completed.
  Player's public shoot return remains void pending this cross-family contract.
- Reset/damage health-rate math can still divide by zero with zero maximum armor/shield/EMP;
  original behavior was preserved. Audio/world gameplay was not executed end to end.
  Engine index +0xf4 retains the legacy pointer-shaped field name/type for compatibility;
  own code uses its proven integer interpretation. No runtime proof for foreign shims is claimed.

## Validation

- Focused reports after each package: `_work/player_package1.json` through package8,
  plus `_work/player_final.json` and continuation `_work/player_resume_stop_guard.json`. The fixed list is `_work/player_fixed_bodies.json`.
- Native UCRT64 `gof2`: clean worktree configure, complete build, and final rebuild all succeed;
  output `cmake-build-ucrt/libgof2.a`. Pre-existing warnings in other units remain.
- All **204/204** ARM translation units recompiled using the canonical NDK r18b clang flags:
  `_work/all_arm_resume.log`, failed []. ARM header static assertions passed.
- `python tools/test_player_recovery.py --so _work/bins/android_2.0.16_libgof2hdaa.so` passes.
  It compiles actual focused Player bodies extracted from the current source, with explicit audio,
  Matrix-constructor and Gun constructor/destructor/setEnemies boundary doubles. Tests EMP duration/elapsed separation,
  equality boundary, finite shake clamp, timer clear, full EMP damage reset, spawnedFlag preservation,
  gamma zero/NaN handling, hit-vector writes, poisoned-memory constructor initialization,
  negative/out-of-range gun slot, independent enemy-list copies and propagation to Gun,
  addEnemy for empty/populated lists, and audio-object reload after isPlaying (playing and stopped branches). Both sound tables match the original bytes.
  This is a focused host regression test, not full native executable gameplay or ARM runtime proof.
- MSYS fork failures were bypassed by uncommitted worktree-local scripts invoking the same installed
  NDK executables directly; common tools/configs were not modified. Report formulas were unchanged.

Full corpus, exact **(unit, symbol)** pair-set comparison, not just counts:

| Metric | Baseline | Final |
|---|---:|---:|
| Comparisons | 4619 | 4621 |
| Source-shape mean | 88.99% | 89.09% |
| Strict fuzzy mean | 75.97% | 76.15% |
| Linked-exact | 2079 | 2091 |
| Byte-exact | 959 | 965 |
| Skipped | 0 | 0 |
| Wrong signatures | 7 | 7 |
| Missing | 250 | 250 |

The continuation adds two comparison rows in the Player unit: the naturally emitted
ArrayAdd<Player*> bulk-copy helpers (pointer/count and array overloads), both linked-exact.
Their original addresses were already represented elsewhere: compared_unique remains 4274.
They are not Player member bodies and are excluded from the fixed 99-body gate.
Existing non-Player member/helper comparison metrics are unchanged against c5874214.

Lost linked-exact pairs: 0. Lost byte-exact pairs: 0.
Also zero linked/byte exact-pair losses against c5874214. Continuation gains:
6 linked and
3 byte pairs.
Gained linked pairs: 12; gained byte pairs:
6. Existing missing/wrong-signature gaps are
checkpoint limitations and were not repaired by touching foreign families.
Reports `_work/full_baseline.json`, `_work/full_final.json` (previous commit), and
`_work/full_resume.json` (continuation) remain generated local evidence.

## Remaining weighted gap

The fixed weighted score is 97.741279%, leaving
1.258721 percentage points to the 99.0% threshold.
The largest contributions to loss from 100% are primary Matrix shoot (0.604104 points),
damage (0.281768), secondary Matrix shoot (0.241081), constructor (0.210971),
heal (0.132755), update (0.131965), and playShootSound (0.128966).
This ranks the remaining work; it does not assign every mismatch to a semantic error.
Constructor comparison immediates and scheduling, Pause/StopEngineSound shrink-wrapped
prologues, and update stack/vector register allocation remain compiler-shape differences.
No volatile padding, dead branches, register bindings, or alias/denominator inflation was added.

## Every fixed unique body

L/B columns show baseline -> final linked/raw-byte exact flags (1=true).
Addresses and aliases were frozen before edits. A changed metric alone is not a semantic proof.

| Android address | Symbol (aliases share row) | Original instructions | Source baseline -> final | Strict baseline -> final | L | B | Outcome |
|---|---|---:|---:|---:|---|---|---|
| 0x000a2570 | `_ZN6PlayerC1Eiiiii`, `_ZN6PlayerC2Eiiiii` | 162 | 94.0 -> 96.7 | 88.1 -> 94.4 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a2730 | `_ZN6Player16updateDamageRateEv` | 34 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2858 | `_ZN6Player11getPositionEv` | 5 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 0 -> 0 | unchanged |
| 0x000a2864 | `_ZN6PlayerD1Ev`, `_ZN6PlayerD2Ev` | 50 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 0 -> 0 | unchanged |
| 0x000a29c6 | `_ZN6Player18setShootingEnabledEb` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a29cc | `_ZN6Player10isAsteroidEv` | 9 | 95.6 -> 100.0 | 88.9 -> 100.0 | 0 -> 1 | 0 -> 1 | accepted |
| 0x000a29e2 | `_ZN6Player10isGasCloudEv` | 9 | 95.6 -> 100.0 | 88.9 -> 100.0 | 0 -> 1 | 0 -> 1 | accepted |
| 0x000a29f8 | `_ZN6Player14setNeverAttackEb` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a29fe | `_ZN6Player15doesNeverAttackEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a04 | `_ZN6Player14setAlwaysEnemyEb` | 5 | 100.0 -> 100.0 | 40.0 -> 40.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2a14 | `_ZN6Player15setAlwaysFriendEb` | 6 | 100.0 -> 100.0 | 66.7 -> 66.7 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2a28 | `_ZN6Player14isAlwaysFriendEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a2e | `_ZN6Player13isAlwaysEnemyEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a34 | `_ZN6Player10setEmpDataEii` | 13 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 0 -> 0 | unchanged |
| 0x000a2a56 | `_ZN6Player17setPlayShootSoundEbi` | 3 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a60 | `_ZN6Player9setRadiusEi` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a64 | `_ZN6Player11setKIPlayerEP8KIPlayer` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a6a | `_ZN6Player11getKIPlayerEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2a70 | `_ZN6Player5resetEv` | 28 | 93.7 -> 100.0 | 70.2 -> 100.0 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a2acc | `_ZN6Player10setEnemiesEP5ArrayIPS_E` | 53 | 93.6 -> 100.0 | 80.4 -> 100.0 | 0 -> 1 | 0 -> 0 | accepted |
| 0x000a2b76 | `_ZN6Player8setEnemyEPS_` | 25 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 0 -> 0 | unchanged |
| 0x000a2be2 | `_ZN6Player10addEnemiesEP5ArrayIPS_E` | 50 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2c6a | `_ZN6Player8addEnemyEPS_` | 37 | 90.2 -> 100.0 | 69.9 -> 100.0 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a2cd2 | `_ZN6Player10getEnemiesEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2cd6 | `_ZN6Player8getEnemyEi` | 4 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ce0 | `_ZN6Player9getRadiusEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ce4 | `_ZN6Player12setHitpointsEi` | 6 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2cf6 | `_ZN6Player11setShieldHPEi` | 10 | 96.0 -> 96.0 | 80.0 -> 80.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d1c | `_ZN6Player10setArmorHPEi` | 6 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d30 | `_ZN6Player10setGammaHPEi` | 15 | 93.1 -> 93.1 | 75.9 -> 75.9 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d64 | `_ZN6Player15setMaxEmpPointsEi` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d6c | `_ZN6Player15setMaxHitpointsEi` | 3 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d76 | `_ZN6Player14setMaxShieldHPEi` | 5 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d8a | `_ZN6Player13setMaxArmorHPEi` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2d92 | `_ZN6Player12getEmpPointsEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2d96 | `_ZN6Player15getMaxEmpPointsEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2d9c | `_ZN6Player14getMaxShieldHPEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2da2 | `_ZN6Player11getShieldHPEv` | 4 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2db0 | `_ZN6Player13getCombinedHPEv` | 10 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2dd6 | `_ZN6Player10getArmorHPEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ddc | `_ZN6Player13getMaxArmorHPEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2de2 | `_ZN6Player10getGammaHPEv` | 4 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2df0 | `_ZN6Player16regenerateShieldEf` | 11 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2e1a | `_ZN6Player16regenerateShieldEv` | 11 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2e44 | `_ZN6Player15regenerateArmorEv` | 7 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2e58 | `_ZN6Player14regenerateHullEv` | 8 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2e6c | `_ZN6Player12getHitpointsEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e70 | `_ZN6Player15getMaxHitpointsEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e76 | `_ZN6Player13getDamageRateEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e7c | `_ZN6Player16getEmpDamageRateEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e82 | `_ZN6Player19getShieldDamageRateEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e88 | `_ZN6Player18getArmorDamageRateEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e8e | `_ZN6Player13setVulnerableEb` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2e94 | `_ZN6Player12setHitVectorEfff` | 3 | 83.6 -> 100.0 | 28.6 -> 100.0 | 0 -> 1 | 0 -> 1 | accepted |
| 0x000a2e9a | `_ZN6Player12setBombForceEf` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ea0 | `_ZN6Player11setEmpForceEf` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ea6 | `_ZN6Player12getBombForceEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2eac | `_ZN6Player11getEmpForceEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2eb2 | `_ZN6Player12getHitVectorEv` | 5 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ec2 | `_ZN6Player15getGunRegenRateEi` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2ec6 | `_ZN6Player10damageHullEi` | 22 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2f04 | `_ZN6Player10damageShipEi` | 5 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a2f10 | `_ZN6Player12damageShieldEi` | 26 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a2f62 | `_ZN6Player11damageGammaEf` | 16 | 91.6 -> 90.3 | 54.5 -> 66.7 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a2f96 | `_ZN6Player23resetDamageDoneByPlayerEv` | 5 | 92.0 -> 100.0 | 80.0 -> 100.0 | 0 -> 1 | 0 -> 1 | accepted |
| 0x000a2fa4 | `_ZN6Player9damageEmpEib` | 194 | 94.6 -> 98.7 | 90.3 -> 95.4 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a31c0 | `_ZN6Player6damageEibi` | 420 | 95.6 -> 98.3 | 39.3 -> 38.9 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a3694 | `_ZN6Player6damageEi` | 3 | 100.0 -> 100.0 | 100.0 -> 100.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a369e | `_ZN6Player11turnedEnemyEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a36a4 | `_ZN6Player9turnEnemyEv` | 3 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a36ac | `_ZN6Player12gunAvailableEi` | 16 | 93.3 -> 100.0 | 93.8 -> 100.0 | 0 -> 1 | 0 -> 1 | accepted |
| 0x000a36ce | `_ZN6Player6isDeadEv` | 6 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a36da | `_ZN6Player9isDamagedEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a36e0 | `_ZN6Player9setActiveEb` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a36e6 | `_ZN6Player8isActiveEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a36ec | `_ZN6Player6addGunEP3Guni` | 36 | 100.0 -> 100.0 | 91.7 -> 91.7 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a3774 | `_ZN6Player16calcWeaponSoundsEi` | 148 | 72.1 -> 98.4 | 52.4 -> 87.2 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a3910 | `_ZN6Player6addGunEP5ArrayIP3GunEi` | 43 | 100.0 -> 100.0 | 93.0 -> 93.0 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a3986 | `_ZN6Player13removeAllGunsEv` | 13 | 87.7 -> 100.0 | 92.9 -> 100.0 | 0 -> 1 | 0 -> 0 | accepted |
| 0x000a39a6 | `_ZN6Player11getGunSlotsEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a39aa | `_ZN6Player13resetGunDelayEi` | 21 | 100.0 -> 100.0 | 90.5 -> 90.5 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a39dc | `_ZN6Player14refillGunDelayEi` | 22 | 100.0 -> 100.0 | 90.9 -> 90.9 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a3a14 | `_ZN6Player14stopShootSoundEii` | 30 | 72.0 -> 95.3 | 45.8 -> 93.3 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a3a64 | `_ZN6Player14playShootSoundEiiPN11AbyssEngine6AEMath6VectorEf` | 76 | 71.3 -> 95.7 | 32.0 -> 68.8 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a3b20 | `_ZN6Player5shootEixbN11AbyssEngine6AEMath6MatrixE` | 149 | 86.6 -> 95.9 | 42.5 -> 56.4 | 0 -> 0 | 0 -> 0 | accepted partial; Gun return unresolved |
| 0x000a3cbc | `_ZN6Player12stopShootingEi` | 31 | 98.7 -> 100.0 | 87.1 -> 90.3 | 0 -> 0 | 0 -> 0 | unchanged |
| 0x000a3d08 | `_ZN6Player12stopShootingEii` | 34 | 93.0 -> 100.0 | 70.4 -> 91.2 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a3d5c | `_ZN6Player5shootEiixbN11AbyssEngine6AEMath6MatrixE` | 178 | 71.5 -> 91.4 | 38.2 -> 34.3 | 0 -> 0 | 0 -> 0 | accepted partial; Gun return unresolved |
| 0x000a3f30 | `_ZN6Player5shootEixb` | 39 | 46.5 -> 96.9 | 5.7 -> 93.5 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a3f9e | `_ZN6Player5shootEiixb` | 39 | 46.9 -> 100.0 | 9.3 -> 100.0 | 0 -> 1 | 0 -> 0 | accepted |
| 0x000a4014 | `_ZN6Player15PlayEngineSoundEiPN11AbyssEngine6AEMath6VectorE` | 43 | 95.2 -> 100.0 | 46.3 -> 100.0 | 0 -> 1 | 0 -> 0 | accepted |
| 0x000a4088 | `_ZN6Player16PauseEngineSoundEv` | 13 | 75.7 -> 90.1 | 43.5 -> 74.1 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a40ac | `_ZN6Player17ResumeEngineSoundEb` | 19 | 87.6 -> 98.9 | 70.6 -> 89.5 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a40e0 | `_ZN6Player15StopEngineSoundEv` | 15 | 78.7 -> 90.9 | 22.2 -> 77.4 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a4108 | `_ZN6Player14GetEngineEventEv` | 2 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a4110 | `_ZN6Player6updateEib` | 152 | 56.8 -> 97.8 | 39.7 -> 83.8 | 0 -> 0 | 0 -> 0 | accepted |
| 0x000a42dc | `_ZN6Player4healEf` | 29 | 88.4 -> 88.4 | 73.3 -> 73.3 | 0 -> 0 | 0 -> 0 | no-gain trial; semantics unchanged |
| 0x000a433a | `_ZN6Player19pitchAllPrimaryGunsEf` | 16 | 100.0 -> 100.0 | 100.0 -> 100.0 | 1 -> 1 | 1 -> 1 | unchanged |
| 0x000a4360 | `_ZN6Player11replaceGunsEiiiiib` | 1 | 76.7 -> 100.0 | 66.7 -> 100.0 | 0 -> 1 | 0 -> 1 | accepted |
