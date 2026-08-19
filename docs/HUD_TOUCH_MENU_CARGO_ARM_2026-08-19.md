# Hud touch, quick-menu and cargo event pass (2026-08-19)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` ARM code and symbols from `_work/bins` and
  `_work/symbols`.
- IDA/Ghidra-backed bodies in
  `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`.
- Focused ARM comparisons for every changed function and the UCRT64 native
  build.

## Recovered behavior

- `touchedElement` now follows the native iPad/phone control-flow and test
  order. It restores the asymmetric secondary-action hitbox
  `x - extent / 2 .. x + extent`, the distinct steering extents, quick-menu
  rectangle and hacking controls.
- `drawMenu` restores the native quick-menu frame iteration, direct button
  drawing, direct `Globals::status` ship checks and the three-temporary
  `String("X ") + String(fuel)` construction.
- `catchCargo` no longer depends on null placeholder globals or empty marker
  strings. Mission delivery substitutes the real `#N` and `#Q` tokens;
  ordinary cargo uses the native `"t "` amount suffix and updates an existing
  aggregate event before adding a new queue entry.
- `drawEventString` keeps the event Y coordinate as a full integer. The former
  cast to `char` could wrap text vertically on taller displays.
- `updateSecondaryWeaponString` restores the native expression
  `name + " (" + amount + ")"` and writes the centred label X to confirmed
  `Hud+0x3c0`, not the unrelated `Hud+0x218` slot.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::touchedElement` | 14.2% | 45.4% | 444 / 428 |
| `Hud::drawMenu` | 17.9% | 32.5% | 225 / 199 |
| `Hud::catchCargo` | 13.5% | 18.5% | 470 / 451 |
| `Hud::drawEventString` | 38.8% | 68.3% | 67 / 59 |
| `Hud::updateSecondaryWeaponString` | 38.2% | 93.0% linked-exact | 130 / 128 |
| `Hud::drawEventQueue` | 23.9% | 23.9% retained | 144 / 140 |

The attempted local/lifetime reorder in `drawEventQueue` reduced similarity to
22.7% without recovering new behavior. It was rejected and the 23.9% body was
restored. `sameHudEventAsBeforeAggregate` remains 76.6% and
`addToEventQueue` remains 36.8%; neither was rewritten speculatively.

## Build status and remaining work

- UCRT64 `libgof2.a` builds successfully.
- The ARM object pass remains 201/204. The same three unrelated translation
  units fail on the known `SolarSystem *` versus integer migration.
- The focused Hud set contains 48 comparisons at about 77.2% average, with
  25 linked-exact and 18 byte-exact functions. The full corpus remains about
  71.71%; the preceding run recorded 4436 comparisons, 1874 linked-exact and
  860 byte-exact functions.
- Cargo replacement and aggregation are source-backed and functional, but the
  temporary-String unwind layout is not byte-exact.
- `drawEventQueue`, `initHudMenu`, `hudEvent` and the large `Hud::draw` body
  remain the densest HUD matching tasks.
