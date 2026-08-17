# Hud Draw Prelude, iPad Remap, And Queue Ownership

Date: 2026-08-17

## Scope And Evidence

This package restores the opening control flow and exit-state ownership of the
Android ARM `Hud::draw` body at `0x163b90`. The primary source-shaped body is
available in:

- `analysis/gof2_libgof2hdaa_full_ida.c`
- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`

The relevant Android fields and globals are also cross-checked against the
recovered `Hud::init`, `Globals::Globals`, `Globals::setCoordsSteer`, and
`Globals::setCoordsFire` bodies. This is a source-backed control-flow pass; it
is not a full `Hud::draw` byte-match claim.

## Confirmed And Applied

- `PlayerEgo::isMining` is evaluated before event-queue processing.
- `Hud::updateQueue(elapsed)` and `Hud::drawEventQueue()` run only while the
  player is not mining, `Hud+0x26c` is nonzero, and `Hud+0x528` is zero. The
  old unconditional tail call to `drawEventQueue` was removed.
- On iPad, cached anchors at `Hud+0x10/+0x14` are compared with the persisted
  `GameSettings` anchors at `+0x54/+0x58`. A mismatch rebuilds both control
  clusters through `Globals::setCoordsSteer` and `Globals::setCoordsFire`.
- The steering remap uses the widths of the native image fields at
  `Hud+0x31c`, `Hud+0x310`, and `Hud+0x300`. The fire remap uses the width and
  handle stored in the leading iPad fire-image fields.
- The packed pair at `Hud+0x424/+0x426` is copied to `Hud+0x41e/+0x420`
  between the steering and fire remaps, matching the Android statement order.
- `Globals::setCoordsSteer` and `Globals::setCoordsFire` now write their
  adjusted/clamped anchor values back to `GameSettings+0x54/+0x58`. `Hud`
  caches those adjusted values after both calls.
- `PaintCanvas::GetColor()` is captured after queue/remap processing. The
  original color is restored on both the rocket-control early return and the
  normal function exit.

## Anchor Semantics

The fields formerly labelled as resolution width/height in the local
`GameSettingsRecord` are HUD control anchors:

| Field | Offset | Low | Medium | High |
| --- | ---: | ---: | ---: | ---: |
| `steerAnchorX` | `0x54` | `415` | `583` | `830` |
| `fireAnchorX` | `0x58` | `365` | `513` | `730` |

These values are initialized by `Globals::Globals` from the quality level and
then remain mutable persisted layout state. Compile-time offset assertions now
protect both fields in `GameSettings.h`.

## Render Ownership Correction

The Android `Hud::draw` body ends immediately after restoring the saved canvas
color. It does not append orbit information, quick-menu, challenge-score, or
pause-button rendering. Those outer layers are orchestrated by
`MGame::OnRender2D`; the recovered C++ already invokes the confirmed pause,
orbit, and menu branches there. Removing the duplicated `Hud::draw` tail avoids
double drawing and restores the original ownership boundary. No unconfirmed
challenge-score call was added to `MGame`.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the three failures are the existing unrelated
  `SolarSystem` pointer/integer mismatches.
- `Hud::draw`: `13.9%` -> `16.4%` fuzzy instruction similarity.
- All 48 `Hud` functions: `69.3%` average, 22 linked-exact and 16
  raw-byte-exact.
- `Hud::init`: `22.1%` fuzzy similarity after removing the non-native zero
  anchor fallback from its shared coordinate helper. This score is lower than
  the previous `27.2%`, while the helper behavior is now closer to the
  recovered source and exact-function counts are unchanged.

## Remaining Work

Most of the large `Hud::draw` body remains non-exact. The next contiguous
source-backed target is the hit-direction/damage-indicator lifetime followed
by the normal steering branch. Large mission, cargo, mining, hacking, and
progress-text branches still require separate focused passes.
