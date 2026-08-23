# MenuTouchWindow State, Dialog And Options ARM Pass

Date: 2026-08-23

## Scope

This pass continues the Android body of
`MenuTouchWindow::OnTouchEnd(int, int, void *)` at `0x12aabc` after the main
dispatcher pass. It replaces remaining generic-array routing with the confirmed
state bodies for the options root, cargo dialog, game/challenge selector, store
credits selector, and store information dialog.

Primary evidence:

- Android 2.0.16 IDA body at `0x12aabc`;
- the corresponding Ghidra body;
- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/MenuTouchWindow.c`;
- the local ARM target extracted from the lawfully supplied Android binary.

## Recovered State Bodies

### State 3: Options Root

On phone layouts, the `MenuTouchWindow+0xac` button array has a native ID
dispatcher, not generic button consumption:

| Button ID | Resulting menu state |
| ---: | ---: |
| `4` | `4` |
| `7` | `6` |
| `8` | `7` |
| `9` | `8` |
| `25` | `14` |

IDs `5` and `6` deliberately do not select a new state.

On iPad, state `3` is the combined control/audio root. It routes the
`+0xe4` button to HUD layout state `11`, applies control-preset selection,
inversion, accelerometer calibration, audio category toggles, slider
persistence, the scale slider, and the large-iPad extra button in Android
order. The byte at `MenuTouchWindow+0x176` is now named
`accelerometerCalibrationPending`; the source body proves it controls the
GameText `493` calibration confirmation.

### States 10, 12, 13 And 17

- **State `10`** closes the cargo summary only after ChoiceWindow returns `0`:
  it clears `messageShowing`, switches to state `0`, then uses the shared
  Layout tail. The generated cargo text remains source-backed semantically:
  empty cargo uses GameText `286`; entries use `item index + 1274`; the title
  uses GameText `166` plus `current / max t`.
- **State `12`** is the two-button game/challenge selector at `+0xb4`. The
  first button stores fade `0.5`, the second stores `1.5`. It starts the
  selected target immediately when playing time is below one; otherwise it
  opens GameText `52` or `26` and sets the confirmation byte at `+0x174`.
  Confirmation selects base GOF2, Valkyrie, or Supernova through the existing
  `+0x234` context slot.
- **State `13`** consumes an informational ChoiceWindow result `0` and clears
  `messageShowing`; other results flow to the shared Layout tail.
- **State `17`** handles the initial store message and the `+0xb8` credit/DLC
  selector. Its selected index is persisted in `+0x234`; choice 1 launches
  Valkyrie when not already purchased, choice 2 launches Supernova, and the
  remaining route enters state `12`.

## Deliberate Boundary

The Android caller tests the return value of `MissionsWindow::OnTouchEnd` in
state `9`. The currently recovered `MissionsWindow` declaration/body is
`void`, so this pass leaves its existing route intact rather than inventing a
return ABI. A dedicated `MissionsWindow::OnTouchEnd` ARM audit must first
recover that contract and then restore the conditional `menuState = 0` write.

## Verification

- UCRT64 `gof2` links successfully as `cmake-build-ucrt/libgof2.a`.
- ARM object build remains `201/204`; the three failures are the pre-existing
  unrelated `SolarSystem *` versus integer signatures.
- Focused `MenuTouchWindow::OnTouchEnd` comparison: `3.7%`, target/base
  `3001/2675`, not linked- or byte-exact.

The preceding dispatcher checkpoint was `7.4%` at `3001/1856`. This is not a
semantic rollback: the options/dialog bodies are now present and force-inlined
into `OnTouchEnd`, making its instruction count much closer to the Android
body. The remaining fuzzy loss is dominated by block ordering, helper/local
lifetimes, String destruction, and a `132`-byte frame where the original saves
`d8-d13` and reserves `280` bytes.

## Remaining Work

1. Audit `MissionsWindow::OnTouchEnd` return ABI and restore state-9 close
   semantics.
2. Recover state `15/16` DLC modal branches and their application-data fields.
3. Recreate the exact cargo String temporary/destructor graph.
4. Only then tune `OnTouchEnd` stack slots, VFP save set, and switch block
   placement for ARM similarity.
