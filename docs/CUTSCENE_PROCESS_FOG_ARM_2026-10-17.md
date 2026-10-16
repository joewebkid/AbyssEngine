# CutScene process fog (2026-10-17)

The Android ARM `CutScene::process(int)` fog calls for race 1 are at
`0x9941e` for mode 23 and `0x98f9a` for mode 4. Both pass fog mode
`0x2601` (linear), start `0`, density `1.0f` and color
`0x011e0cff`. Literal words at `0x9963c` and `0x9927c` provide end
distances `30000.0f` and `5000.0f` respectively. Mode 4 follows
the parameter call with `FogEnable(true, 1)` at `0x98fa6`; mode 23
does not enable fog in this process branch. These values and call
sequences were checked directly in Android ARM instructions and data.

The previous C++ passed dummy mode and placeholder end/color values,
and enabled mode zero in the mode-4 branch. The recovered calls now
use the confirmed values. A host `-O0/-O2` smoke test compiles the
extracted production call fragments and checks values and whether
the enable call is present. The surrounding race conditions were
unchanged by this patch and are not exercised by that narrow smoke.

Focused ARM `process` rises from **89.1% source-shape / 47.8%
strict fuzzy** (`665/706` rebuilt/original instructions) to
**89.7% / 49.3%** (`669/706`), still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), this
smoke and the previous mode-23 transform and reset-camera smokes pass.
Full-corpus results are recorded in `PROJECT_STATUS.md` and
`VALIDATION.md`. Full report
`A:/Galaxy on Fire new version/_work/cutscene_process_fog_full_20261017.json`
compares **4,323/4,524** originals, with **201 missing** and **123
extras**. Rounded averages remain **93.73% source-shape / 82.85%
strict fuzzy**; **2,517 linked-exact** and **1,076 byte-exact**
`(unit,symbol)` pairs are unchanged from
`cutscene_process_mode23_full_20261017.json`.

Live ARM32 gameplay and GPU fog output remain untested. Mode-23
animation handling and mode-4 enemy-transform updates in `process`
still contain placeholder handles; they need separate ARM audits.
