# BluePrint::addItem station reload — Android ARM, 2026-10-02

## Evidence and accepted change

Android HD `libgof2hdaa.so` at `0x177008` calls `Status::getStation()`
twice on the local-station branch: once before comparing the station index,
and again before reading its name. This is visible in
`analysis/gof2_libgof2hdaa_full_ida.c` and the extracted
`gof2_recovered_sdk_first_pass/extracted/data_economy_progression/BluePrint.c`.
The previous C++ cached the first station pointer for both operations.
`BluePrint::addItem()` now makes the second call, without changing the
remote-station branch or item accounting. The distinction matters if the
status station changes between calls; runtime behavior was not exercised.

## Verification

Focused ARM comparison of `BluePrint::addItem()` improved from **96.3% to
98.8% source-shape** and from **74.9% to 81.8% strict fuzzy**. It remains
non-exact. Local focused reports are
`_work/blueprint-isunlocked/baseline_additem.json` and
`_work/blueprint-isunlocked/after_additem.json`.

All **204/204 ARM translation units** compiled, and the UCRT64 native `gof2`
target rebuilt. The full ARM corpus compared **4,620 rows**, covering
**4,274/4,524** original symbols with **0 skipped**. Linked-exact remained
**2,083** and byte-exact **960**. A complete `(unit, mangled symbol)` set
comparison against the prior full report found **zero lost or gained exact
pairs**. The 250 missing symbols remain 7 wrong-signature and 243 absent.
Local full reports are `_work/blueprint-isunlocked/destructor_full.json` and
`_work/blueprint-isunlocked/additem_full.json`; these generated reports are
not committed.

Residual `addItem()` differences include branch and register layout. The
source-backed correction above does not justify changing station index
semantics merely to raise the similarity score. `getIngredientsValue()` also
remains non-exact and needs separate evidence before any edit.
