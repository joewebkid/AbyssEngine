# Hud Status And Gamma Bar Recovery

Date: 2026-08-16

## Evidence

The package was reconstructed from the Android HD ARM bodies and cross-checked
against the available Ghidra output and the recovered first-pass SDK source:

- `Hud::init` at `0x1604e4`;
- `Hud::draw` at `0x163b90`;
- `Status::getGammaRayDamagePerSecond(int,int)` at `0x0acdc0`.

## Recovered Behavior

- Shield, hull, and armor-regeneration fill widths use
  `damageRate * 0.01f * nativeFillWidth`. The previous host source omitted the
  `0.01f` conversion and therefore did not preserve the native percentage
  contract.
- `DrawRegion2D` now receives the recovered destination X/Y coordinates and a
  zero rotation value. The divider images also include `Layout+0x1e8` in their
  destination Y coordinate.
- `Hud+0x2c8`, `Hud+0x2cc`, and `Hud+0x2d0` are now typed as the gamma fill,
  background, and frame slots. Their source resource IDs are `0x1f5b`,
  `0x1f5a`, and `0x1f59` respectively.
- The gamma row is visible only when the station/campaign lookup returns a
  positive damage rate. Its frame and fill Y coordinates use the native
  recurrences `2 * Hud+0x444 - Hud+0x442` and
  `2 * Hud+0x448 - Hud+0x44a`.
- Gamma fill width is `Player::getGammaHP() / 100.0f * nativeFillWidth`.
- `Hud::resetAnalogStick` resets both lock-bracket coordinates to the current
  reticle coordinates.

The same audit repaired the gameplay consumers of the gamma lookup:

- `Level::update` now uses its 64-bit elapsed-time argument, applies gamma
  damage as `rate * elapsedMs / 1000.0f`, multiplies the protection attribute
  into that rate, updates alien attackers with the real elapsed time, and
  skips the LOD update only when the native boolean argument is set.
- `PlayerEgo::PlayEngineSound` and `StopEngineSound` reinterpret the soft-float
  result for the positive-rate test and pass the recovered `0.0f` pitch to the
  gamma protection sound.
- The new-game gamma reset in `MGame` tests the returned zero bit pattern
  directly instead of numerically converting the IEEE-754 payload.

## Gamma Damage Table

For station indexes `0x6d` through `0x71`, the Android `.rodata` tables are:

| Campaign argument | Damage per second |
| --- | --- |
| `<= 0x69` | `0.7, 0.4, 0.4, 0.3, 0.2` |
| `> 0x69` while `Status+0x1e8 <= 0x9d` | `3.0, 2.0, 1.0, 0.5, 0.3` |
| Later campaign state | `1.0` only for station `0x6d`; otherwise `0.0` |

The C++ declaration remains `int` because the original soft-float ARM ABI
returns the IEEE-754 payload in `r0`. Consumers must reinterpret those bits as
`float`; a numeric integer-to-float cast is not equivalent.

## Boundary

This is a source-backed status-bar slice of the large `Hud::draw` body. It does
not claim a full `Hud::draw` recovery or whole-function ARM byte matching.
Cargo/passenger state, mission progress, the complete control cluster, and
remaining reticle branches stay queued as separate packages. The wider
`Level::update` body remains a separate source-shape and byte-match task.

## Validation

`cmake --build cmake-build-ucrt --target gof2 -- -k 0` links `libgof2.a`.

The incremental ARM verifier rebuilt the affected translation units and
reported the following fuzzy source-shape baselines:

| Function | Match |
| --- | ---: |
| `Hud::draw` | 3.4% |
| `Hud::resetAnalogStick` | 33.3% |
| `Status::getGammaRayDamagePerSecond` | 62.1% |
| `Level::update` | 60.8% |
| `PlayerEgo::PlayEngineSound` | 70.0% |
| `PlayerEgo::StopEngineSound` | 70.4% |

None of these functions is linked- or byte-exact. The repository-wide
`verify` target still exits non-zero because it detects eight pre-existing
wrong-signature symbols and three unrelated translation-unit compile failures;
the report itself completes and covers `4130/4524` original functions.
