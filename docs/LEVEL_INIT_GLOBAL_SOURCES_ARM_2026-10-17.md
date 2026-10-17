# Level initialization global sources (2026-10-17)

## Recovered links

Android 2.0.16 `libgof2hdaa.so` at `0xadf84` writes 1 directly to
`Level::initStreamOutPosition`. The previous `setInitStreamOut()` dereferenced
an unbound pointer. At `0xadf94`, the decompiled `Level::init()` reads that
same flag during entry placement and black-market placement, reads
`Mission::empty` for a missing mission, and tests the two game-option bytes
at `Globals::options[0x35]` and `[0x37]`. Those sources replace four local
null placeholders; `GameSettings` now asserts the option-field offsets.

The Android evidence establishes these data sources, not the correctness of
the surrounding recovered entry-placement control flow. In particular, the
source's station-stack-to-integer cast and virtual `placeAtStation` call are
not validated by this package. Full stage-0/1 execution remains open.

## Verification

Focused ARM results, source-shape / strict fuzzy:

| Symbol | Before | After | Exact |
| --- | ---: | ---: | --- |
| `Level::setInitStreamOut()` | 22.9% / 0.0% | 100% / 100% | linked-exact |
| `Level::init()` | 64.3% / 35.4% | 69.8% / 44.8% | no |

Reports: `A:/Galaxy on Fire new version/_work/level_init_globals_before_20261017.json`
and `A:/Galaxy on Fire new version/_work/level_init_globals_after_20261017.json`.
The extracted-body host smoke checks `setInitStreamOut()` and the constructor
at `-O0/-O2`. Native `gof2` builds and 40 verifier tests pass (one existing
skip). This does not assert that `Level::init()` can run end-to-end.

The full ARM report at
`A:/Galaxy on Fire new version/_work/level_init_globals_full_20261017.json`
compares 4,323/4,524 originals in 4,730 rows. Mean source-shape is 93.83%
(previously 93.81%) and mean strict fuzzy is 82.99% (previously 82.97%).
Linked-exact rises from 2,527 to 2,528; byte-exact remains 1,076.
Pairwise comparison against `level_entry_full_20261017.json` shows no lost
linked- or byte-exact symbols and only the new `setInitStreamOut()` gain.
Missing remains 201, skipped and wrong-type zero.
