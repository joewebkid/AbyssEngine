# BluePrint destructor ARM lifetime correction — 2026-10-02

## Evidence and accepted form

Android HD `libgof2hdaa.so` at `0x176fdc` (symbols
`_ZN9BluePrintD1Ev` and `_ZN9BluePrintD2Ev`) releases the owned
`Array<int>` payload with `ArrayRelease<int>` before destroying and deleting
the array object, nulling the field, and destroying `stationName`. The
Android IDA dump in `analysis/gof2_libgof2hdaa_full_ida.c` shows this
sequence. The previous C++ destructor used `delete ingredientCounters`
alone, so the explicit release call was missing. The accepted form adds
`ArrayRelease(*ingredientCounters)` inside a non-null ownership
branch, then keeps deletion and nulling. The existing `ArrayRelease<int>`
implementation is itself linked-exact in the ARM corpus. No shared `Array`
implementation or neighboring BluePrint methods changed.

## Verification

Baseline is local commit `dda739f6`, which already includes the separate
`isUnlocked` ABI correction. The focused D1/D2 report changed from **83.1%
source-shape / 89.7% strict fuzzy**, neither exact, to **100% / 100%** and
**linked-exact** for both aliases. Local reports:
`_work/blueprint-isunlocked/baseline_destructor.json` and
`_work/blueprint-isunlocked/after_destructor.json`.

UCRT64 `gof2` rebuilt successfully; all **204/204** ARM translation units
compiled. The final full corpus compared **4,620 rows**, covering the same
**4,274/4,524** original symbols with **0 skipped**. Linked-exact rows
increased **2,080 → 2,083**; byte-exact remained **960**. A complete
`(unit, mangled symbol)` set comparison against the previous full report
found **zero lost exact pairs**. The three gained linked-exact rows are D1,
D2, and the newly emitted `ArrayRelease<int>` instantiation in the BluePrint
translation unit. They are comparison rows, not three newly recovered
distinct original bodies. The 250 missing names remain 7 wrong-signature
and 243 absent. Final local report:
`_work/blueprint-isunlocked/destructor_full.json`.

Native gameplay execution was not tested. `addItem()` and
`getIngredientsValue()` remain non-exact, but their Android control flow did
not yet reveal a justified semantic edit; their residual register/branch
differences must not be changed solely to raise a metric. The next evidence
target is `addItem()` station-name ownership and cleanup on the remote
station path.
