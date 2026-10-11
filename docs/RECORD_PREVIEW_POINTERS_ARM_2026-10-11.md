# Record preview station and ship pointers — Android ARM (2026-10-11)

Android ARM `RecordHandler::recordStoreWritePreview(GameRecord *, int)` at
`0xcfea4` loads the station pointer from `GameRecord + 0x138` before
`Station::getName`, and the ship pointer from `GameRecord + 0x130` before
`Ship::getIndex`. The live-status overload at `0xcfd78` instead calls
`Status::getStation` and `Status::getSystem` before serializing their
names. The previous C++ called `getName()` on a local `String` cast to
`Station` or `SolarSystem`, and narrowed saved ship/station pointers
through 32-bit integer fields on native builds.

The ARM class now exposes typed pointer members at `0x130` and `0x138`.
The 64-bit native class keeps those raw slots for the existing layout
but stores usable pointers in aligned sidecar members. Reader assignment,
preview writing, module restoration, and the other known uses of these
two pointers now pass through typed accessors. The status overload calls
the real station/system getters. A search of `src/game/mission` found
no remaining direct use of the old raw fields outside the conditional
layout declaration.

Focused Android comparison: `recordStoreWritePreview(int)` improves
from 85.0% to 88.2% source-shape and from 31.7% to 45.6% strict
fuzzy. The `GameRecord *` overload improves from 85.9% to 86.3%
source-shape and from 60.1% to 61.1% strict fuzzy. The larger
`recordStoreRead` shifts from 91.8% to 91.6% source-shape and from
23.3% to 16.7% strict fuzzy; it was not exact before or after.

Native `gof2`, all 206 ARM units, and 36 verifier tests pass (one
skipped). The full corpus compares 4,323/4,524 original functions:
92.24% average source-shape, 80.43% strict fuzzy, 2,352 linked-exact,
1,008 byte-exact, zero skipped units, and zero wrong-type symbols.
No previously exact pair was lost. A live save/reload round trip remains
untested; other
`GameRecord` pointers still occupy raw 32-bit fields and can be unsafe
on native 64-bit builds. Next audit those remaining pointer slots and
establish an executable serialization test before claiming runtime
correctness.
