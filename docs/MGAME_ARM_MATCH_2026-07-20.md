# MGame OnTouchEnd ARM Match Baseline

Date: 2026-07-20

## Scope

This note starts focused ARMv7 validation of
`_ZN5MGame10OnTouchEndEiiPv` (`MGame::OnTouchEnd(int, int, void *)`) against
the Android 2.0.16 body at `0x17a144`.

Inputs are local and ignored by Git:

- `_work/bins/android_2.0.16_libgof2hdaa.so`;
- `_work/symbols/android_2.0.16.symbols.tsv`;
- `_work/symbols/android_thumb_map.tsv`.

The body and the paused StarMap/autopilot routes are independently visible in
`analysis/gof2_libgof2hdaa_full_ida.c` and
`analysis/gof2_gidra_engine_full_source.c`. The behavioral recovery is
documented separately in `MGAME_PAUSED_TOUCH_ROUTING_2026-07-20.md`.

## Local ARM Runner

The Windows/MSYS local-NDK fallback is now usable from this workspace, whose
absolute path contains spaces. `match_flags.sh` represents each compiler flag
as one newline-delimited argument and `build_objs.sh` reconstructs a Bash
array before invoking clang. `verify.py --unit <unit>` limits a `--show`
operation to a known base object, avoiding an expensive scan of every object.

The focused command is:

```powershell
$env:PATH = 'C:\msys64\usr\bin;C:\msys64\ucrt64\bin;' + $env:PATH
$env:GOF2_VERIFY_LOCAL_NDK = '1'
$env:GOF2_NDK_ROOT = (Resolve-Path '.cache\ndk\android-ndk-r18b')
C:\msys64\ucrt64\bin\python.exe tools\verify\verify.py `
  --build-dir cmake-build-match\verify --no-build `
  --unit game/menu/MGame --show _ZN5MGame10OnTouchEndEiiPv
```

The matching object was compiled with the local NDK r18b clang 7.0.2 and the
canonical `-Oz` flag set. This is a focused result only; it does not update the
checked-in project-wide `report.json` snapshot.

## Baseline

| Revision point | Fuzzy match | Linked-exact | Raw bytes | Original instructions | Built instructions |
| --- | ---: | --- | --- | ---: | ---: |
| Before this pass | 3.4% | no | no | 2863 | 1109 |
| After source-backed prologue correction | 2.4% | no | no | 2863 | 1096 |

The prologue correction follows the Android body directly: equality of
`MGame+0xc0` and the touch id clears only `MGame+0xc0`; it does not reset the
three adjacent pause-state bytes. The Android body also assumes initialized
`player` and `hud` pointers, so the host-side defensive early return was
removed from this matching path.

The lower fuzzy score is not a negative byte-match result by itself. This
large function has different block ordering, stack/local lifetime, and more
than 1700 missing normalized instructions, so a small early edit shifts the
whole-sequence alignment. Neither linked equality nor raw byte equality
regressed; both were already false. The correction remains source-backed but
is not called a match improvement.

## Pause/StarMap Findings

The Android body keeps the autopilot ChoiceWindow and StarMap close paths in
`MGame::OnTouchEnd`, not in separate leaf functions. In particular it:

- tests `MGame+0xc5` for the autopilot ChoiceWindow path;
- opens/reuses `StarMap` at `MGame+0x90`, sets post effect `0x1400002`, calls
  `initLights`, enables jump-map mode, and marks `MGame+0x5d/+0xc7` paused;
- gives the global Layout first refusal before `StarMap::OnTouchEnd`;
- restores stream position from landmark `1` using direction plus up
  `(0, 1, 0)` and position offset `(0, 0, 8000)`;
- clears pause/map state, resumes sound, then deletes the StarMap object.

Those behaviors are already source-backed in the local C++ body. A forced
inline experiment for the three helper boundaries produced the same focused
machine-code size and score, so it was deliberately not retained.

## Next Match Work

1. Recover the complete paused dispatcher in native block order, including the
   selector-specific ChoiceWindow paths before attempting more local tuning.
2. Bring the terminal dialogue/cutscene and game-over branches into the same
   body pass, retaining the original String locals and lifetime boundaries.
3. Only then de-shim the remaining `OnTouchEnd` helper boundaries and compare
   the whole function after every coherent block-order change.

This is the start of a byte-match campaign, not a claim that the function is
already close to ARM identity.
