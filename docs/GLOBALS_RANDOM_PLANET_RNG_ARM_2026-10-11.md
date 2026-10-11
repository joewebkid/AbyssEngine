# Random planet name RNG — Android ARM (2026-10-11)

## Evidence and correction

Android 2.0.16 `Globals::getRandomPlanetName()` at ELF address `0xe4a8c`
constructs a `FileRead`, obtains `nextInt(100)` from `Globals::rnd`,
loads that station, copies its name, and destroys the temporary objects.
The recovered source had this sequence but read the RNG through a
translation-unit-local null placeholder. `Globals::init` already
initializes `Globals::rnd` from its `AERandom` object. The function now
uses that live pointer directly; no name-selection or ownership branch
was changed.

## Verification

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `getRandomPlanetName`: source-shape 94.2% to 100.0%, strict
  fuzzy 76.5% to 86.1%.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions
  compared, 201 missing, zero skipped or wrong-type symbols; average
  source-shape 92.32% to 92.33%, strict fuzzy 80.47%, 2,353 linked-exact,
  1,008 byte-exact. Per-symbol comparison found no exact-pair losses.
  Report: `A:/Galaxy on Fire new version/_work/random_planet_rng_full_20261011.json`.

## Remaining limit and next priority

The original ARM call sequence and native compilation are confirmed, not
live file loading or generated-name behavior. Full native executable
linking remains incomplete. Next audit the separate canvas placeholder
in `Globals::getLine`, including its text-width loop and callers, before
changing it.
