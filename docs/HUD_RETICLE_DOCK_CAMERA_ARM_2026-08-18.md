# Hud Reticle, Dock and Camera ARM Pass

Date: 2026-08-18

## Scope

This package restores the contiguous Android ARM slice of `Hud::draw` between
the mission-panel tail and the quick-menu branch. The primary body is Android
2.0.16 `Hud::draw` at `0x163b90`, cross-checked against the extracted
source-shaped `Hud.c` body.

The recovered slice covers the reticle, two hacking controls, dock-action
visibility and disabled tint, the adjacent time-extender action slot, the
autopilot/docking dock-button substitution, camera-mode image selection and
label timing, and the auto-turret/camera-banner substitution. This is a
source-backed behavioral and source-shape pass, not a whole-function byte-match
claim.

## Reticle And Hacking Controls

The reticle uses `Hud+0x348`. On iPad it is drawn at the two coordinates stored
at `Hud+0x0c/+0x0e`; phone builds anchor the same image to the lower-right
screen corner with flags `0x11/0x22`.

When `Hud+0x528` says the hacking game is active and the player is not in
turret mode, Android draws two controls from Image2D `0x1f58/0x1f57`. Their
pressed bits are `Hud+0x285 & 2` and `Hud+0x285 & 4`, and their coordinate pairs
are `+0x454/+0x456` and `+0x458/+0x45a`.

Mining, the hacking game, a completed docking-point attachment, or
landing/takeoff changes the following action color to `0xffffff2f`.

## Dock Action

The ordinary dock action uses Image2D `0x04b1/0x04b0` at
`Hud+0x3f8/+0x3fa`. It is shown only when:

- the player is outside alien orbit, or campaign mission `154` is active in
  alien orbit and at least one docking target exists;
- the campaign mission index is at least `2`;
- no mission exists, or the active mission type is not `183`.

The pressed state is `Hud+0x284 & 0x40`. During autopilot or an active docking
procedure, while neither fully docked nor landing/taking off, Android replaces
the ordinary state with the pressed dock image in white. This substitution was
missing from the previous C++ body.

The intervening action slot is part of the same native control-flow region and
is therefore retained here. `Hud::setTimeExtender` proves its state at
`Hud+0x000`, `+0x280/+0x281`, and flash timers at `+0x4bc/+0x4c0`. Android uses
Image2D resources `0x053f..0x0543`, coordinates `+0x404/+0x406`, offset
`+0x450`, touch bit `Hud+0x285 & 1`, current canvas-color preservation, and a
disabled path gated by radio/autopilot/docking/Radar state.

## Camera And Auto Turret

The camera button indexes the four-entry arrays directly with the next camera
mode. The pressed state is `Hud+0x284 & 0x80`.

| Mode | Idle Image2D | Pressed Image2D | GameText |
| ---: | ---: | ---: | ---: |
| 0 | `0x0528` | `0x0527` | `217` |
| 1 | `0x04e9` | `0x04ea` | `218` |
| 2 | `0x04be` | `0x04bf` | `219` |
| 3 | `0x052a` | `0x0529` | `220` |

`Hud+0x514` remembers the current mode. A change starts `Hud+0x518` with the
current elapsed value and assigns the corresponding GameText String at
`Hud+0x51c` through the original four-case switch.

If the ship has an auto turret, Image2D `0x0546/0x0547` is drawn at
`Hud+0x3fe/+0x400`. Enabled state combines `PlayerEgo::autoTurretIsEnabled()`
with the native high-byte press test `Hud+0x287 & 0x20`, equivalent to full
mask `0x20000000`.

Without an auto turret, the same region becomes the transient camera label.
Its fade uses the byte result of `(timer / 2000.0) * 255`, mirrors the value
after 255 with the native `-2-alpha` conversion, uses `PaintCanvas::GetWidth`
for centering, and restores the exact color returned by `GetColor` after
drawing. The timer advances to 4000 ms and then resets to zero.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem`
  pointer/integer failures remain.
- `Hud::draw`: `26.7%` -> `28.3%` fuzzy instruction similarity.
- Generated `Hud::draw`: `2582` instructions versus `3223` in the target.
- All 48 `Hud` functions: `69.5%` average, 22 linked-exact and 16
  raw-byte-exact. No exact-function regression occurred.

## Remaining Work

The quick-menu, secondary weapon, boost and main-fire audit is completed in
`HUD_QUICK_SECONDARY_FIRE_ARM_2026-08-18.md`. Its remaining secondary String
lifetime issue is explicitly queued for a broad stack/local-lifetime pass. The
next contiguous package is the dock-transfer, jump/cloak progress and mining
hint tail.
