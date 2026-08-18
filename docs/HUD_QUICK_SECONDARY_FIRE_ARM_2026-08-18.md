# Hud Quick, Secondary And Fire ARM Pass

Date: 2026-08-18

## Scope

This package audits the contiguous Android ARM action sequence in `Hud::draw`
after the camera/auto-turret branch and before the progress panels. The primary
evidence is Android 2.0.16 `Hud::draw` at `0x163b90` and its extracted
source-shaped body.

The accepted changes cover quick-menu and secondary pressed/idle branching,
the real main-fire tutorial flag, and the direct Radar target-overlay
predicate. Boost and secondary-label experiments are recorded separately below
because their literal C++ rewrites reduced whole-function ARM similarity.

## Quick Menu

The quick-menu button is suppressed by `Hud+0x283`. Otherwise Android selects
Image2D `0x04bb/0x04ba` at `Hud+0x416/+0x418` from either touch bit
`Hud+0x284 & 4` or the `Hud+0x498/+0x49c` flash pair. The pressed path resets
the pulse to `80` only while the remaining flash time is positive.

The branch is now expressed with the original pressed/idle control flow rather
than a shared local image selector. Mouse-cursor or open quick-menu state keeps
the incoming `0xffffff00` action tint.

## Secondary Weapon

The secondary action uses Image2D `0x04bd/0x04bc` at
`Hud+0x3ec/+0x3ee`. It is visible for a non-empty selected secondary item while
not in turret mode, or while `Level+0x69` carries the delayed-secondary state.
Touch bit `0x08` and flash fields `Hud+0x48c/+0x490` select the pressed path and
the same native `80` pulse reset.

The banner is Image2D `0x04c2`, anchored at the bottom-center. Its label is the
item GameText entry `itemIndex + 1274`, followed by `" (amount)"`, centered with
`GetTextWidth`. The current C++ refreshes the same text through
`updateSecondaryWeaponString()` before drawing.

Android constructs this label directly inside `Hud::draw` with six temporary
`String` objects. Reproducing that chain in the current incomplete function
changed the generated stack frame and reduced similarity from `29.0%` to
`22.2%`. That experiment was rejected. Exact hidden-return placement is queued
for the broad `Hud::draw` stack/local-lifetime pass; this package does not claim
that the secondary String lifetime is byte-matched.

## Boost And Main Fire

Boost uses Image2D `0x04b3/0x04b2`, touch bit `0x02`, readiness fields
`Hud+0x484/+0x488`, and alpha `55 + boostRate * 75`, with `255` at full charge.
Mouse input, mining, hacking or completed docking applies the disabled yellow
state. The existing compact C++ branch is behaviorally equivalent. A literal
branch expansion reduced ARM similarity and was therefore rejected pending the
same whole-function stack/control-flow pass.

Main fire uses Image2D `0x04b5/0x04b4` at `Hud+0x3e4/+0x3e6`. The pressed
predicate is touch bit `0x10` or `Hud+0x4a5`. The latter is the byte written by
`enableFireForTutorial(bool)`, not `autofireEnabled` at `+0x4a0`; the previous
C++ body used the wrong field. iPad retains anchor flags `0x11/0x44`.

The adjacent target-context overlay is Image2D `0x0536`. Its direct Radar
predicate checks locked planet `+0x14`, asteroid `+0x0c`, station `+0x24`, or
an enemy at `+0x04` with bytes `+0x70` and `+0x75` set. It remains hidden while
docking, mining, using a stream, running a docking procedure, or operating a
turret.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem`
  pointer/integer failures remain.
- `Hud::draw`: `28.3%` -> `29.0%` fuzzy instruction similarity.
- Generated `Hud::draw`: `2578` instructions versus `3223` in the target.
- All 48 `Hud` functions: `69.6%` average, 22 linked-exact and 16
  raw-byte-exact. No exact-function regression occurred.

## Remaining Work

The next contiguous package is the dock-transfer, jump/cloak progress and
mining-hint tail. After that, a broad `Hud::draw` stack/local-lifetime pass is
required before the native secondary-label temporary chain can be reintroduced
without destroying whole-function alignment.
