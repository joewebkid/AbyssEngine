# Hud Gamma, Rocket And Hit Entry ARM Pass

Date: 2026-08-19

## Scope

This pass covers the Android ARM `Hud::draw` span from the gamma bars through
the rocket-control exit and the normal hit-direction entry. The primary binary
is `libgof2hd.so`, function `_ZN3Hud4drawEv` at `0x163b90`; the matching
recovered C reference is the `Hud::draw` body around lines 3220-3408.

The focused baseline was `46.6%` similarity with `3033/3223` generated/target
instructions. The accepted source finishes at `46.9%` and `3034/3223`.

## Confirmed Source Shape

- Gamma bar Y coordinates now retain the native `base - previous + base`
  recurrence for both the frame (`Hud+0x444`, `Hud+0x442`) and fill
  (`Hud+0x448`, `Hud+0x44a`). This is mathematically equivalent to the former
  expression, but emits the target `sub` followed by `add` sequence.
- The secondary rocket control keeps the native uncollapsed pressed condition:
  touch-mask bit `0x08`, or an active flash countdown with an expired pulse.
  An active countdown reloads the pulse to `80` after drawing the pressed
  image; otherwise the idle image is used.
- Normal hit-direction images are selected through the native Hud offsets.
  Horizontal uses `+0x36c` for shield damage and `+0x364` for armor damage;
  vertical uses `+0x368` and `+0x360`. The comparison is the target signed
  `damageRate < 1` form.

## Rejected A/B Forms

The following experiments were reverted and are not present in the source:

- Replacing retained owners with direct `Globals::status`, removing gamma
  guards, and rebuilding the hit owner lowered similarity to `44.3%`.
- Repeating direct `Globals::Canvas` loads around the rocket branch lowered it
  to `35.9%`.
- Moving the initial canvas lifetime below the event/iPad branches produced
  `44.4%`.
- Isolated direct `Globals::status` produced `46.8%`; rebuilding a fresh
  `Player` owner for gamma HP produced `44.7%`.

The gamma null guards and the retained `Status::gStatus` alias therefore remain
explicit recovery scaffolding. They should only be removed during a wider
prologue/GOT-owner lifetime pass, not by a local source-style guess.

## Verification

- Android ARM compile: `201/204`; the same three unrelated `SolarSystem *`
  versus integer failures remain outside this package.
- Focused `_ZN3Hud4drawEv`: `46.9%`, `3034/3223` instructions.
- Stack reservation remains exact at `224` bytes; the saved VFP set remains
  `d8-d11`.
- All 48 Hud functions: `70.2%` average, 22 linked-exact, 16 byte-exact.
- Full no-build corpus: 4436 comparisons, `71.6%` average, 1871 linked-exact,
  858 byte-exact. Exact counters are unchanged.
- UCRT64 `gof2` target links successfully.

## Next Boundary

The next coherent package is the four normal hit-direction pulse bodies:
alpha division, screen-size/canvas reloads, `Radar+0x4c/+0x50` transforms,
timer stores, and the shared steering-color join.
