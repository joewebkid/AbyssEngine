# CutScene mode-23 camera initialization (2026-10-17)

The Android ARM `CutScene::initialize` branch at `0x98870..0x9892c`
creates a camera through the output ID at `CutScene+0x70`, uses that
ID for perspective, current-camera selection, a 60-byte local-matrix
copy and local-matrix submission. The perspective literals at
`0x98a10..0x98a18` are `0.8f`, `200.0f` and `100000.0f`. It rotates
the copied matrix around Y by the literal with bits `0x3ec90fdb`
(`0.3926991f`) and translates it by `(300, 700, 3000)`.

The branch then creates a transform through the output ID at
`CutScene+0x78`. The `Level::getEnemies` result is dereferenced to
its first `KIPlayer`, whose geometry at player offset `+8` supplies
`getPosition()`. The three returned coordinates are passed to
`MatrixSetTranslation` for the transform's local matrix. Finally the
camera is attached under that transform, followed by `resetCamera`
and `checkForTurret`. These offsets, calls, and literals were checked
against the original ARM instructions and data words; decompiler
pseudocode was only supplementary.

The prior C++ used camera/transform ID zero, zero-valued placeholder
transforms, an integer camera ID cast to `Matrix*`, and a null
`AEGeometry*` dereference. The recovered branch now uses the observed
IDs, data flow and literals. The `-O0/-O2` host smoke test extracts
the production branch statements and checks the perspective, matrix
copy, geometry position, transform attachment and final calls.

Focused ARM `initialize` rises from **88.9% source-shape / 36.2%
strict fuzzy** to **92.3% / 37.0%**, still nonexact at `377/380`
rebuilt/original instructions. Native `gof2`, **208/208** ARM units,
40 verifier tests (one existing skip), this smoke and the earlier
CutScene camera/time smokes pass. Full report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode23_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_init_mode2_full_20261017.json`.

The host smoke verifies call and value flow, not actual ARM32 gameplay
or GPU rendering. Other `initialize` paths still have placeholders;
the next bounded priority is the mode-4 setup, checked separately
against the original binary.
