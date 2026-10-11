# GameRecord preview strings — Android ARM (2026-10-11)

Android `GameRecord::GameRecord` at `0x155900` constructs `String` objects
at offsets `0x188` and `0x194`; `GameRecord::~GameRecord` at `0x1559d4`
destroys both, in reverse order. The first preview string read into `0x194`
is the station name written by `Status::getStation` / `Station::getName`;
the second at `0x188` is the system name written by
`Status::getSystem` / `SolarSystem::getName`. The former C++ treated these
objects as six raw 32-bit words and cast their addresses to `String`,
without constructing or destroying them.

On 32-bit ARM, the class now declares two 12-byte `String` members at the
confirmed offsets, with compile-time size and offset checks. On 64-bit
native builds, the preview strings are separate, aligned members after
the existing raw fields, so a 24-byte native `String` cannot overwrite
nearby fields. Preview I/O and the save-slot menu use typed accessors.

Focused Android comparison: both constructor aliases improve from 58.8%
to 92.2% source-shape and from 36.5% to 63.6% strict fuzzy. Both
destructor aliases improve from 2.2% source-shape / 0% strict fuzzy to
100% / 100% and become linked-exact. Native `gof2`, all 206 ARM units,
and 36 verifier tests pass (one skipped). Full corpus compares
4,323/4,524 original functions: 92.24% average source-shape, 80.43%
strict fuzzy, 2,352 linked-exact, 1,008 byte-exact, zero skipped units,
and zero wrong-type symbols. No previously exact pair was lost.

This confirms ARM construction/destruction and native compilation, not a
64-bit save/reload round trip. An isolated native smoke executable cannot
currently link `GameRecord.cpp` because its larger `load()` path still
references unresolved game functions. `GameRecord::load` also retains
32-bit raw-offset assumptions. Next audit the station lookup in
`RecordHandler::recordStoreWritePreview(GameRecord *, int)`, then enable
a focused serialization round trip once its link dependencies are closed.
