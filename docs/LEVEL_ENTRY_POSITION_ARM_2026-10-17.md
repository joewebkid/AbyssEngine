# Ordinary Level entry position (2026-10-17)

Android 2.0.16 `libgof2hdaa.so` `Level::init` at `0xadf94` calls
`createPlayer`, then takes a stream-out placement path only when
`initStreamOutPosition` and the player's geometry are present. Otherwise it
calls `PlayerEgo::setPosition(10, 10, 10000)` (`0xae26c`–`0xae272`). Campaign
mission 1 overrides that position with `(0, 0, -110000)` (`0xae280`–`0xae28c`).
The prior source called a guessed virtual `placeAtStation` on the player for
every mode, including the ordinary launch. The ordinary and mission-1 paths
now follow the ARM calls.

The source-extracted branch passes a host smoke at `-O0` and `-O2` for
ordinary launch, mission 1 and a stream-out flag with no geometry. This
test does not execute `createSpace`, `createPlayer`, or all of `Level::init`.
Native `gof2` and 40 verifier tests pass (one existing skip); the ARM build
compiles 208/208 TUs. Focused `Level::init` source-shape / strict fuzzy rises
from 69.8% / 44.8% to 71.7% / 47.2%, still nonexact. Baseline:
`A:/Galaxy on Fire new version/_work/level_init_globals_after_final_20261017.json`;
after: `A:/Galaxy on Fire new version/_work/level_entry_position_after_20261017.json`.

The stream-out branch is **not** recovered by this change. Android code gets
a position from `landmarks[2]` or a station orbit and sets direction, while
the current `createSpace` allocates `landmarks` but does not create its
objects. The retained guessed virtual call is explicitly marked as a
shortcut. Restoring `createSpace`'s station/jumpgate landmarks is the next
priority before claiming full stage-0 behavior.

The full ARM report at
`A:/Galaxy on Fire new version/_work/level_entry_position_full_20261017.json`
compares 4,323/4,524 originals in 4,730 rows. Mean source-shape / strict
fuzzy remains 93.83% / 82.99% after rounding, with 2,528 linked-exact and
1,076 byte-exact. Pairwise comparison with
`level_init_globals_full_20261017.json` found zero exact-pair losses or
gains. Missing remains 201, skipped and wrong-type zero.
