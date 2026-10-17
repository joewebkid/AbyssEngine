# CutScene turret position cleanup (2026-10-17)

The Android ARM `CutScene::checkForTurret()` cleanup at
`0x98db0..0x98e06` first returns on a null outer position list.
For every nonnull inner slot, it calls
`ArrayReleaseClasses<Vector*>(*slot)`, destroys and deletes the
inner array, and clears the outer pointer. After the loop, it calls
`ArrayReleaseClasses<Array<Vector*>*>(*positions)`, destroys the
outer array, and deletes its allocation. The outer release call was
missing from the recovered C++.

`FileRead::loadWeaponPositions` constructs an outer array of four
slots and initializes each pointer to null. The corrected cleanup
preserves the existing inner release and adds the outer release
before `delete positions`. The production-fragment host test runs
this path at `-O0/-O2` with two populated and two empty slots and
checks that both vectors are destroyed once. The test also covers
the null-outer-list return. It does not validate every ownership
path inside `FileRead` or a live ARM32 run; the function still
dereferences a null return earlier if its data file is unavailable,
as does the observed original path.

Focused ARM `checkForTurret` source-shape/strict fuzzy changes from
**93.8%/48.7%** to **93.3%/48.5%**, still nonexact. This is a
source-backed cleanup call, not a metric-only edit. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), and
the prior turret motion/transform tests pass. Full-corpus results
are in [PROJECT_STATUS.md](PROJECT_STATUS.md) and
[VALIDATION.md](VALIDATION.md).

The full ARM corpus compares **4,323/4,524** original functions in
**4,721** comparisons. The outer-release template gains one
linked-exact match (**2,518** total) without losses; byte-exact
remains **1,076**. Rounded corpus source-shape/strict fuzzy stays
**93.74%/82.87%**.
