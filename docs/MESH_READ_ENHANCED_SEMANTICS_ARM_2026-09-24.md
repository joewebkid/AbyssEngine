# Mesh::ReadEnhancedDataFromFile ARM semantics (2026-09-24)

Target: Android ARMv7 `AbyssEngine::Mesh::ReadEnhancedDataFromFile(unsigned int, unsigned int)` at `0x6bbec` (`_ZN11AbyssEngine4Mesh24ReadEnhancedDataFromFileEjj`).

## Recovered behavior

- Animation timing ignores non-positive values and retains the smallest positive interval. The ARM path first checks the candidate against zero, then replaces the current interval only when the current interval is greater than the candidate. The previous C++ comparison selected the maximum instead.
- For the first three animation groups, ARM handles types `0` and `1`; an unrecognized type falls through to the next group header instead of returning failure. The three `goto fail` branches did not match that control flow.
- The optional UV animation group handles type `2`; other type values continue to the following section. The `type != 0` failure check was not present in the ARM flow.

Read failures still take the existing cleanup path. This change only corrects timing selection and unsupported-section routing.

## Verification

| Check | Before | After |
|---|---:|---:|
| Focused source-shape | 92.0% | 94.5% |
| Focused exact match | no | no |
| Full ARM comparisons | 4,613 | 4,613 |
| Compared unique / original | 4,274 / 4,524 | 4,274 / 4,524 |
| Missing (wrong signature / absent) | 250 (7 / 243) | 250 (7 / 243) |
| Linked-exact | 2,037 | 2,037 |
| Byte-exact | 932 | 932 |

The exact-symbol sets were compared by object and symbol; no linked-exact or byte-exact entry was lost. The full ARM object build compiled 204 translation units with zero failures. The focused ARM report and full-corpus snapshots are generated under `cmake-build-match/verify/` and are local build artifacts.

The native `gof2` target builds successfully with the UCRT64 CMake build. Focused strict fuzzy similarity changed from 37.7% to 22.4%; this is not an exact-match signal. The source-shape score and ARM branch behavior are the relevant evidence for this semantic correction.
