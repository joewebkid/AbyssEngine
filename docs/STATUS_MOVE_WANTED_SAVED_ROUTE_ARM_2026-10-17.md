# Saved-route wanted movement (2026-10-17)

Android 2.0.16 ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
places `Status::moveWanted` at `0x0a9684`. At `0x0a9722` it checks
`Level::programmedStation` before comparing its station ID with a
wanted record's current location. The prior source dereferenced a
static null `Station**` instead. At `0x0a975e`–`0x0a9790` the ARM body
loads the current and destination Station objects from that wanted
record's saved IDs via `FileRead::loadStation`; the prior source chose
both randomly. Only the **reroute** branch at `0x0a983a` and `0x0a9862`
uses `Globals::getRandomStation`, through the initialized
`Globals::globals` pointer. The separate `Globals::gGlobals` static is
never assigned in the recovered constructor path.

ARM `moveWanted` at `0x0a989c` compares the **size** of the returned
`Array<int>` path with the allowed 2–4 range. Its first data element is
the starting system ID, as confirmed by `SystemPathFinder::getSystemPath`
at `0x11b8ba`; the prior source compared that first ID instead.

The asset-backed host fixture now extracts the real `moveWanted` body.
It activates one loaded wanted record with saved station IDs 62 and
59, both in system 20, after new-game startup. With a programmed
station 62, the record does not move and no file opens. With that guard
cleared, the record moves to station 59; `loadSystemsBinary` and the two
station loads produce three balanced file open/close pairs. Both `-O0`
and `-O2` pass. A second case starts with current and destination station
both 62, then feeds a controlled three-element path and a real catalog
station from an allowed visible system. It checks that the wanted record's
`travelsTo` and `lastSeen` change once, with **63/63** total file
open/close pairs. The pathfinder result and random-station choice are
explicit test-only boundaries; this does **not** validate the real
cross-system path search or stochastic distribution.

Focused ARM source-shape for `moveWanted` rises from **78.9% to 86.2%**;
strict fuzzy falls from **34.9% to 33.7%**, and the function remains
nonexact. Native `gof2`, 208/208 ARM translation units and the verifier
suite of 40 tests (one existing skip) pass. The full report at
`A:/Galaxy on Fire new version/_work/status_move_wanted_full_20261017.json`
compares **4,323/4,524** original functions in **4,730** rows, averaging
**93.78%** source-shape and **82.95%** strict fuzzy. It has **2,527
linked-exact / 1,076 byte-exact** comparisons, 201 absent originals,
zero wrong-type and zero skipped units. Pairwise comparison with the
preceding full report finds zero linked-/byte-exact gains or losses.

Unverified: repeated wanted transitions, real random candidate selection,
cross-system `SystemPathFinder` behavior, and end-to-end station-to-flight
runtime. The next bounded step is a real pathfinder cross-system wanted
move and then `Level::init` entry.
