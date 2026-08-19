# Hud Draw Stack And Lifetime ARM Pass

Date: 2026-08-18

## Scope

This package performs the first whole-function stack/local-lifetime pass over
Android 2.0.16 `Hud::draw` at `0x163b90`. It combines changes that were harmful
when tested in isolation, then restores the native String and progress call
order that controls Clang's register allocation across the complete function.

The starting implementation scored `29.0%`, emitted `2580` instructions,
reserved `176` stack bytes and saved only `d8-d9`. The Android target contains
`3223` instructions, reserves `224` bytes and saves `d8-d11`.

## In-Body Control Blocks

The iPad coordinate remap now runs directly inside `Hud::draw`, as it does in
Android. The calls to `Globals::setCoordsSteer` and `setCoordsFire` preserve the
native width-query order, output field order, knob-coordinate copy and final
anchor stores. The helper remains available for `Hud::init`, where it is a
separate local implementation concern.

The steering renderer is no longer a C++ lambda. `-Oz` had outlined that
lambda into a local `Hud::draw(...)::$_0::operator()` symbol which does not
exist in the original binary. Both native copies are now present in
`Hud::draw`: the rocket-control early-return branch and the ordinary flight
branch. Their touch-steering predicate follows the Android short-circuit call
order.

## Secondary String Lifetime

Android builds the secondary-weapon label from seven live String objects:

1. `" ("`;
2. the item amount;
3. their first concatenation;
4. `")"`;
5. the completed suffix;
6. the suffix orientation-copy;
7. the final GameText plus suffix label.

The C++ body now names those objects explicitly. It draws the final local
String directly instead of calling `updateSecondaryWeaponString()` and using a
cached member. This reproduces the confirmed constructor/destructor order and
hidden-return pressure while keeping the standalone update method intact for
its other ABI symbol.

## Dock Progress Lifetime

The decisive match improvement came from the exact dock-transfer sequence:

- query progress height for the stacked offset;
- query font height and transfer amounts;
- fetch GameText `3204/3205` and construct a separate `String(" ")`;
- compute forward/reverse transfer rate and stack offset before concatenation;
- concatenate the final label;
- query progress width and height again for drawing;
- compute the 1000 ms fade;
- re-query text dimensions and mission state for each native marker branch.

The original does not guard division by zero; the local invented zero guard
was removed. The caller/state contract supplies a positive dock total while
this branch is active.

## Rejected Experiments

- Inline iPad remap alone reduced the old fuzzy score from `29.0%` to `17.1%`.
- Direct steering copies alone produced `21.9%` while moving the body from
  `2580` to `2780` instructions.
- Adding the exact secondary String chain before fixing dock lifetime produced
  `19.9%`.
- Wrapping progress locals in the apparent outer flag gate reduced the combined
  body from `29.8%` to `21.1%`; it was rejected and the prior lifetime retained.

These results confirm that large-function fuzzy scores cannot be used as a
local truth oracle. The accepted package is the combined source-backed shape,
verified only after all related lifetimes were present.

## Verification

- UCRT64 build: green; `libgof2.a` links successfully.
- ARM corpus: `201/204`; the same three unrelated `SolarSystem`
  pointer/integer failures remain.
- `Hud::draw`: `29.0%` -> `33.4%` fuzzy instruction similarity.
- Generated body: `2580` -> `3002` instructions; Android target: `3223`.
- Stack reservation: `176` -> `216` bytes; Android target: `224`.
- VFP save set now matches Android: `d8-d11`.
- All 48 Hud functions: `69.6%` average, 22 linked-exact and 16
  raw-byte-exact. No exact-function regression occurred.

## Follow-Up

The eight-byte frame and stack-canary difference was resolved by the next
package: the original code-generation pattern requires
`-fstack-protector-strong`, not an artificial source-level scratch buffer. The
boost body and full-corpus A/B are recorded in
`HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md`.
