# MGame Paused Touch-Routing Pass

Date: 2026-07-20

## Scope

This pass recovers the first two actionable modal branches before `LABEL_69`
in Android `MGame::OnTouchEnd(int, int, void *)` at `0x17a144`:

- the `autopilotMenuOpen` ChoiceWindow result flow;
- the StarMap close/accept flow, including the stream landing correction.

The primary evidence is Android IDA and Ghidra:

- `analysis/gof2_libgof2hdaa_full_ida.c`, `MGame::OnTouchEnd`, `0x17a144`;
- `analysis/gof2_gidra_engine_full_source.c`, `MGame::OnTouchEnd`,
  `0x18a144`.

DeepOpen's J2ME `MGame` and `AutoPilotList` only corroborate the state names
and orbit-menu intent. They were not used for Android masks, offsets, touch
control flow, or the recovered 3D placement.

## Confirmed And Applied

| State | Confirmed Android behavior now represented locally |
| --- | --- |
| `MGame+0xc5` | `ChoiceWindow::OnTouchEnd`: result `1` opens/reuses `StarMap`, sets post effect `0x1400002`, initializes map lights and jump-map mode, then leaves the game paused. Result `0` clears the menu, exits turret mode, clears `usingJumpDrive`, starts the jump scene, and resets the gun delay. |
| `MGame+0xc7` | The global `Layout` gets first refusal while its visible flag is set. A successful `StarMap::OnTouchEnd` restores the level light, unpauses/resumes sounds, handles map exit versus stream interaction, then destroys the map object. |
| Stream close, non-exit | `dockToStream(false)`, clear autopilot, copy landmark `1` geometry direction to the player geometry with up `(0, 1, 0)`, and position the player at landmark `1` plus `(0, 0, 8000)`. Landmark `1` is independently corroborated by `Level::collideStream` and `LevelScript`'s warp-gate routing. |
| Map exit | A stream exit clears `usingJumpDrive` and calls `startJumpScene`; a non-stream exit invokes `LevelScript::setAutoPilotToProgrammedStation` unless `Level::doInstantJump` is set. |

`dockChoiceOpen` intentionally still falls through to `Hud::touchEnd`, exactly
as the Android body jumps to `LABEL_69` for that state.

## Remaining Paused Branches

- `choiceWindowOpen` includes several mission/jump/campaign-specific actions.
- `cutsceneActive` first needs its direct mission failure/success dispatcher
  separated from the generic dialogue completion route.
- `menuTouchOpen` needs a focused `MenuTouchWindow::OnTouchEnd` application-data
  gate audit.
- Game-over and special `MGame+0xc6/+0xca/+0xcf` ChoiceWindow paths require
  their own evidence packets before they are named or merged.

This is source-backed behavioral recovery, not an ARM byte-match claim.

## Validation

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```
