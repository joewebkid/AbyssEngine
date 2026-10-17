# Level entry constructor and canvas recovery (2026-10-17)

## Evidence and correction

- Android 2.0.16 `libgof2hdaa.so` `Level::Level(int)` at `0xadac8` calls
  `AbyssEngine::AEMath::Matrix::Matrix()` exactly three times for the
  `skyMatrix`, `cloudMatrix` and `reversalMatrix` members. These C++ members
  already construct implicitly before the body. The prior body called a null
  `g_levelSubCtor` pointer three more times; those calls are removed.
- Android `Level::init()` at `0xadf94` loads `Globals::Canvas` for the first
  `ParticleSystemManager` constructor. The null `g_init_canvas` placeholder
  is replaced with `Globals::Canvas`. Later init paths still need separate
  recovery and execution checks.

## Validation

Focused ARM verification before / after, source-shape then strict fuzzy:

| Symbol | Before | After | Exact |
| --- | ---: | ---: | --- |
| `Level::Level(int)` C1/C2 | 32.6% / 25.3% | 95.0% / 75.5% | no |
| `Level::init()` | 63.0% / 39.6% | 64.3% / 35.4% | no |

Reports: `A:/Galaxy on Fire new version/_work/level_entry_before_20261017.json`
and `A:/Galaxy on Fire new version/_work/level_entry_after_20261017.json`.
The strict fuzzy decline for `init()` is recorded, not hidden.

The isolated host smoke compiles the current constructor body extracted from
`Level.cpp` with the production `Matrix.cpp` at `-O0` and `-O2`; both pass.
It checks the mission and skybox sentinels and all three initialized matrices.
The smoke does not execute the full `Level` translation unit or destructor,
and host 64-bit field offsets are not an Android ABI oracle. The native `gof2`
library builds; all 40 verifier unit tests pass with one existing skip.

The full ARM report at
`A:/Galaxy on Fire new version/_work/level_entry_full_20261017.json`
compiled 208/208 TUs and compared 4,323/4,524 unique original functions
in 4,730 rows. Mean source-shape is 93.81% (previously 93.78%); mean strict
fuzzy is 82.97% (previously 82.95%). There are 2,527 linked-exact and 1,076
byte-exact rows, unchanged. Pairwise symbol comparison against
`galaxy_distance_full_20261017.json` found zero lost exact pairs. Missing
remains 201; skipped and wrong-type remain zero.

The next behavioral gate is a resource-backed `Level::init` stage-0 run,
followed by stage 1 and the first flight update/render. Remaining null
placeholders in these paths must be resolved against native evidence, not
merely given non-null test values.
