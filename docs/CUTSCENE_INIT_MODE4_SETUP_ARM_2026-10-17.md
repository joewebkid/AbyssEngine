# CutScene mode-4 setup (2026-10-17)

The Android ARM `CutScene::initialize` mode-4 path at
`0x9868c..0x9870c` creates a camera through the output ID at
`CutScene+0x6c`. Its `CameraSetPerspective` call loads that same ID
and the literals at `0x98a40`, `0x98a14` and `0x98a44`:
`1.2f`, `200.0f` and `48000.0f`. It then deletes an existing follow
camera, selects the new camera and calls `resetCamera`.

For race 3, the ARM code constructs mesh `0x36d6`, loads the
`AEGeometry+0x0c` transform ID and calls `TransformGetTransform`
before `SetAnimationState(0, nullptr)`. For race 0, it constructs
meshes `0x37c8` and `0x37c7`, passes the latter geometry's
`+0x0c` transform to the former's `addChild`, then deletes the
temporary geometry. For other races, the original makes a second
`getRace()` call and does not create either mesh. These calls and
offsets were checked in ARM disassembly, including the separate
`0x9892e..0x98988` race-0 path.

The previous C++ sent placeholder perspective to camera 0 and used
`childTransform` instead of the observed primary `transform` in
both geometry paths. The recovered branch now follows the original
IDs, values and handles. A host `-O0/-O2` smoke test extracts the
production statements and exercises races 3, 0 and 1, checking
camera IDs, perspective, follow-camera destruction, transform handle
selection and mesh attachment.

Focused ARM `initialize` improves from **92.3% source-shape / 37.0%
strict fuzzy** to **93.1% / 37.9%**, still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the earlier mode-23 and mode-2 smokes pass. Full-corpus
report `A:/Galaxy on Fire new version/_work/cutscene_init_mode4_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_init_mode23_full_20261017.json`.

The host smoke verifies the setup call flow but not real GPU output
or live ARM32 gameplay. `resetCamera` itself still contains
placeholder perspective and fog calls and needs a separate ARM audit.
