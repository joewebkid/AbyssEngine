# CutScene resetCamera (2026-10-17)

The original Android ARM `CutScene::resetCamera` at `0x98514..0x98628`
branches on modes 23 and 4. For either mode, race 1 enables fog.
`FogSetParameter` receives mode `0x2601` (linear), start `0`, density
`1.0f`, and color `0x011e0cff`. The end distance is `30000.0f` for
mode 23 (`0x985f0`) or `5000.0f` for mode 4 (`0x98604`). `FogEnable`
receives `(true, 1)`. The perspective call uses the corresponding
camera ID at `CutScene+0x70` or `+0x6c`; the literals are
`0.8/200/100000` for mode 23 and `1.2/200/48000` for mode 4.

After mode-23 perspective, ARM loads the first enemy and tail-branches
through an interworking veneer at `0x19b9a8` to the PLT entry at
`0x62010`, `KIPlayer::getType()`. The result is unused. The previous
C++ only loaded that enemy without calling `getType`; this call is
restored. All values, IDs and the veneer target were checked against
ARM instructions and literal words.

The prior source passed dummy fog mode, placeholder end distance and
color, camera ID zero and placeholder perspective. An extracted host
test checks the production `resetCamera` body at `-O0/-O2` for modes
23, 4 and 2 with races 1 and 0, including the conditional fog, camera
IDs, perspective and `getType` call.

Focused ARM `resetCamera` rises from **77.9% source-shape / 34.3%
strict fuzzy** (`79/102` rebuilt/original instructions) to
**91.6% / 57.3%** (`90/102`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the earlier mode-4 and mode-23 smokes pass. Full-corpus
report `A:/Galaxy on Fire new version/_work/cutscene_reset_camera_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_init_mode4_full_20261017.json`.

The host test checks call flow, not actual ARM32 gameplay, GPU fog or
frame output. Remaining `CutScene::process` camera/fog branches still
contain placeholders and need separate ARM audits.
