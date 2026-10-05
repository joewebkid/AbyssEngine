# SolarSystem discovery and race tables — Android ARM, 2026-10-02

## Evidence and accepted form

Android HD `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has a first `LOAD` segment with file offsets equal to these virtual
addresses. Reading little-endian 32-bit values gives:

| Address | Use | Values |
| --- | --- | --- |
| `0x1fb7e8` | pirate-base station IDs | `1, 33, 47, 86` |
| `0x203114` | hidden-blueprint station IDs | `132, 133, 134, 129, 123` |
| `0x1fb878` | attack-race lookup | `1, 0, 3, 2` |

The Android IDA functions at `0x155720`, `0x155790`, and `0x155708`
reference these tables. The Ghidra listing independently confirms the
two discovery functions' flow on the same Android binary. Both functions
read the status flag arrays at offsets `0x4c` and `0x58`; the local
`Status` layout maps these to `field_4c` and `field_58`, allocated with
lengths 4 and 5. `Globals::status` is assigned alongside
`Status::gStatus` during game initialization.

Previously all three local tables contained only zeros, and the two
discovery functions dereferenced never-initialized static root pointers.
The accepted change restores the Android values and reads the status
arrays through `Globals::status`. The loop bounds and short-circuit
behavior remain as observed. No guard for an uninitialized game status
was added because the original functions assume initialized global state.
No separate iOS/PC table source was established for this step.

## Verification and limits

Focused ARM comparison moved both `hasPirateBase()` and
`hasHiddenBlueprint()` from **69.3% source-shape / 32.0% strict fuzzy**
to **100% / 100% linked-exact**. `getAttackRace()` remained linked-exact;
its incorrect table data was not detected by instruction matching.
The native UCRT64 `gof2` target rebuilt, and **204/204 ARM translation
units** compiled. Local focused reports:
`_work/blueprint-isunlocked/baseline_solarsystem_discovery.json` and
`_work/blueprint-isunlocked/after_solarsystem_tables.json`.

The full corpus compared **4,620 rows**, covering **4,274/4,524**
original symbols with **0 skipped**. Linked-exact rose **2,088 → 2,090**;
byte-exact remained **960**. Complete `(unit, mangled symbol)` set
comparison found **zero lost exact pairs** and exactly the two discovery
functions gained. The 250 missing symbols remain 7 wrong-signature and
243 absent. Mean source-shape rose **89.03% → 89.04%** and mean strict
fuzzy **76.02% → 76.05%** (rounded). Local full reports:
`_work/blueprint-isunlocked/solarsystem_ctor_full.json` and
`_work/blueprint-isunlocked/solarsystem_tables_full.json`; generated
reports and the original game binary are not committed.

Native gameplay execution was not tested. Next priority is to replace
the similarly uninitialized `gStatusOrbit` and `gGalaxyDiscover` roots
in `SolarSystem::currentOrbitHasWarpGate()` and
`SolarSystem::isFullyDiscovered()` using Android-backed globals.
