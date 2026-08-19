# Hud quick-menu initialization and event pass (2026-08-20)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` ARM code and symbols from `_work/bins` and
  `_work/symbols`.
- IDA/Ghidra-backed bodies in
  `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`.
- Focused ARM comparisons for the changed functions and the UCRT64 native
  build.

## Recovered behavior and source shape

- `initHudMenu` now follows all four native menu modes: equipment actions,
  installed-equipment selection, pause/status actions and orbit/docking
  targets.
- The equipment list restores the native seven-object String expression
  `name + String(" (" + amount + ")", false)` and preserves the original
  action mapping `0x2000`, `0x4000`, `0x8000`, `0x10000`.
- Station 101 uses the confirmed empty suffix; other stations append
  `GameText[136]`. Programmed-station and docking-target labels now preserve
  the hidden-return/copy lifetimes visible in the ARM body.
- Original repeated `Status::getShip()` and `Level::getPlayer()` calls replace
  cached aliases. Non-native null guards were removed only where the Android
  function dereferences the objects unconditionally.
- Button construction now keeps the native order `operator new -> text/String
  construction -> TouchButton constructor -> action store -> ArrayAdd`.
- `hudEvent` restores the station-name copy lifetime, duplicate-event probe,
  ListItem placement construction and the important-event mask allocation
  order.
- `hudEventMedal` no longer uses placeholder globals or zero-length separator
  arrays. Its confirmed format is `GameText[id + 0x5e3] + ":" + percent +
  "%"`, with a 100 percent clamp and priority 3 queue item.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::initHudMenu` | 10.7% | 27.0% | 1245 / 1144 |
| `Hud::hudEvent` | 22.9% | 24.9% | 1088 / 1011 |
| `Hud::hudEventMedal` | placeholder body | 84.1% | 182 / 182 |

The attempted replacement of the local `rowStep` with repeated
`Layout+0x30` loads increased `initHudMenu` to 1198 instructions but reduced
similarity from 27.0% to 24.3%. It was rejected. The recovered decompiler
loads do not prove that this was the original source expression.

The focused Hud set now contains 48 comparisons at 78.5% average, with 25
linked-exact and 18 byte-exact functions.

## Build status and remaining work

- UCRT64 `libgof2.a` builds successfully.
- The ARM object pass remains 201/204. The same three unrelated translation
  units fail on the known `SolarSystem *` versus integer migration.
- `hudEvent` event 19 remains explicitly unclaimed: the recovered body shows
  an assignment into `Hud+0x1e0`, but the decompiler does not expose a reliable
  source operand.
- `initHudMenu` is source-backed but not byte-exact. Its iPad floating-point
  remap, stack alignment and final button-position loop remain the largest
  source-shape differences.

