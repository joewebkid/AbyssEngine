# MGame Touch Success, Cargo And Transition ARM Pass

Date: 2026-08-23

## Scope

This package continues `MGame::OnTouchEnd(int, int, void *)` at Android
address `0x17a144`. The previous pass covered pause, StarMap, ordinary menu,
failed-dialogue and HUD action routing but deliberately left the successful
dialogue dispatcher and two neighboring ChoiceWindow modes incomplete.

The retained implementation was checked against both the Android IDA body and
the independent Ghidra body. UCRT64 remains a compile gate; the ARM verifier is
the source-shape measurement.

## Recovered Behavior

- A completed instant-action mission sets `switch_to_target_setting = 2`,
  deactivates `MGame` and selects application module 1.
- Ordinary freelance completion clears the freelance slot, displays reward plus
  bonus, and credits the same total. Campaign completion advances the campaign
  before evaluating the native checkpoint dispatcher.
- The dispatcher now covers campaign checkpoints 15, 22, 42, 43, 65, 74, 81,
  95, 96, 100, 110, 120, 126, 127, 134, 144, 155, 161 and 162. Each branch
  preserves the observed combination of `setStation`/`departStation`, station
  ID, stream-out flag, target-setting mode and application module 2 or 5.
- Shield, armor, hull and gamma values are copied from the live `Player` into
  `Status+0x5c/+0x60/+0x64/+0x68` before every checkpoint that does so in the
  native body.
- Campaign 42 destroys both active objective slots before common route cleanup.
- Completing freelance mission type 183 installs item 216 with amount 10,
  creates the confirmed `"Client"` follow-up mission and switches to module 2.
- Failed campaign dialogue now performs the native `Status::resetGame()` tail.

## ChoiceWindow Modes

`MGame+0xca` is the cargo conversion mode. Confirmation converts source cargo
IDs 154 through 165 in batches of 30, or 100 in hardcore mode. IDs 154..164 map
to `source + 11`; ID 165 maps to 218. The result dialog uses GameText 291 with
per-item lines or GameText 292 when nothing was converted.

The low byte at `MGame+0x1e4` selects the terminal/challenge ChoiceWindow path:
choice 1 fades music, switches to module 1 and resets the game; choice 0 starts
the Supernova challenge through `MenuTouchWindow`; choice 2 leaves the dialog
open. The zero-flag path remains the ordinary ChoiceWindow dismissal route.

## Flight Input Tail

- HUD action bit `0x02` now performs the native boost readiness checks, forces
  thrust to 1.0, writes the 1.0/100.0 boost-touch pair, clears `Engine+0x360`
  and calls `PlayerEgo::boost()`.
- Active start sequences and a dead player route through
  `LevelScript::skipSequence()` before combat actions.
- The entry snapshot of `PlayerEgo::isAutoPilot()` is retained. A false-to-true
  transition writes the same 1.0/100.0 pair at the common touch-end tail.
- Rocket-control mode now exits after the combat-touch cluster, as Android does.

## ARM Result

Focused similarity improved from `4.3%` to `9.2%`:

| Stage | Match | Target/base instructions |
| --- | ---: | ---: |
| previous paused/HUD routing | 4.3% | 2863 / 1426 |
| completed package | 9.2% | 2863 / 2169 |

The large base-size increase is expected: it represents previously absent
mission, cargo, terminal and boost behavior. This function is not byte-matched.

## Remaining Boundary

- `Status+0x84` is proven to supply the station index for campaign checkpoint
  155, but its wider semantic field name remains unconfirmed.
- The exact gameplay name of the low-byte flag at `MGame+0x1e4` remains
  unconfirmed even though its complete selection behavior is recovered.
- The menu-close engine-sound, particle-effect and cutscene-skip tail still
  needs a dedicated pass.
- Helper extraction preserves behavior but differs from the original monolithic
  allocation and register lifetime, so the current score is not an identity
  claim.

## Validation

- Focused ARM compile: green.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem *` versus integer
  migration failures remain.
- UCRT64 `libgof2.a`: green.
