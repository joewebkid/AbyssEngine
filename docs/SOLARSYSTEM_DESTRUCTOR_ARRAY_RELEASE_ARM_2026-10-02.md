# SolarSystem destructor array release — Android ARM, 2026-10-02

## Evidence and accepted form

Android HD `libgof2hdaa.so` at `0x155694` calls `ArrayRelease<int>` for
each of the three owned array objects at offsets 56, 60, and 64 before
deleting the array object and clearing its field. The extracted Android
`SolarSystem.c` shows the same sequence. The prior C++ destructor called
`ArrayRemoveAll` for each array instead. In the local `Array.h`, that helper
resizes the payload to one element; `ArrayRelease` directly frees and nulls
the payload, matching the observed destruction path. Only these three
calls were changed; the ownership order and null checks remain intact.

## Verification

Both `SolarSystem` destructor aliases (D1/D2) improved from **90.0%
source-shape / 93.0% strict fuzzy** to **100% / 100%**, becoming linked-exact
(not byte-exact). The D1 disassembly is identical after normalization.
Focused results are present in the prior full report and local
`_work/blueprint-isunlocked/after_solarsystem_destructor.json`.

The native UCRT64 `gof2` target rebuilt and **204/204 ARM translation units**
compiled. The full ARM corpus compared **4,620 rows** covering
**4,274/4,524** original symbols with **0 skipped**. Linked-exact rose
**2,084 → 2,087**; byte-exact remained **960**. Comparing complete
`(unit, mangled symbol)` exact sets against the previous report found
**zero lost pairs**. The three gained linked-exact rows are the two
destructor aliases and an `ArrayRelease<int>` instantiation in
`SolarSystem.o`; these do not represent three distinct recovered bodies.
The 250 missing symbols remain 7 wrong-signature and 243 absent. Local
full reports are `_work/blueprint-isunlocked/stationname_full.json` and
`_work/blueprint-isunlocked/solarsystem_destructor_full.json`; generated
reports are not committed.

Native gameplay and allocator instrumentation were not run. The next
source-backed ABI target is `SolarSystem::getName()`, whose Android code
uses the `(const String&, bool)` constructor with `reverse=false`.
