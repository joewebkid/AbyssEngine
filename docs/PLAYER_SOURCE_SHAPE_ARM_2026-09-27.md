# Player ARM source-shape pass (2026-09-27)

## Scope and baseline

The fresh 101-symbol `Player` baseline was **95.07% source-shape / 86.42%
strict**, with 48 linked-exact and 43 raw-byte-exact functions. The accepted
sound-global, shoot-wrapper and hit-vector changes bring the focused result to
**96.80% source-shape / 89.98% strict**, 49 linked-exact and 44 byte-exact.
The requested 99.9% average is not reached. Reports are local generated files:
`_work/player_family_baseline_20260927.json`,
`_work/player_sound_globals_trial_20260927.json`,
`_work/player_shoot_wrapper_trial_20260927.json`, and
`_work/player_hitvector_void_trial_20260927.json`.

## Accepted forms

| Source-backed change | Android evidence | Focused result |
| --- | --- | --- |
| Route sound lifecycle through `Globals::sound` and the positional-audio flag through `Globals::options[0x0f]` | `playShootSound` `0xa3a64`, `stopShootSound` `0xa3a14`, engine-sound bodies `0xa4014..0xa40e0`, `update` `0xa4110`; `MGame` identifies `word_2181F7` as options byte `+0x0f` | family 95.07 -> 95.69%; sound functions gain 3.9-14.4 points |
| Pass `Player::transformMatrix` directly in both `shoot` wrappers | Android wrappers `0xa3f30` and `0xa3f9e` copy the 60-byte Matrix by value | family 95.69 -> 96.64%; wrappers 46.5 -> 96.9% and 46.9 -> 92.5% |
| Make `setHitVector` return `void` | Android `0xa2e94` stores three floats; inspected callers discard R0 | family 96.64 -> 96.80%; function is now 3/3 instruction, linked- and byte-exact |

The old wrappers wrote `mat.m[15]` even though `Matrix::m` has only 15
elements. Removing this out-of-bounds write is a runtime correctness fix, not
just a code-shape adjustment. None of the three packages changed the existing
exact functions adversely. All 204 ARM translation units compiled and UCRT64
`gof2` linked `libgof2.a`; the host build still emits pre-existing warnings in
other units.

The final unscoped corpus report,
`_work/full_arm_player_sound_wrapper_hitvector_20260927.json`, completed with
4,626 comparisons, 89.54% corpus source-shape / 76.57% strict, 2,073
linked-exact and 943 byte-exact functions, zero skipped, and zero
`missing_wrong_type`. The 242 absent original symbols remain a separate
recovery issue, not a regression introduced by this package.

The checked iOS engine dumps do not contain these `Player` symbol bodies, and
the installed PC resource build is not a C++ control-flow dump. They provide no
independent ABI confirmation for this package; Android instructions and
callers remain the primary evidence. The extracted Android SDK `Player.c` is
the same lineage of evidence, not an independent cross-check.

## Rejected or deferred forms

- Do not mark the main `shoot` bodies complete by treating the recovered
  `Gun::shoot`/`shootAt` `void` stubs as successful shots. Android
  `Player::shoot` at `0xa3d5c` branches on `Gun::shoot`'s result, and the
  Android `Gun::shoot` at `0x152b24` forwards the result of `shootAt`. Restoring
  that dependency needs a separate source-backed Gun body and return ABI pass.
- Do not fabricate the missing shoot sound-ID tables or the zero-valued
  `k_shoot*`/`k_update*` placeholders. Their current values remain unverified.
- Do not change the public return type of `PauseEngineSound` or
  `StopEngineSound` solely to reproduce register allocation. IDA's inferred
  `int` return could be an incidental R0 value; all inspected C++ callers
  discard it. This is queued for caller/ABI cross-check.

The biggest remaining source-shape deficits are the two large `shoot` bodies
(71.5%, 86.6%), `update` (56.8%), `calcWeaponSounds` (72.1%), and
`damage` (95.6%). Several short functions are below 90%, so a 99.9% family
mean requires more than polishing one overload. Focused reports must be
followed by the full unscoped corpus gate and a native build after each major
accepted package.
