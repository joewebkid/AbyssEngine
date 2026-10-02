# SolarSystem runtime globals — Android ARM, 2026-10-02

Android HD `libgof2hdaa.so` at `0x155818` reads the current station
through `Globals::status`; at `0x15585c` it reads the visited-station
flags through `Globals::galaxy` inside the iteration. The IDA extraction
in `gof2_recovered_sdk_first_pass/extracted/world_level_missions/SolarSystem.c`
shows both flows. The local `Globals::init` assigns these globals when
creating `Status` and `Galaxy`. The former C++ instead dereferenced two
never-initialized static pointer roots, and cached the galaxy pointer
outside the discovery loop. The accepted change removes those roots and
uses the observed globals at the observed points. It adds no new null
guards because the original assumes initialized game state.

Focused ARM comparison improved `currentOrbitHasWarpGate()` from
**84.2% source-shape / 78.3% strict fuzzy** to **100% / 100% linked-exact**,
and `isFullyDiscovered()` from **83.8% / 63.8%** to **100% / 100%
linked-exact**. Neither became byte-exact. Local focused reports:
`_work/blueprint-isunlocked/baseline_solarsystem_runtime_roots.json` and
`_work/blueprint-isunlocked/after_solarsystem_runtime_roots.json`.

The native UCRT64 `gof2` target rebuilt and **204/204 ARM translation
units** compiled. The full corpus compared **4,620 rows**, covering
**4,274/4,524** original symbols with **0 skipped**. Linked-exact rose
**2,090 → 2,092**, byte-exact remained **960**. Complete `(unit,
mangled symbol)` set comparison found **zero lost exact pairs** and the
two functions above as the only gained pairs. The 250 missing symbols
remain 7 wrong-signature and 243 absent. Mean source-shape rose **89.04%
→ 89.05%** and mean strict fuzzy **76.05% → 76.06%** (rounded). Local full
reports are `_work/blueprint-isunlocked/solarsystem_tables_full.json`
and `_work/blueprint-isunlocked/solarsystem_runtime_roots_full.json`;
generated reports are not committed.

Native gameplay execution was not tested. The next priority is to audit
the seven wrong-signature symbols and then select a bounded ABI or
behavior correction outside this completed `SolarSystem` root-pointer
cluster. A low source-shape percentage alone is not a reason to alter
otherwise source-backed code.
