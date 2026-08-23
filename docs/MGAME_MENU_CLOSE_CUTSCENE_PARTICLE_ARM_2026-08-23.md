# MGame Menu Close, Cutscene, And Particle ARM Pass

Date: 2026-08-23

## Scope

This package restores the shared tail reached after
`MenuTouchWindow::OnTouchEnd` closes the pause menu inside
`MGame::OnTouchEnd(int, int, void *)`.

Primary evidence:

- Android 2.0.16 `MGame::OnTouchEnd` at `0x17a144` in
  `analysis/gof2_libgof2hdaa_full_ida.c`;
- the independent Ghidra body in
  `analysis/gof2_gidra_engine_full_source.c`;
- local ARM disassembly from
  `_work/bins/android_2.0.16_libgof2hdaa.so`;
- the matching NDK r18b object produced by `tools/verify/build_objs.sh`.

## Recovered Close Tail

When `MenuTouchWindow::OnTouchEnd` returns nonzero, Android performs the
following shared sequence:

1. clear `MGame+0x1a6` and the pause flag at `MGame+0x5d`;
2. call `MGame::resumeSounds`;
3. inspect FMOD category `2` and the saved low byte at `MGame+0x1a4`;
4. route player and enemy engine audio through `ResumeEngineSound`,
   `StopEngineSound`, or `PlayEngineSound`;
5. clear menu/touch state at `MGame+0xc9`, `+0x98`, `+0x9c`, and `+0xc0`;
6. reinitialize the current `StarSystem` light;
7. call `Level::enableParticleEffects` with the native quality thresholds;
8. consume the first byte of `MenuTouchWindow` as the skip request;
9. clear free-camera mode at `MGame+0x15e` and process cinematic mode.

The engine-sound branch is asymmetric in the original. A currently enabled
category with a previously enabled snapshot resumes sounds. A currently
disabled category with a previously enabled snapshot stops sounds. All other
cases start the engine loops through `PlayEngineSound`.

## Packed Options Evidence

The particle quality is not a pointer-owned field. Android exports
`Globals::options` at `0x2181e8`, and the decompiler label `flt_218210` is
exactly `Globals::options+0x28`. The matching object now emits a direct
`vldr [options,#0x28]` before `Level::enableParticleEffects`.

The earlier local placeholder incorrectly treated an unrelated integer as a
pointer to a synthetic particle object. This package replaces that model with
an offset-locked `MGameOptionsView`:

| Options offset | Android label | Confirmed use |
| --- | --- | --- |
| `+0x0f` | low byte of `word_2181F7` | full effects/positional engine path used during `OnInitialize` |
| `+0x28` | `flt_218210` | particle quality slider consumed by `MGame::OnTouchEnd` |

The exact user-facing name of `+0x0f` remains open. Its runtime behavior is
confirmed, but it must not be renamed to a narrower audio-only or
particle-only setting without a wider settings audit.

The close thresholds are source-backed:

- emit particles when quality is greater than `0.0f`;
- render particles when quality is greater than `0.7f`.

`MGame::OnInitialize` uses a different emit threshold (`>= 0.25f`) in the
Android body. The two call sites must not be folded into one guessed policy.

## Skip Producer And Campaign Routes

The first byte of `MenuTouchWindow` is set when the main-state button pair
`(id=18, secondary=0)` succeeds. The main-state touch path then returns
nonzero to `MGame`; ordinary main-state releases continue through the native
Layout/back tail.

`MGame` clears the request before dispatch and restores these routes:

- campaign IDs `154`, `157`, and `158`: call `LevelScript::skipCutscene`;
- campaign ID `1`: clear `Globals::switch_to_target_setting`, deactivate
  `MGame`, and switch to module `5`;
- campaign ID `0`: advance the campaign, set kills to `3`, select target
  setting `1`, deactivate `MGame`, switch to module `2`, and clear
  `Level::initStreamOutPosition`.

The compact Android predicate is `(mission - 154) <= 4` followed by mask
`0x19`; it is preserved rather than expanded into a speculative range.

## Cinematic And Free-Camera Tail

After the close path, `MenuTouchWindow::inCinematicMode` controls
`MGame::setCinematicMode` and clears `MGame+0xf8`. Entering cinematic mode
also reinitializes the current star-system light. If the menu remains open and
free-camera mode is active while cinematic mode is false, Android disables
cinematic mode and clears the same HUD touch field.

The follow-up audit in `MENUTOUCH_MAIN_DISPATCH_FREECAM_ARM_2026-08-23.md`
resolves this boundary. Native `MGame::freeCamTouchEnd` does not consume either
coordinate argument, so the compiler intentionally leaves `r1/r2` unreloaded;
only `this`, touch ID, and stored drag deltas are live.

## Verification

- UCRT64 `gof2` target links successfully as `libgof2.a`.
- ARM corpus remains `201/204`; the three failures are the existing unrelated
  `SolarSystem *` versus integer type errors.
- `MGame::OnTouchEnd`: `8.5%`, `2863/2358` target/base instructions.
- `MGame::OnInitialize`: `14.1%`, `771/182` instructions; the packed
  effects/particle fragment is restored, but most of this function remains.
- `MenuTouchWindow::OnTouchEnd`: `5.4%`, `3001/1274` instructions.
- neither function is linked-exact or raw-byte-exact.

The earlier success/cargo package measured `9.2%` at `2863/2169`. The new
large tail raises generated coverage by 189 instructions but changes block
ordering and whole-sequence alignment, so the fuzzy score is lower. This is a
source-backed behavior gain, not an ARM byte-match claim.

## Remaining Work

1. Audit the complete `MGame::OnTouchEnd` block order and stack/local lifetime
   instead of tuning isolated fuzzy matches.
2. Continue `MenuTouchWindow::OnTouchEnd` whole-switch String and stack-lifetime
   work after the main dispatcher recovered by the follow-up pass.
3. Audit the full packed `Globals::options` record before assigning semantic
   names to every neighboring byte and float.
