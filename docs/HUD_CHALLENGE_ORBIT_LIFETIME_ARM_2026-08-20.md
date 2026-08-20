# Hud Challenge And Orbit Lifetime ARM Pass

Date: 2026-08-20

## Scope

This package follows the event-presentation pass with two adjacent Android
2.0.16 bodies: `Hud::drawChallengeModeScore(int)` at `0x1637e0` and
`Hud::drawOrbitInformation()` at `0x166018`. IDA and Ghidra bodies were checked
independently before source changes were retained.

## Challenge Score Cleanup

The final 3000 ms multiplier blink now has the native cleanup edge. When
`timer % 100 < 50`, Android has already selected white and jumps directly to
the main score String destructor. It does not execute the ordinary trailing
`SetColor` a second time.

The score length is unsigned in the native comparison. Restoring that type
changes the ARM condition from signed `bgt` to the target unsigned `bhi`. The
frame-width local is also reused as the post-padding digit step, matching the
single IDA local-slot role without adding synthetic storage.

Focused verification improves `drawChallengeModeScore` from `48.7%` to
`49.0%`, with `350/340` target/base instructions. The function remains
behavior-correct but is not byte-exact; register allocation and stack-slot
ownership still differ across all three digit loops.

## Orbit String Lifetime

The system/security line contains five live String objects:

1. the hidden return from `SolarSystem::getName()`;
2. its orientation-copy;
3. `String(" ")`;
4. the first concatenation;
5. the final concatenation with GameText 137.

Android keeps all five objects alive through `PaintCanvas::DrawString` and
destroys them after the call. The previous named `systemLine` local ended the
four intermediate lifetimes before drawing. Passing the concatenation directly
to `DrawString` restores the native hidden-return and unwind order.

Focused verification improves `drawOrbitInformation` from `50.3%` to `56.3%`.
The generated body is now `219/225` instructions and reserves 92 stack bytes,
down from 100; the target reserves 84. The remaining eight-byte difference is
associated with retained Canvas/font/status/layout register spills, not missing
String behavior.

## Rejected Experiments

- Moving all three challenge digit-loop indices to the declaration order shown
  by Hex-Rays reduced the focused result to `33.6%`. The decompiler variable
  order does not represent the original C++ lifetime and was removed.
- Delaying the large `Hud::draw` Canvas local until after the iPad remap reduced
  that body from `47.4%` to `33.2%`. Although individual IDA statements reload
  `Globals::Canvas`, the complete ARM body proves a wider retained lifetime.
- Making the orbit X coordinate mutable did not change code generation and was
  removed.

No volatile/register forcing, artificial stack arrays, inline assembly or
other percentage-only source changes were retained.

## Verification

- UCRT64 `libgof2.a`: green.
- ARM object coverage: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::drawChallengeModeScore`: `48.7% -> 49.0%`, `350/340` instructions.
- `Hud::drawOrbitInformation`: `50.3% -> 56.3%`, `225/219` instructions.
- `Hud::draw`: unchanged at `47.4%`, `3223/3033` instructions.
- All 48 Hud functions: `88.4%` average, 26 linked-exact and 19 byte-exact.

## Next Package

The strongest remaining target is `Hud::initHudMenu` at `27.3%`. It should be
handled as a constructor-style allocation and String-lifetime package before
another whole-body `Hud::draw` register pass. The rejected Canvas experiment
shows that the large draw function now needs a coherent multi-branch rewrite,
not isolated global-pointer lifetime edits.
