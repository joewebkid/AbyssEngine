# PlayerAsteroid ABI and global dependencies — Android ARM (2026-10-11)

The recovered `PlayerAsteroid` contained two missing four-byte fields,
shifting later derived fields and making reads/writes use the wrong ARM
offsets. They are restored as unknown fields at `0x130` and `0x154`.
Compile-time ARM assertions now pin the accessed fields from
`asteroidFlag` (`0x124`) through the hit-flash floats (`0x15c`,
`0x160`) and total size `0x170`. The original constructor, accessors,
and update routine provide the offset evidence.

The constructor and update paths now use the actual Android globals:
`Globals::Canvas` for the transform, `Globals::rnd` for random draws,
`this->level` for `asteroidDied`, and `Globals::status->field_d8` for
the destroyed-asteroid count. This removes zero/null stand-ins and an
undefined transform wrapper. `setAsteroidCenter` now updates the
existing `PlayerAsteroid::asteroidCenter` and float `asteroidDistance`
statics instead of unused local globals.

The original ARM constructor passes group `-1` and inactive mode to
`KIPlayer`, uses `0.7` of the scaled bounding radius, populates the
static random vector with three `(nextInt(0x2000) - 0x1000) / 4096`
components, and scales its random spin by
`1 - min(1, max(0.9, scaling))`. It also sets the byte at base offset
`0x3c` and resets static `emitTime`. These behaviors were restored.
Update's inactive-state transition, hit-flash values `0.1`/`0.001`, bomb-force decay
`0.98` with `0.05` cutoff, and constructor's `0.0001` flash timer now
follow ARM literals and control flow.

Native `gof2` and all 206 ARM translation units build; all 36 verifier
tests pass (one skipped). The 26-function focused family improves from
89.8% to 93.1% mean source-shape and from 70.1% to 80.7% strict fuzzy.
Its exact counts rise from 8 to 13 linked-exact and 3 to 8 byte-exact.

The full ARM corpus compares 4,323/4,524 original functions at
92.11% mean source-shape and 80.33% mean strict fuzzy. Exact totals
rise to 2,338 linked-exact and 1,008 byte-exact: five new pairs in
each category, with no lost exact pair, skipped unit, or wrong-type
symbol compared with the preceding full report.

Runtime asteroid spawning, loot, collision, and explosion have not
been tested in a running game. The deeper force/translation math and
`getProjectionVector` still need source-level review; a high score
alone does not validate them. `KIPlayer` currently ignores the
constructor's inactive argument, a separate base-class limitation.
