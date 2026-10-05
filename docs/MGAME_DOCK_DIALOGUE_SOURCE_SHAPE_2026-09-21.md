# MGame Dock And Dialogue ARM Pass

Date: 2026-09-21

## Sources

Android IDA `analysis/gof2_libgof2hdaa_full_ida.c`:

- `MGame::OnTouchEnd(int,int,void*)` at `0x17a144`, including the StarMap,
  autopilot ChoiceWindow, dialogue and menu-close branches.
- `MGame::dockEvent(int,int)` at `0x17f920`, especially the repeated
  `MGame+0xcb` stream-collision check and programmed-station dialogue.

The ARM disassembly diff, not an apparent similarity of C++ expressions,
determines which source forms are retained. The Android body is primary;
the extracted decompilation is not claimed as an original source file.

## Accepted

- `dockEvent` checks `touchesStream` again after the `PlayerEgo` stream-docking
  calls. The earlier C++ outer `if` incorrectly assumed those calls could not
  change the field. When the flag clears, execution now reaches the station
  collision path, as in Android.
- `dockEvent` calls `isAutoPilot()` before testing `Level::programmedStation`,
  matching the Android call order rather than short-circuiting the call.
- The StarMap touch-end path directly uses its active `Layout`, `StarMap` and
  `StarSystem`; Android does not have the three nullable early exits removed
  in this pass. The stream restore uses the native `Vector + (0,0,8000)`
  operation, although that expression alone does not move rounded similarity.
- The autopilot ChoiceWindow path no longer has a nullable early exit absent
  from Android. The dialogue path resets pause/sound/cutscene state before
  reading the mission, then obtains the current mission again for the fail/win
  tests. These changes preserve the existing reward/failure helper bodies.

| Symbol | Previous fuzzy/source | Current fuzzy/source | Target/base instructions |
| --- | ---: | ---: | ---: |
| `_ZN5MGame9dockEventEii` | 70.8% / 92.3% | 78.8% / 94.4% | 477 / 503 |
| `_ZN5MGame10OnTouchEndEiiPv` | 26.5% / 81.2% | 32.0% / 81.3% | 2769 / 2666 |

## Rejected Or Deferred

- Two separate position `Vector` locals in `dockEvent`, whether in one or
  two scopes, moved the stack layout and lowered source-shape to 92.5% with
  the accepted collision recheck. Directly passing two `getPosition()`
  results yielded 92.6%. The existing single local is retained pending a
  broader stack-lifetime pass.
- Merging the two programmed-station ChoiceWindow bodies through a common
  label reduced `dockEvent` to 87.9% source-shape; moving the label earlier
  produced the same code. Both forms were reverted. Android's shared
  `LABEL_17` remains an evidence-backed control-flow target, not a current
  byte-match claim.
- Removing all internal guards in the StarMap stream-position helper reduced
  `OnTouchEnd` to 80.4% source-shape and 12.1% fuzzy. The native path has no
  such guards, but the local edit was reverted pending a wider function-body
  rewrite.
- Removing the MenuTouchWindow and two StarSystem nullable guards in the
  menu-close helper left source-shape at 81.3% but reduced fuzzy matching to
  11.1%; reverted. The conditional player check in `StopEngineSound` is
  native and was never removed.
- Explicit touch-slot `goto` labels in `freeCamTouchEnd` compiled to the same
  instructions as the existing readable form; reverted as churn.

## Verification And Remaining Work

- Focused ARM compile succeeded. The final `_ZN5MGame` report has 45
  comparisons, average fuzzy 89.0%, average source-shape 97.6%, 16
  linked-exact and 9 byte-exact. The preceding package was 88.7% fuzzy and
  97.6% source-shape; the one-decimal family source average does not yet move.
- UCRT64 `gof2` linked `libgof2.a`. The two existing empty non-void key
  handler warnings remain.
- Full ARM verification with `--no-build` completed: 4681 comparisons,
  average fuzzy 75.4%, average source-shape 87.4%, 4130/4524 original
  functions compared. It used the freshly compiled MGame object and cached
  objects for other units, not a fresh full-corpus rebuild.
- Reports: `_work/mgame_family_dock_dialogue_final_20260921.json` and
  `_work/mgame_full_corpus_dock_dialogue_20260921.json` (local outputs).

The requested 99% MGame-family average is **not reached**. `OnTouchEnd`
remains 81.3% source-shape; its large modal/mission/HUD control-flow and
stack-local lifetime are the highest-impact next audit. No ARM byte-match
is claimed for either changed method.
