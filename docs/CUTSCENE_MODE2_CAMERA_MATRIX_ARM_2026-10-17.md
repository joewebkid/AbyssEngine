# CutScene mode-2 camera matrix (2026-10-17)

Android ARM `CutScene::process(int)` mode 2 at `0x990c8..0x990f2`
loads the camera ID from `CutScene+0x74`, calls
`PaintCanvas::CameraGetLocal(id)`, copies the returned 60-byte matrix,
then calls `AEMath::MatrixSetRotation(matrix, 0, cameraRotX, 0)`.
It passes that local matrix and the same ID to `CameraSetLocal`.
The old source instead read camera 0, rotated an unrelated buffer at
zero angles and treated `cameraId74` as a `Matrix*` when setting camera
0. That path could dereference an integer ID as an address.

The source now copies the selected camera matrix, rotates it around
the confirmed Y angle and submits that copy. A host `-O0/-O2` smoke
extracts these production statements, records both camera IDs and
rotation arguments, and checks that the source matrix remains intact
while the submitted copy contains the mutation. ARM compilation is
**208/208**, native `gof2` builds, and 40 verifier tests pass (one
existing skip). Focused `process` source-shape/strict fuzzy changes
from **87.6%/43.6% to 88.3%/43.7%**, still nonexact at `650/706`
rebuilt/original instructions.

Full report
`A:/Galaxy on Fire new version/_work/cutscene_mode2_camera_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and
**123 extras**. Rounded averages remain **93.73% source-shape /
82.85% strict fuzzy**. Exact sets remain **2,517 linked-exact /
1,076 byte-exact**, with zero gains and zero losses against
`cutscene_camera_step_full_20261016.json`.

The smoke checks the call route, not GPU output or the actual matrix
math implementation on ARM32. Other `CutScene::process` branches and
the mode-2 `initialize` camera path remain nonexact. Next priority:
audit the mode-2 `initialize` camera setup against Android ARM before
reusing this correction there.
