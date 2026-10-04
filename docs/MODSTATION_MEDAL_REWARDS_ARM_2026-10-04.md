# ModStation medal rewards from Android ARM (2026-10-04)

Scope: `ModStation::checkMedals()` and `ModStation::addAchievement(int, int)`
on the local `codex/blueprint-isunlocked-arm-20261002` branch. Primary
reference: Android 2.0.16 `libgof2hdaa.so`, `checkMedals` at
`0x0d8b1c..0x0d8cf0` and `addAchievement` at `0x0dbff8..0x0dc078`.
This is a source and ARM-code recovery, not a playable-game claim.

## Confirmed bindings and behavior

- Both medal-credit paths in `checkMedals` index the four-word table at
  `.rodata 0x1fb848`: `0, 5000, 2500, 1000`. The index is the medal's
  stage (`medal[1]`). The same table is used by `ChoiceWindow::draw` for
  the displayed reward. Both previous credit pointers were permanently null.
- The Android function loads `Globals::status` and
  `Globals::achievements` via GOT entries `0x21015c` and `0x210214`.
  `checkMedals` now follows those bindings for mission/hardcore checks,
  credit updates and the new-medal list.
- `addAchievement` uses `Globals::achievements` and one three-element global
  array: GOT `0x20fdf4` relocates to `g_android_current_achievements`.
  The array is already defined in `src/platform/android/JniBridge.cpp` and
  read by the achievement-ID JNI entry point. Its three original stores
  update slots 0, 1 and 2, respectively; the three previous source pointers
  were null. The recovered function is linked-exact under NDK r18b.

The original does not show a range guard before indexing the four-word
credit table. This pass does not invent one or claim malformed medal stages
are safe. At `0x0d8bc8`, the original also makes an unusual
`addAchievement(0, 1)` call while `r0` still holds the campaign-mission
result `1`. The callee does not access `this` in the observed ARM body; the
source preserves this call shape, but its C++ receiver deserves separate
runtime/ABI review.

## Verification

| Check | Before | After |
| --- | ---: | ---: |
| `checkMedals` source-shape | 92.5% | 94.5% |
| `checkMedals` strict fuzzy | 55.3% | 52.3% |
| `addAchievement` source-shape | 80.2% | 100.0% |
| `addAchievement` strict fuzzy | 62.4% | 100.0% |
| Full compared originals | 4,322/4,524 | 4,322/4,524 |
| Full mean source-shape | 90.09% | 90.09% |
| Full mean strict fuzzy | 77.23% | 77.24% |
| Linked-exact / byte-exact rows | 2,164 / 977 | 2,165 / 977 |

The UCRT64 `gof2` native library and all 206 matching ARM translation units
build. The complete `--fail-on-wrong-type` report has 4,675 comparison rows,
202 absent originals, zero wrong-signature and zero skipped. Exact
`(unit, symbol)` set comparison finds one new linked-exact pair
(`addAchievement`) and no linked- or byte-exact losses. `checkMedals` is not
exact. Focused report:
`_work/blueprint-isunlocked/modstation_medal_after.json`; full before/after
reports: `choice_draw_full.json` and `modstation_medal_full.json` in the same
ignored directory.

## Limits and next priority

Neither achievement notifications nor credit changes were exercised in an
integrated game session. `ModStation::medalArray` is still typed as `int *`
with casts to `Array<int *> *`; the observed 32-bit layout supports a typed
field, but native 64-bit ABI and ownership should be audited separately.
The unusual `addAchievement` receiver noted above is part of that review.
