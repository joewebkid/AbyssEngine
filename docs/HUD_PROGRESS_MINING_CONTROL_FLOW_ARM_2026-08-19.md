# Hud Progress And Mining Control-Flow ARM Pass

Date: 2026-08-19

## Scope

This package revisits the final progress/mining portion of Android
`Hud::draw` at `0x163b90`. It uses the Android 2.0.16 body around
`0x1658f8..0x165fce` and focused ARMv7 verification for
`_ZN3Hud4drawExxP9PlayerEgobjj`.

The starting point was `42.9%`, with `3018` generated instructions versus
`3223` in the target. The accepted source now reaches `45.7%` and `3027/3223`.
This is an improvement in source shape, not a byte-match claim.

## Accepted Recovery

The progress tail no longer keeps a nullable `GameText *` cache across the
dock, jump/cloak and mining branches. Android reads `Globals::gameText` at the
individual String construction sites and assumes the initialized runtime
singletons are valid. The defensive null branches were not present in the
native body and also changed hidden-return and cleanup placement.

The jump/cloak selection now follows the native shared-body control flow:

- `Hud+0x277` selects `PlayerEgo::getDriveChargeRate()`;
- `Hud+0x276` selects `PlayerEgo::getCloakRate()`;
- the selected rate is multiplied by `1.05`, capped at `1.0`, and drawn through
  the same progress body;
- GameText `318` is used for jump drive and `317` for cloak;
- inactive jump and cloak branches join the mining tutorial tail directly.

The mining tutorial predicate now reproduces the Android call order: hint byte
`Globals::hints[0x11]`, a fresh `PlayerEgo::isMining()` call, campaign mission
`2`, signed 64-bit `LevelScript+0x08 >= 12001`, and both asteroid docking
checks. Removed `Status`, `LevelScript`, and `GameText` null guards were local
recovery scaffolding, not native behavior.

The pulse timer is calculated in a local, wrapped at `2000`, and written once
to `Hud+0x4c4`, matching the target store shape. The alpha mirror remains
`-1 - alpha` above `255`. GameText `618` is centered with signed
`Globals::w / 2`, and the branch now restores white before destroying its
temporary String; the function then restores the incoming canvas color.

## Rejected Experiments

Several literal Hex-Rays shapes were tested and rejected because they damaged
the focused ARM result:

- reloading `Globals::Canvas` throughout the whole progress body reduced
  `45.4%` to `44.5%`;
- wrapping all three progress flags in the decompiler's outer join reduced
  `45.7%` to `44.6%`;
- replacing the retained center coordinate with repeated `Globals::w` reads,
  together with the adjacent global-status experiment, reduced the result to
  `38.9%`;
- moving `Layout+0x218` and `Hud+0x3e2` reads to the apparent decompiler order
  reduced `45.7%` to `45.0%`.

These values and calls remain behaviorally understood, but the decompiler does
not prove their original C++ expression scopes. The rejected forms are not in
the committed source.

## Verification

- UCRT64 `gof2`: green; `libgof2.a` links successfully.
- ARM compile: `201/204`; the same three unrelated `SolarSystem *` versus
  integer failures remain.
- `Hud::draw`: `45.7%`, `3027/3223` instructions.
- Stack reservation: exact `224` bytes.
- VFP save set: exact `d8-d11`.
- All 48 `Hud` functions: `70.1%` average, 22 linked-exact and 16
  raw-byte-exact; no exact-function regression.
- Full no-build corpus: 4436 comparisons, `71.6%` average, 1871 linked-exact
  and 858 raw-byte-exact.

## Remaining Work

`Hud::draw` still has a 196-instruction size deficit and substantial register
allocation differences in its first half. The next pass should target one
coherent early/middle branch family and its local lifetimes rather than adding
synthetic stack objects or forcing broad GOT reloads.
