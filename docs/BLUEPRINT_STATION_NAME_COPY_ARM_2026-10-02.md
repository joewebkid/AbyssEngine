# BluePrint::getStationName string-copy ABI — Android ARM, 2026-10-02

Android HD `libgof2hdaa.so` at `0x177348` constructs the returned
`AbyssEngine::String` through the `(const String&, bool)` overload with
`reverse=false`. The extracted Android pseudocode also shows that call.
The prior C++ used the implicit one-argument copy constructor. Both local
constructors copy the same string when `reverse` is false, but they are
different ARM calls. `getStationName()` now explicitly chooses the observed
overload. No shared string implementation or caller changed.

Focused comparison improved from **93.0% source-shape / 90.9% strict fuzzy**
to **100% / 100%**, becoming linked-exact (not byte-exact). The prior ARM
diff consisted of the missing `movs r2, #0` argument setup. Local focused
reports: `_work/blueprint-isunlocked/baseline_stationname.json` and
`_work/blueprint-isunlocked/after_stationname.json`.

The native UCRT64 `gof2` target rebuilt and **204/204 ARM translation units**
compiled. Full corpus comparison covered **4,274/4,524** original symbols
over **4,620 rows**, with **0 skipped**. Linked-exact increased **2,083 →
2,084**, byte-exact remained **960**, and a complete `(unit, mangled symbol)`
set comparison found **zero lost exact pairs**. The sole gained linked-exact
pair is `BluePrint::getStationName()`. The 250 missing symbols remain 7
wrong-signature and 243 absent. Local full reports:
`_work/blueprint-isunlocked/additem_full.json` and
`_work/blueprint-isunlocked/stationname_full.json`; generated reports are
not committed.

Native gameplay execution was not tested. The next source-backed priority
is `SolarSystem` destructor ownership: its current `ArrayRemoveAll` calls
must be compared with the Android `ArrayRelease<int>` sequence before any
edit.
