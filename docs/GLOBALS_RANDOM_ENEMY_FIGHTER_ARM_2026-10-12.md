# Globals getRandomEnemyFighter — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 function at ELF address `0xe49dc` reads
`Globals::status` for the DLC check and `Globals::rnd` for both bounded
random draws (`100` and `37`). Its default branch indexes a race table at
`.rodata` address `0x1fe2a0`; the first 37 integers match the ship-race
table already recovered in `Ship.cpp` from a second Android copy at
`0x2067c4`. A direct value-by-value check found 37/37 local entries equal
to the original table. The code excludes special indices, then accepts a
draw only when its table race equals the requested race. The existing
special cases for races 1, 9, and 10 remain unchanged.

The recovered function used two null RNG placeholders and a one-element
zero table. It now calls `Globals::rnd->nextInt` through `AERandom`, reads
`Globals::status`, and has the 37 ARM-confirmed table entries. A compile-time
assert guards the table length used by `nextInt(37)`. The change is in the
shared function used by `Level` and `NewsTicker`; their callers did not
need edits.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Globals` comparison: 49 functions. Source-shape rose 86.5% to
  98.9% and strict fuzzy 72.7% to 95.8%; linked-exact and byte-exact were
  not attained. The only reported instruction difference is the compiler's
  equivalent `kind < 4` in place of original `kind <= 3`.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type; 92.35% average source-shape,
  80.52% strict fuzzy, 2,355 linked-exact, and 1,008 byte-exact. The
  corpus averages and exact counts are unchanged after rounding. A
  per-symbol comparison found only this function's metrics changed and
  no exact-pair losses. Report:
  `A:/Galaxy on Fire new version/_work/globals_random_enemy_fighter_full_20261012.json`.

No live enemy-spawning run was performed; the native target is a static
library. Next priority: `createBillBoard`, whose remaining null canvas and
counter placeholders affect the `MovingStars` caller.
