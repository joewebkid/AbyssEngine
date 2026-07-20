# MGame HUD Action-Routing Pass

Date: 2026-07-20

## Scope

This package recovers the unpaused quick-menu and physical-orbit portions of
Android `MGame::OnTouchEnd(int, int, void *)` at `0x17a144`. The evidence is
the matching body in `analysis/gof2_libgof2hdaa_full_ida.c`, following its
`LABEL_69` call to `Hud::touchEnd`.

It is the source-backed owner of the masks constructed by
`Hud::initHudMenu(0..3)`. `Hud::hudAction` is not an alternate dispatcher: its
Android body at `0x16650c` only returns zero.

DeepOpen J2ME v1.0.4 independently preserves the same five conceptual
orbit-menu entries in `GOF2/AutoPilotList.java` (programmed station, jump gate,
station, asteroid waypoint, route waypoint). It corroborates the menu's
semantic order, but not the Android masks, offsets, or touch control flow; see
`DEEPOPEN_J2ME_CROSSWALK_2026-07-20.md`.

## Confirmed And Applied

| Mask | Android action routed by `MGame::OnTouchEnd` |
| --- | --- |
| `0x200`, `0x400` | Rebuild quick menu as weapon or wingman menu. |
| `0x800` | Close menu and invoke `MGame::useCloak()`. |
| `0x1000` | Invoke `MGame::UseKhadorDrive()`. |
| `0x2000..0x10000` | Select secondary-equipment entry 0..3 and update `Hud`. |
| `0x20000..0x100000` | Send wingman command 1, 3, 2, or 0; command 3 receives `Radar::getLockedEnemy()`. |
| `0x200000` | `LevelScript::setAutoPilotToProgrammedStation()`. |
| `0x400000`, `0x800000` | Autopilot to landmark 1 (jump gate) or 0 (station), with HUD events 12 and 10. |
| `0x1000000`, `0x2000000` | Autopilot to asteroid or player-route waypoint, with HUD events 14 and 13. |
| `0x04000000 << i` (`i = 0..5`) | Set `Radar+0x04`, begin docking target `i`, clear the following three Radar target slots, and release HUD keys. |

The existing local `MGame+0xc8` low-byte slot is now named `orbitMenuOpen`.
The Android body opens it with HUD menu type 3, then closes and resets the
menu/sound state after an orbit/docking choice. The same source branch also
cancels existing autopilot or active asteroid/docking/stream procedures when
the physical orbit button (`0x40`) is pressed.

## Boundaries And Risks

- The source branch before `LABEL_69` handles paused game-over, StarMap,
  dialogue, ChoiceWindow, MenuTouchWindow, and cutscene state. It remains
  outside this package; the local handler returns for those modal pause states,
  while deliberately continuing to `Hud::touchEnd` when its own `hudMenuOpen`
  or `orbitMenuOpen` flag is set.
- Boost, firing, hacking, rocket-control, and auto-turret touch masks share
  this native handler but are not part of this quick-menu package. They remain
  deliberately untouched until their direct input side effects are recovered.
- `Radar+0x24` is reached by the confirmed stream-cancellation path and is
  currently named `lockedStation` in the local `Radar` mirror. Its exact
  stream-target meaning needs a focused Radar/PlayerEgo field audit; this code
  preserves the direct native call path without renaming that field.
- This is source-backed behavioral recovery, not an ARM byte-match claim. The
  host class layout and generated code differ from 32-bit ARM.

## Validation

Run the UCRT64 native compile gate:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```
