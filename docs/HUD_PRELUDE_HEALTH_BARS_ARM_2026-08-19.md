# Hud Prelude And Health Bars ARM Pass

Date: 2026-08-19

## Scope

This package audits the opening and shield/armor/gamma portion of Android
`Hud::draw` at `0x163b90`, primarily the body represented around
`0x163c14..0x16415e`. The focused target is
`_ZN3Hud4drawExxP9PlayerEgobjj`.

The package starts from the committed `45.7%` result and preserves the exact
224-byte stack reservation and `d8-d11` VFP save set. The accepted source
reaches `46.6%` and `3033/3223` instructions. It is not byte-identical.

## iPad Anchor Cache

`Globals::setCoordsSteer` and `Globals::setCoordsFire` may clamp their inputs
and write the adjusted anchors back to `GameSettings+0x54/+0x58`. Android
reloads those globals after both calls before updating `Hud+0x10/+0x14`.

The prior C++ cached the pre-call `steerAnchor` and `fireAnchor` locals. It now
stores the post-call `GameSettings::steerAnchorX/fireAnchorX` values, avoiding
a repeated remap when a persisted anchor is adjusted by the native layout
logic.

## Boost Prelude Lifetime

The boost-ready branch reads `Hud+0x48c` (`secondaryFlashRemaining`) and
`Hud+0x4d4` (`menuOriginX`) before writing the ready latch and flash timers at
`Hud+0x474/+0x484/+0x488`. Those two values now have explicit pre-store local
lifetimes. The resulting ARM shape improved the focused result without
changing the 2000 ms/80 ms behavior.

## Shield And Armor Bars

Android computes each bar width in this order:

`int((float(rate) * 0.01f) * float(Hud+0x446))`

The old source precomputed `0.01f * width` and multiplied the rate by that
value. Those expressions are not equivalent under single-precision rounding.
Shield damage, hull damage, and armor regeneration now all use the native
multiplication order.

The no-shield and with-shield paths select address pairs before the armor row:

| State | Frame Y address | Fill Y address |
| --- | --- | --- |
| shield row drawn | `Hud+0x444` | `Hud+0x448` |
| no shield row | `Hud+0x442` | `Hud+0x44a` |

The C++ now represents that selection as two field addresses and loads the
values at the join, matching the recovered `v24/v25` shape. The retained
`Player *` is loaded after the initial white `SetColor`, which matches the
native lifetime while avoiding repeated `PlayerEgo+0x00` loads.

## Gamma Guard Debt

The Android body directly uses `Globals::status`, obtains its station, and
does not contain null guards. The current C++ still has two defensive gamma
guards and uses the synchronized `Status::gStatus` alias. These checks are
recovery scaffolding, not claimed original behavior.

Removing them in isolation reduced `Hud::draw` to `42.2%`; combining their
removal with the direct global-status form reached `42.1%`. Their control-flow
currently compensates for wider register/lifetime differences and must be
removed only as part of a coherent gamma/rocket-entry rewrite.

## Other Rejected Shapes

- Reloading `Globals::globals` between the steering and fire coordinate calls
  reduced `46.5%` to `45.0%`.
- Re-reading `ego->player` before every getter instead of retaining one local
  reduced the result to `33.2%`.

Both forms were literal readings of the decompiler, but the target assembly
shows register retention once the surrounding order is considered. They are
not present in the committed source.

## Verification

- UCRT64 `gof2`: green; `libgof2.a` links successfully.
- ARM compile: `201/204`; the same three unrelated `SolarSystem *` versus
  integer failures remain.
- `Hud::draw`: `45.7% -> 46.6%`, `3033/3223` instructions.
- Stack reservation: exact `224` bytes.
- VFP save set: exact `d8-d11`.
- All 48 `Hud` functions: `70.2%` average, 22 linked-exact and 16
  raw-byte-exact.
- Full no-build corpus: 4436 comparisons, `71.6%` average, 1871 linked-exact
  and 858 raw-byte-exact.

## Remaining Work

The next coherent package should join the gamma tail to the rocket-control
early return and normal hit-direction entry. That pass can remove the gamma
scaffolding while correcting the surrounding `Status`, `Canvas`, `Player`, and
`PlayerEgo` register lifetimes together.
