# Hud Mission And Target Overlay Recovery

Date: 2026-08-16

## Evidence

This package is source-backed by the Android HD ARM bodies and was checked in
both decompiler views:

- `Hud::draw` at `0x163b90`;
- `Hud::init` at `0x1604e4`;
- `MGame::OnRender2D`, which supplies both 64-bit arguments to `Hud::draw`;
- the typed `Level`, `Mission`, `Ship`, `Status`, `PlayerEgo`, `Radar`, and
  `KIPlayer` fields consumed by those branches.

The second `Hud::draw` argument is not `SystemTimeMillis`. Android computes it
as `LevelScript+0x00 - *(int64_t *)(LevelScript+0x08)`. The local MGame call
site now reproduces that expression, while `Radio::draw` continues to receive
the original `LevelScript+0x08` time value.

## Image And Layout Map

| Android Hud slot | Resource | Confirmed consumer |
| --- | ---: | --- |
| `+0x2e8` | `0x0536` | target-context overlay beside main fire |
| `+0x320` | `0x04c5` | timed-mission panel |
| `+0x324` | `0x0520` | ordinary cargo panel |
| `+0x334` | `0x1f43` | passenger panel |
| `+0x338` | `0x1f61` | production cargo panel |
| `+0x33c` | `0x1f60` | production remaining panel |
| `+0x340` | `0x1f5c` | volatile-cargo fill overlay |
| `+0x344` | `0x1f42` | mission status panel |
| `+0x360/+0x364` | `0x0525/0x0526` | armor vertical/horizontal hit arrows |
| `+0x368/+0x36c` | `0x052b/0x052c` | shield vertical/horizontal hit arrows |

The panel origin remains the native `Hud+0x438/+0x43a` pair. The recovered
layout reads are `Layout+0x2c`, `+0x1ec`, `+0x1f0`, `+0x1f4`, `+0x1f8`,
`+0x1fc`, `+0x200`, `+0x204`, and `+0x208`; no replacement coordinates were
invented.

## Recovered Branches

- Mission type `12` draws `Level+0x24 : Level+0x20`, now typed as the two kill
  counters.
- Mission type `184` draws passenger capacity from `Status+0x178` and
  `Ship::getMaxPassengers`, followed by the mission status row. Campaign/station
  pairs `102/113` and `139/131` deliberately use the cargo variant instead.
- Mission type `174` draws the production-good cargo amount/capacity and the
  remaining production amount.
- Ordinary flight draws `currentLoad / maxLoad t`; values at least `101` apply
  the native two-column text correction.
- A positive mission timer, except campaign step `42`, replaces the cargo row
  with `Globals::longToTimeString` and resource `0x04c5`.
- Every cargo-style branch preserves the native volatile-goods fill width,
  clamped only at `1.0f` as in Android.
- `PlayerEgo+0x20` is now named as the HUD hit-direction bitfield. Bits `0x01`,
  `0x02`, `0x18`, and `0x24` start left, right, top, and bottom 300 ms pulses.
  Arrow placement uses `Radar+0x4c/+0x50`, with the native alignment and flip
  flags.
- The target-context overlay is hidden during asteroid/stream/docking/mining
  and turret states. It is shown for raw Radar slots `+0x14`, `+0x0c`, `+0x24`,
  or an enemy at `+0x04` whose bytes `+0x70` and `+0x75` are both active.

## Boundary

This is a behavioral source recovery, not a full `Hud::draw` ABI or byte-match
claim. The local `Hud` class still uses a compact host mirror, so semantic image
aliases and four host-side pulse timers do not yet occupy the original ARM
object offsets.

The dense post-fire progress family remains queued: docking transfer,
jump-drive charge, cloak/boost-related progress, extender notification, and
their alpha/string lifetime details. The exact gameplay name of raw
`Radar+0x14` also remains subject to the wider Radar ABI audit even though this
consumer and offset are confirmed.

## Validation

- UCRT64 native build: `libgof2.a` links successfully.
- `git diff --check`: clean.
- ARM `verify-fn` for `_ZN3Hud4drawExxP9PlayerEgobjj`: `1.8%`,
  `linked_equal=False`, `bytes_equal=False`, original `3223` instructions,
  local `785` instructions.
- ARM `verify-fn` for `_ZN5MGame10OnRender2DEv`: `15.4%`,
  `linked_equal=False`, `bytes_equal=False`, original `433` instructions,
  local `241` instructions.

The unchanged whole-function percentage is expected until the remaining body
and exact 32-bit `Hud` layout are restored. It must not be read as a regression
or as evidence of byte equality.
