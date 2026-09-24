# StarMap ARM source-shape report — 2026-09-24

## Scope and result

- Baseline: source snapshot immediately before the StarMap recovery changes.
- Edited source: `src/game/world/StarMap.cpp` only. `StarMap.h` and files outside the ownership boundary are unchanged.
- Android ARM library: `_work/bins/android_2.0.16_libgof2hdaa.so`; decompiler: `analysis/gof2_libgof2hdaa_full_ida.c`.
- The requested `99%` source-shape target was **not reached**. The best accepted version measured **83.81% unweighted**, **73.62% instruction-weighted**, and **48.29% strict fuzzy** across 20 distinct implementation bodies. Constructor C2 and destructor D2 are shown as aliases in the table and excluded from those averages.
- Baseline at the pre-StarMap source snapshot: **58.62% unweighted**, **40.75% instruction-weighted**, and **35.08% strict fuzzy** across the same 20 bodies. The compared original implementations contain 7101 instructions; current source contains 5112.

The current source is an evidence-backed partial recovery, not a 99% reconstruction. The remaining ceiling is dominated by incomplete large bodies (`draw`, `OnTouchEnd`, `init`, `update`) and residual ABI/code-shape differences. No synthetic locals, dead branches, volatile barriers, or alias averaging were used to raise the reported score.

## Per-function results

Instruction counts are `original ARM / current ARM`; `strict` is the verifier's ordered fuzzy match. Alias rows remain visible but are excluded from the aggregate above.

| Function / alias | Instructions (original / current) | Baseline source-shape | Final source-shape | Final strict |
|---|---:|---:|---:|---:|
| `_ZN7StarMap16drawOnScreenInfoEib` | 1207 / 1007 | 49.0% | 78.9% | 17.4% |
| `_ZN7StarMap10OnTouchEndEii` | 1037 / 616 | 17.4% | 66.6% | 25.3% |
| `_ZN7StarMap6updateEi` | 971 / 700 | 74.7% | 76.6% | 35.1% |
| `_ZN7StarMap4drawEv` | 916 / 362 | 38.5% | 49.7% | 26.4% |
| `_ZN7StarMap4initEbP7Missionbi` | 750 / 517 | 2.5% | 72.9% | 23.2% |
| `_ZN7StarMap14initStarSystemEv` | 710 / 541 | 66.1% | 80.9% | 13.9% |
| `_ZN7StarMapC1EbP7Missionbi` | 333 / 274 | 3.4% | 85.1% | 44.2% |
| `alias of StarMap::StarMap (C1)` | 333 / 274 | 3.4% | 85.1% | 44.2% |
| `_ZN7StarMap12OnTouchBeginEii` | 275 / 270 | 17.1% | 91.6% | 45.1% |
| `_ZN7StarMap11OnTouchMoveEii` | 265 / 231 | 80.6% | 81.3% | 35.5% |
| `_ZN7StarMap7drawKeyEv` | 246 / 208 | 8.1% | 59.5% | 22.9% |
| `_ZN7StarMap6departEb` | 205 / 204 | 62.9% | 94.9% | 60.1% |
| `_ZN7StarMapD1Ev` | 69 / 71 | 92.0% | 92.0% | 48.6% |
| `alias of StarMap::~StarMap (D1)` | 69 / 71 | 92.0% | 92.0% | 48.6% |
| `_ZN7StarMap6renderEv` | 47 / 47 | 20.1% | 95.7% | 78.7% |
| `_ZN7StarMap10initLightsEv` | 21 / 17 | 87.8% | 87.8% | 57.9% |
| `_ZN7StarMap24askForJumpIntoAlienWorldEv` | 21 / 21 | 86.6% | 97.1% | 85.7% |
| `_ZN7StarMap8setStartEii` | 17 / 15 | 88.9% | 88.9% | 62.5% |
| `_ZN7StarMap14isInPlanetModeEv` | 5 / 5 | 100.0% | 100.0% | 100.0% |
| `_ZN7StarMap14setJumpMapModeEbb` | 3 / 3 | 86.7% | 86.7% | 33.3% |
| `_ZN7StarMap14missionChangedEv` | 2 / 2 | 90.0% | 90.0% | 50.0% |
| `_ZN7StarMap8renderBGEv` | 1 / 1 | 100.0% | 100.0% | 100.0% |

