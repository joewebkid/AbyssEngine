# Hud event presentation and challenge blink pass (2026-08-20)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` from the local verify corpus.
- IDA body at `Hud.c:1596`, `Hud.c:2497` and `Hud.c:2563`.
- Independent Ghidra bodies at `0x1722f4`, `0x1736f4` and `0x1737e0`.
- NDK r18b `-Oz` function comparisons and the UCRT64 native build.

## Recovered behavior and source shape

- `drawEventQueue` reads `Radar::drawTarget` twice through the same static
  slot. A visible target adds `Hud+0x3e2` to the banner/text Y path.
- Banner alpha is `(eventQueueTimer / 2000.0f) * 255.0f`. Values above 255
  retain the native `0xfe - rawAlpha` low-byte behavior.
- Text slide is `Layout+0x1e0 * -1.0f` while a target is visible and
  `Layout+0x1e0 * -2.0f` otherwise. The banner image itself uses
  `Hud+0x354`, `Hud+0x3e0` and `Layout+0x1e4`.
- Event priority colors are confirmed as kind 2 `(0, 237, 0)`, kind 1
  `(255, 42, 0)`, kind 3 `(255, 128, 0)`, and white for all other kinds.
  The event label is centered against `Globals::w`.
- `drawEventString` now keeps the native independent wrapped and ordinary
  draw exits. Wrapped text is anchored against `eventLineMarginAlt`; ordinary
  text uses `eventLineX` and `eventLineMargin`. Both draw at
  `eventLineY - 1`.
- Both duplicate searches dereference the owning `eventQueue` field at the
  native sites. The ordinary search returns `1/0`; the aggregate search
  returns the matching queue index or `-1`.
- Challenge multiplier visibility is now behavior-correct: during the final
  3000 ms, `timer % 100 < 50` suppresses the complete multiplier row. The
  prior C++ body incorrectly kept the icon and multiplier digits visible and
  blinked only the bonus value.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::drawEventQueue` | 55.6% | 75.4% | 144 / 137 |
| `Hud::drawEventString` | 68.3% | 90.1% | 67 / 64 |
| `Hud::sameHudEventAsBefore` | 77.6% | 94.1% | 25 / 26 |
| `Hud::sameHudEventAsBeforeAggregate` | 76.6% | 93.9% | 24 / 25 |
| `Hud::drawChallengeModeScore` | 50.1% | 48.7% | 350 / 340 |

The challenge score reduction is retained because the Android and Ghidra
bodies independently confirm the corrected whole-row blink behavior. The
remaining mismatch is stack-slot and basic-block placement, not evidence for
the old behavior. No volatile/register forcing, fake stack storage or inline
assembly was retained.

The focused 48-function Hud set now averages 88.2%, with 26 linked-exact and
19 raw-byte-exact functions.

## Build status and remaining work

- UCRT64 `libgof2.a` builds successfully.
- ARM object coverage remains 201/204. The same three unrelated translation
  units fail on the known `SolarSystem *` versus integer migration.
- None of the five functions in this package is claimed byte-exact.
- `drawChallengeModeScore` still needs a dedicated String stack-slot/lifetime
  pass. `drawEventQueue` retains register-allocation and literal-pool
  differences despite matching behavior and call order.

## Godot transfer contract

- Reproduce the event-kind colors and centered label behavior exactly.
- Drive banner fade from a 2000 ms timer and use separate target-visible and
  target-hidden slide distances.
- Blink the complete challenge multiplier row at 10 Hz during its last three
  seconds; do not leave the icon visible during the off phase.
- Duplicate suppression must search queue slots from newest to oldest and
  ignore slot zero.
