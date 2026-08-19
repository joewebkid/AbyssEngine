# Hud challenge, orbit and quick-menu ABI pass (2026-08-19)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` ARM code and symbols from `_work/bins` and
  `_work/symbols`.
- IDA/Ghidra-backed bodies in
  `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`.
- Focused `verify-fn` comparisons for `Hud`, plus the UCRT64 native build.

## Runtime and ABI corrections

- The steering-mode byte read by `Hud::draw` at `0x2181f9` is
  `Globals::options + 0x11`; the duplicate standalone global was removed.
- `KIPlayer+0x70` is read with `ldrb`, so the field is now a byte followed by
  explicit padding. The 32-bit class layout does not change.
- The mining hint triangle uses `255 - alpha`, matching the target `rsb`.
- `Status+0x180/+0x184/+0x18c` are typed as challenge multiplier timer,
  challenge score and challenge multiplier.
- `Hud+0x4cc/+0x4d0` are the quick-menu translation coordinates. The previous
  `+0x4d4/+0x4d8` names were eight bytes late.
- `Hud+0x4d4..+0x4f0` now receives the eight native metrics from
  `Layout+0x12c..+0x148`: boost text X, touch extents, analog radius, radar
  inset and event margins.
- The initial quick-menu row is stored at `Hud+0x3d8` (`menuBaseY`), not in the
  later translation field.

## Recovered behavior

- `drawChallengeModeScore` no longer dereferences placeholder null globals.
  It restores seven-digit padding, cyan digits, bonus flicker, multiplier icon
  position and timer-driven multiplier scaling.
- `drawOrbitInformation` restores direct station/system text lifetimes and the
  12-byte RGB table stride used by Android.
- `setCurrentSecondaryWeapon` is restored to its two-instruction native role:
  store the item and tail-call `updateSecondaryWeaponString`.
- `hudEvent` restores event 27 (`GameText[322]`) and direct
  `Globals::gameText/status` ownership.
- `initHudMenu` uses persisted `GameSettings+0x54/+0x58` iPad anchors, including
  the native floating-point `112.5` subtraction, and inlines button creation
  and final translate/position-cache routing.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::draw` | 46.9% | 47.4% | 3223 / 3033 |
| `Hud::drawChallengeModeScore` | 3.3% | 50.1% | 350 / 340 |
| `Hud::drawOrbitInformation` | 7.8% | 38.6% | 225 / 210 |
| `Hud::setCurrentSecondaryWeapon` | 9.1% | 100.0% normalized | 2 / 2 |
| `Hud::hudEvent` | 10.3% | 22.9% | 1088 / 1003 |
| `Hud::initHudMenu` | 8.9% | 10.7% | 1245 / 994 |
| `Hud::getAnalogX/Y` | 90.9% | 100.0% linked and byte exact | 11 / 11 each |

The full 4436-comparison corpus is now 71.68% average with 1873 linked-exact
and 860 byte-exact comparisons. The ARM object build remains 201/204; the same
three unrelated `SolarSystem *` versus integer translation units fail.

## Remaining work

- `initHudMenu` still needs a direct constructor/control-flow pass for all four
  menu modes; helper inlining recovered size but not final source shape.
- `touchedElement`, `drawMenu`, `catchCargo` and `drawEventQueue` are now the
  densest connected quick-menu/input blocks.
- Challenge/orbit functions are source-backed and functional, but are not
  claimed byte-exact.