## Accepted ARM-backed changes

- Removed uninitialized function/global pointer shims that compiled as null calls and cut off constructor/init/render tails. Replaced them with the matching `Globals` services and direct `AEGeometry::render()` calls. This raised constructor source-shape from 3.4% to 85.1%, `init` from 2.5% to 72.9%, `render` from 20.1% to 95.7%, and `depart` from 62.9% to 94.9%.
- `drawKey` now takes horizontal placement from `Globals::w` and vertical placement from `Globals::h`, matching ARM at `0x0c80e8`. Its six text-width IDs are `(400, 401, 547, 556, 555, 274)`, read directly from ARM `.rodata` at `0x1fdd60`; sequential IDs were rejected.
- `drawOnScreenInfo` now restores Android campaign markers for mission IDs 52/116/120/125, campaign and freelance targets, pending products, current-point pulse, selected system race/security labels, align/placement flags, and icon row placement. ARM address `0x0c8354`; source-shape rose from 49.0% to 78.9%.
- `initStarSystem` now uses the ARM-backed unique station-angle selection, station-scale table at `0x1fdd78`, planet-texture map at `0x1fdd14`, system-name image IDs at `0x1fdde8`, ring rotation/scaling, root rotation, planet scale cases, lighting values and post-build random seed. ARM address `0x0c7580`; source-shape rose from 66.1% to 80.9%.
- `OnTouchEnd` now distinguishes the current station confirmation from a remote station prompt and restores confirmed campaign equipment/passenger restrictions and localized warnings (IDs 419, 532, 3213, 3214, 3217, 3218). ARM address `0x0cae90`; source-shape rose from 17.4% to 66.6%.
- `draw()` filters map systems and route endpoints through the status visibility array, as in the Android path. Android address `0x0c90f4`.
- `init` preserves confirmed camera constants, mission target lookup, cargo item `122`, and background transforms. Android address `0x0c6bf0`.

### Rejected or deferred forms

- Rejected the prior sequential `0x112 + i` width loop after ARM `.rodata` contradicted it.
- Did not invent mission paths/camera coordinates where the decompile and current typed layout did not prove the data mapping.
- Deferred further `draw`/`OnTouchEnd` mission-route and cargo branches where required table/layout mappings remain unresolved; the current score is reported as measured, not extrapolated.
- An initial focused invocation scanned all object files despite filtering symbols; it was stopped and replaced with the verifier's unit-scoped `game/world/StarMap` run. This was a validation-command correction, not a source variant.

## Verification

- Focused report: `_work/starmap-arm-stations-trial1.json`; 22 symbol comparisons, 20 unique implementations, no skipped symbols. ARM build output: 204 translation units compiled, 0 failed.
- Native build: `cmake --build cmake-build-ucrt --target gof2 -- -k 0` — passed.
- Fresh full ARM corpus: `_work/starmap-arm-full-corpus-final.json` (log: `_work/starmap-arm-full-corpus-final.log`) — 204/204 translation units compiled; 4614 comparisons; 0 skipped; overall fuzzy 75.55%, source-shape 88.73%; linked-exact 2039; byte-exact 932. Coverage: 4274/4524 original functions compared; 250 missing (7 wrong type, 243 absent). The full run retained the byte-exact count and increased linked-exact by one versus the comparable prior full report.
- Comparable full-corpus baseline `_work/texture-create-arm-corpus-final.json` was produced immediately before this task at `fc7a491e`; source files were identical between `fc7a491e` and `ef904303`, so it is a valid prior-source baseline. It recorded 4,613 comparisons, 2,038 linked-exact, 932 byte-exact, 75.48% fuzzy and 88.6% source-shape.
- `git diff --check` was run after removing the trailing blank line. No tests were added or run; the requested native and ARM verifiers are the validation used here.
