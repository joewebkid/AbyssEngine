# CutScene mode-2 ship motion (2026-10-17)

The original Android ARM `CutScene::process(int)` mode-2 path is at
`0x990aa..0x9921e`. Its `0x99290..0x992a4` literal pool gives the
motion coefficients as float32: `0.1`, `0.001`, `0.0002`, `0.00018`,
`0.0005`, and `0.0003`. The first ship is the penultimate enemy; the
second is the last enemy. Both use `KIPlayer+0x08` as their geometry.

For signed frame delta `d` and its signed 32-bit wrapped negation `-d`,
the observed calls are:

| Ship | `translate(x,y,z)` | `rotate(x,y,z)` |
| --- | --- | --- |
| Penultimate | `(0.1d, 0, 0.001d)` | `(-0.0002d, 0.0002d, -0.00018d)` |
| Last | `(0.1d, 0, 0)` | `(0.0002d, -0.0005d, -0.0003d)` |

The negative terms above mean multiplication by the *wrapped negated
integer converted to float*, not floating-point negation after the
conversion. This matters at the signed 32-bit minimum. The original
tests `getPlayingTime() > 0` before calling `Level::getEnemies()`;
therefore zero and negative playing times skip the list access. A null
list or fewer than two enemies also skips ship motion. A missing first
geometry does not suppress the second ship's movement.

Before this patch, zero-initialized coefficient placeholders disabled
the movement and the C++ used the wrong rotation axes. The corrected
branch uses the ARM constants and axes, and delays the enemy lookup
until the time check passes. The extracted `-O0/-O2` host test covers
both ships, skipped and missing-object paths, and a high-bit frame
delta. It does not prove live ARM32 or GPU-visible movement.

Focused `process` source-shape/strict fuzzy rises from **93.9%/62.8%**
to **94.0%/63.5%**, still nonexact. Native `gof2`, **208/208** ARM
units, and 40 verifier tests (one existing skip) pass. Full-corpus
results are recorded in [PROJECT_STATUS.md](PROJECT_STATUS.md) and
[VALIDATION.md](VALIDATION.md).
