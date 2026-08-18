# Hud Cargo And Mission Panels ARM Pass

Date: 2026-08-17

## Scope And Evidence

This package restores the cargo/passenger/production/countdown dispatch that
immediately follows the steering cluster in Android ARM `Hud::draw` at
`0x163b90`. The primary source-shaped evidence is in:

- `analysis/gof2_libgof2hdaa_full_ida.c`
- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`

The pass removes local convenience routing that shortened the generated ARM
body and restores the original live `Status`, `Mission`, `Ship`, `Level`, and
temporary `String` call flow. It remains a focused source-backed pass, not a
full byte-match claim for `Hud::draw`.

## Dispatch Order

The recovered decision tree is:

1. Mission type `12`: draw the kill-counter cargo panel.
2. Otherwise, when the remaining mission time is below one or campaign step
   is `42`, select passenger, production, or ordinary cargo state.
3. Otherwise draw the mission countdown panel.

The mission type `12` label is:

`Level+0x24 + " : " + Level+0x20`

These fields are now typed as `killCountB` and `killCountA` and protected by
ARM offset assertions.

## Passenger Mission 184

The primary label is `Status+0x178 / Ship::getMaxPassengers()`. Normally it
uses the passenger panel. Two campaign/station combinations intentionally use
the ordinary cargo panel while outside alien orbit:

- campaign `102`, station `113`;
- campaign `139`, station `131`.

The second row always uses the mission-status panel and displays
`Mission::getStatusValue()`. Its vertical placement is based on the native
passenger-panel height even when the first row uses the cargo-panel exception.

## Production Mission 174

The first row obtains the production good from the ship cargo and displays:

`current amount / (current amount + free cargo space)`

The second row displays:

`Mission::getProductionGoodAmount() - Mission::getStatusValue()`

The production cargo and remaining panels use the same passenger-panel height
as the native vertical stacking reference.

## Ordinary Cargo And Timer

The ordinary cargo label is `current load / maximum load` followed by `t`.
When current load is at least `101`, its text is shifted left by twice the
layout line height at `Layout+0x2c`.

The countdown path calls `Globals::longToTimeString` with the second 64-bit
argument supplied to `Hud::draw`, then draws the timer image and text. The
former nullable `Globals::gGlobals` fallback was removed; Android calls the
live `Globals::globals` instance directly.

## Volatile Cargo Overlay

Every cargo-like branch independently checks `PlayerEgo::hasVolatileGoods`.
When active, Image2D `0x1f5c` is cropped to:

`min(PlayerEgo::getVolatileForce(), 1.0) * image width`

The source-shaped path calls `getVolatileForce` once for the comparison and a
second time when the value is at most one. The overlay always uses
`missionPanelX - Layout+0x1ec` and `missionPanelY`, including passenger and
production branches. The helper is forced inline and no longer emits an extra
non-native ARM symbol.

## Images And Layout

| Role | Hud offset | Image2D resource |
| --- | ---: | ---: |
| Mission timer | `0x320` | `0x04c5` |
| Cargo | `0x324` | `0x0520` |
| Passenger | `0x334` | `0x1f43` |
| Production cargo | `0x338` | `0x1f61` |
| Production remaining | `0x33c` | `0x1f60` |
| Volatile overlay | `0x340` | `0x1f5c` |
| Mission status | `0x344` | `0x1f42` |

The common panel anchor is now typed as `missionPanelX/Y` at
`Hud+0x438/+0x43a`. Confirmed draw-time layout fields are `0x1ec`, `0x1f0`,
`0x1f4`, `0x1f8`, `0x1fc`, `0x200`, `0x204`, and `0x208`. After the panel
dispatch, mouse-cursor or open quick-menu state restores the native
`0xffffff00` color before reticle rendering.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem`
  pointer/integer failures remain.
- `Hud::draw`: `17.6%` -> `26.7%` fuzzy instruction similarity.
- Generated `Hud::draw`: `2399` instructions versus `3223` in the target.
- All 48 `Hud` functions: `69.5%` average, 22 linked-exact and 16
  raw-byte-exact.

## Remaining Work

The reticle, dock-action, camera-mode and auto-turret cluster is completed in
`HUD_RETICLE_DOCK_CAMERA_ARM_2026-08-18.md`. The next contiguous package is the
quick-menu, secondary weapon, boost and main-fire action sequence.
