# Hud Progress And Mining Tail ARM Pass

Date: 2026-08-18

## Scope

This package audits the final Android ARM block of `Hud::draw`, immediately
after the quick/secondary/fire action cluster. The primary evidence is the
Android 2.0.16 `Hud::draw` body at `0x163b90`, including the dock-transfer,
jump/cloak progress and mining tutorial branches.

The accepted changes correct one resource-routing error and type the native
64-bit script timer. They are source-backed behavior fixes, but they do not
increase the whole-function ARM similarity score.

## Dock Transfer

The active predicate is `Hud+0x278`, followed by
`PlayerEgo::isDockedToDockingPoint()` and a positive hitpoint check. Android
uses two different Image2D fields in this branch:

- `Hud+0x37c`, resource `0x0539`, supplies the width and height used for the
  progress geometry;
- `Hud+0x38c`, resource `0x1f41`, is the actual dock-transfer fill passed to
  `DrawRegion2D`.

The previous C++ used resource `0x1f41` for both operations. `Hud::draw` now
preserves the native split. The reverse/normal labels remain GameText `3205`
and `3204`; the mission and production markers remain resources `0x1f40` and
`0x1f5f` at `Hud+0x380/+0x384`.

## Jump And Cloak

The adjacent branch is selected by `Hud+0x277` for jump drive and `Hud+0x276`
for cloak. Both rates are multiplied by `1.05` and clamped to `1.0`. Labels are
GameText `318` and `317`. Resource `0x0539` is the centered fill and resource
`0x053a` is the panel background; `Hud+0x464` drives the 1000 ms fade.

This behavior was already present in the C++ body and was retained after the
audit. It is source-backed, not byte-matched in isolation.

## Mining Hint

The tutorial hint is gated by hint byte `0x11`, inactive mining, campaign `2`,
`LevelScript+0x08 >= 12001`, and the absence of asteroid docking/docked state.
The native load at `LevelScript+0x08` is a signed 64-bit load. `LevelScript`
therefore exposes an ABI-neutral `scriptTime` union member over the existing
`field_0x8/field_0xc` words instead of rebuilding an unsigned value manually.

The pulse at `Hud+0x4c4` still runs over `0..2000` ms and mirrors its alpha in
the second half. The label is GameText `618`, centered at half the screen width
and drawn at `Layout+0x2c + Hud+0x3e2`. The incoming canvas color is restored at
the end of `Hud::draw`.

## Rejected Shape Experiment

Android has a shared outer test for `Hud+0x276/+0x277/+0x278`. Wrapping the C++
progress locals in a literal equivalent changed local lifetimes and reduced
`Hud::draw` from `29.0%` to `28.4%` (`2604` generated instructions versus
`3223` target instructions). The experiment was rejected. The accepted body
keeps the prior local layout while preserving the confirmed behavior fixes.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem`
  pointer/integer failures remain.
- `Hud::draw`: `29.0%`, `2578` generated instructions versus `3223` target.
- All 48 `Hud` functions: `69.6%` average, 22 linked-exact and 16
  raw-byte-exact. No exact-function regression occurred.

## Remaining Work

The next dense `Hud::draw` package should be a whole-function stack/local
lifetime pass. It must reconcile String hidden-return slots, progress locals
and branch joins together; isolated source-shape rewrites have already shown
that locally correct C++ can make the complete ARM frame worse.
