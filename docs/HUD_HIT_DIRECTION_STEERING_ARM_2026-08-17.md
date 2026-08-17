# Hud Hit-Direction And Steering ARM Pass

Date: 2026-08-17

## Scope And Evidence

This package restores the contiguous hit-direction and steering section of the
Android ARM `Hud::draw` body at `0x163b90`. The source-shaped statements were
checked in:

- `analysis/gof2_libgof2hdaa_full_ida.c`
- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`

The field offsets were cross-checked against the ARM layouts of `Hud`,
`PlayerEgo`, and `Radar`. This is a source-backed behavior and ABI pass, not a
full byte-match claim for the large `Hud::draw` function.

## Hit-Direction Contract

`PlayerEgo+0x20` is read as a direction bitfield. Every asserted direction
reloads one of four `Hud` timers to 300 ms:

| Direction | Flag mask | Timer offset | Draw transform |
| --- | ---: | ---: | --- |
| Left | `0x01` | `Hud+0x4ac` | horizontal image, mirrored with `0x11/0x41/1` |
| Right | `0x02` | `Hud+0x4b0` | horizontal image with `0x12/0x42` |
| Top | `0x18` | `Hud+0x4b4` | vertical image with `0x11/0x14` |
| Bottom | `0x24` | `Hud+0x4b8` | vertical image, mirrored with `0x21/0x24/2` |

The native statement order writes left, right, bottom, and then top, while the
draw order is left, right, top, and bottom. Every visible pulse uses alpha
`255 * timer / 300` and subtracts the current frame delta after drawing.

The horizontal image is selected from `Hud+0x364/+0x36c`; the vertical image
is selected from `Hud+0x360/+0x368`. Armor images are used when
`PlayerEgo::getShieldDamageRate() < 1`, otherwise the shield variants are
used. Placement is relative to the screen centre and the live dimensions at
`Radar+0x4c/+0x50`. The previous non-native nullable `Radar` branch was
removed; the original runtime contract requires this attachment while the HUD
is being drawn.

## Steering Contract

Both the rocket-control early branch and normal branch draw the steering base
and knob with the same native conditions:

- alpha is reduced to `0x32` when touch steering is disabled;
- alpha is also reduced during autopilot, asteroid/docking-point docking, or
  the active hacking game when the player is not operating a turret;
- touch bit `0x20` selects the pressed knob at `Hud+0x41e/+0x420`;
- the idle path restores that pair from `Hud+0x424/+0x426` before drawing;
- the rocket branch restores the canvas color captured by `Hud::draw`;
- the normal branch ends the cluster at opaque white.

The prior C++ incorrectly used `Hud+0x1ec` as a letterbox condition. Android
reads `Hud+0x528` (`hackingGameActive`) here. `Hud+0x1ec` is now named
`eventTextWraps`: it is produced from event/secondary text width and only
selects the event-string alignment path. The `letterbox` function argument is
unused in the recovered Android body and is no longer written into that field.

## ABI Locks

New compile-time assertions cover:

- all four hit-image fields at `Hud+0x360..+0x36c`;
- all four hit timers at `Hud+0x4ac..+0x4b8`;
- `PlayerEgo+0x14` and hit flags at `PlayerEgo+0x20`;
- `Radar::imageWidth/imageHeight` at `+0x4c/+0x50`.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the three failures remain the known unrelated
  `SolarSystem` pointer/integer mismatches.
- `Hud::draw`: `16.4%` -> `17.6%` fuzzy instruction similarity.
- All 48 `Hud` functions: `69.3%` average, 22 linked-exact and 16
  raw-byte-exact.

During source-shape testing, direct duplication of the two steering blocks and
an explicit branch-shaped image selector reduced fuzzy alignment despite being
behaviorally equivalent. Those forms were rejected. The retained local helper
accepts the native final color as an argument, preserving the distinct rocket
and normal color lifetimes while keeping the stronger ARM alignment.

## Remaining Work

The cargo/passenger/production and mission-timer follow-up is recorded in
`HUD_CARGO_MISSION_PANELS_ARM_2026-08-17.md`. The next contiguous package is
the reticle, dock-action, camera-mode, and auto-turret cluster.
