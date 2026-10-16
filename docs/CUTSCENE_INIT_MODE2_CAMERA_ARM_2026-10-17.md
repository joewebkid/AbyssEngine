# CutScene mode-2 camera initialization (2026-10-17)

The Android ARM `CutScene::initialize` mode-2 path at
`0x9870e..0x987b2` creates a camera through the output ID at
`CutScene+0x74`, then uses that ID for perspective, current-camera,
local-matrix read and local-matrix write. Perspective literals are
`0.92f`, `200.0f` and `200000.0f`. It copies 60 bytes of the local
matrix, draws random integers with bounds `20000` and `60000`, and
translates the copy by `(first - 20000, 0, second + 40000)`.
It sets `cameraRotX` to the literal with bits `0xbf490fdb`
(`-0.7853982f`), rotates the same matrix around Y and submits it.
REA/IDA pseudocode for the same binary (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
agrees on the call sequence; the literal values, registers and
offsets above were independently checked in ARM disassembly.

The prior source used camera 0, zero-valued placeholder perspective,
unbounded random draws, a separate translation buffer, zero rotation
and an integer camera ID cast to `Matrix*`. The mode-2 path now follows
the observed calls and values. A host `-O0/-O2` smoke test extracts
the production setup statements and checks the output ID, perspective,
random bounds, translation, rotation and copied-matrix ownership.

Focused ARM `initialize` improves from **86.9% source-shape / 35.7%
strict fuzzy** to **88.9% / 36.2%**, still nonexact at `360/380`
rebuilt/original instructions. Native `gof2`, **208/208** ARM units,
40 verifier tests (one existing skip), this smoke and the earlier
CutScene camera/time smokes pass. Full report
`A:/Galaxy on Fire new version/_work/cutscene_init_mode2_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_mode2_camera_full_20261017.json`.

The host test records calls but does not check real GPU rendering or
full ARM32 gameplay. Other `initialize` branches remain nonexact.
Next priority is the separate mode-23 camera setup: Android uses
different perspective, rotation and translation literals, so mode-2
values must not be reused there without its own verification.
