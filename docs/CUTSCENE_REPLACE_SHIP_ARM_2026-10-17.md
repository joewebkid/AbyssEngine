# CutScene ship replacement (2026-10-17)

Android ARM `CutScene::replacePlayerShip(int,int)` at `0x99668..0x99770`
keeps the first argument in `r8` and the second in `r6`. It passes
both to `Globals::getShipGroup(first, second, false)` at `0x996dc..0x996ea`.
The first argument also indexes the signed 32-bit vertical-offset
table at `.rodata 0x1fb934..0x1fba33`; those **64** words match the
separate hangar offset table already recovered in `Level.cpp`. The
five words starting at `0x1fba34` are item IDs, not more offsets.

For a present old geometry, the original detaches its main transform
ID (`AEGeometry+0x0c`) from the turret's main transform ID, copies
the 60-byte matrix to the new ship group, converts the selected
vertical offset to float, and invokes the KIPlayer virtual
`setPosition(const Vector&)` slot at `vtable+0x44` with `(0, offset, 0)`.
It hides the exhaust, removes the old geometry through
`Level::lodManager` (`Level+0x00`), deletes it, and calls
`checkForTurret()`. A null enemy list returns without that final
call; a missing old geometry still calls it. The original performs
no explicit bounds check on the ship index; this patch does not
invent one.

Before the patch, the first argument was ignored, the group variant
was forced to zero, the zero-initialized offset table disabled the
height adjustment, child transform IDs were used, the float-position
overload was called, and the LOD-manager pointer came from the wrong
object. The source now follows the ARM data flow. The extracted
host `-O0/-O2` test exercises argument separation, offsets at
indices 19 and 63, matrix preservation, transform IDs, LOD removal,
exhaust hiding and both early-exit branches. It does not establish
live ARM32/GPU behavior or valid caller index ranges.

Focused ARM source-shape/strict fuzzy rises from **94.7%/64.9%**
to **98.0%/89.5%**; the function is still nonexact. Native `gof2`,
**208/208** ARM units and 40 verifier tests (one existing skip) pass.
Full-corpus results are in [PROJECT_STATUS.md](PROJECT_STATUS.md) and
[VALIDATION.md](VALIDATION.md).
