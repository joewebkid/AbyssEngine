# CutScene weapon-position loader lifetime (2026-10-17)

Android ARM `CutScene::checkForTurret()` at `0x98cfa..0x98d20`
allocates a one-byte `FileRead`, constructs it, asks the current
ship for its index, calls `FileRead::loadWeaponPositions(index)`,
then destroys and deletes `FileRead` **before** reading the returned
position array. `Level.cpp` already uses the same heap lifetime.
The previous `CutScene.cpp` declared a separate incomplete
`FileRead` class and kept a stack instance alive until function
exit. The code now includes the real header and follows the
observed lifetime.

`FileRead::loadWeaponPositions` at `0x1212e4..0x1214a0` checks for
`data/bin/weapons_hd.bin` and returns null on the missing-file
branch (`0x12148a`). The original `CutScene` dereferences the
returned array at `0x98d24` before its later null check at
`0x98db0`. Therefore a missing data file is an original path
precondition or crash risk, **not** evidence for a speculative
null-guard in the reconstructed source. A live asset-backed run
and failure-path policy still need separate work.

The extracted host `-O0/-O2` test checks constructor, ship lookup,
loader call and destructor order, and that no `FileRead` instance
remains alive before positions are used. Focused ARM
`checkForTurret` source-shape/strict fuzzy rises from
**93.3%/48.5%** to **96.0%/65.0%**, still nonexact. Native `gof2`,
**208/208** ARM units, 40 verifier tests (one existing skip), and
the preceding turret tests pass. Full-corpus results are in
[PROJECT_STATUS.md](PROJECT_STATUS.md) and
[VALIDATION.md](VALIDATION.md).

The full corpus compares **4,323/4,524** originals in **4,721**
comparisons. Rounded source-shape/strict fuzzy remains
**93.74%/82.87%**; exact sets retain **2,518 linked-exact** and
**1,076 byte-exact** with no losses. A separate focused baseline for
`FileRead::loadWeaponPositions` is **83.1%/36.0%** and guides the
next bounded recovery package.
