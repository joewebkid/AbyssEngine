# Galaxy::getStation lifetime and player pointer (2026-10-17)

Android 2.0.16 `libgof2hdaa.so`, symbol `_ZN6Galaxy10getStationEi`,
shows two paths at `0x175eb0`–`0x175ede`. A negative ID returns the
`playerStation` pointer at offset `0x78` through `Globals::status`.
For a nonnegative ID, the function allocates a `FileRead` object,
constructs it, calls `loadStation(id)`, destroys it, and then calls
`operator delete` before returning the station. The previous source
used `Status::gStatus` and a stack `FileRead`; this change restores the
observed pointer source and object lifetime without changing the station
parser or its error behavior.

Focused ARM verification before/after: source-shape **72.0% → 100.0%**,
strict fuzzy **29.4% → 96.7%**. The function remains nonexact in both
linked and byte comparisons; the remaining focused diff moves `mov r5, r1`
across the nonnegative branch's `movs r0, #1` without changing the data
flow. The full corpus is recorded in
`docs/PROJECT_STATUS.md` and `docs/VALIDATION.md` for this revision.

The asset-backed host fixture extracts the production `Galaxy::getStation`
body. At `-O0` and `-O2`, negative ID `-1` returns the current player
station with `Status::gStatus` deliberately left null, and ID `0`
loads a real catalog station through the normal AEFile/FileRead path.
The seven mounted files have **48 opens / 48 closes**. Both native
`gof2` and `gof2_host_file` targets build; the 40 verifier unit tests
pass with one existing skip. This is host evidence for two inputs, not
proof of all invalid IDs, platform file failures, full `Status::resetGame`
or Android-device gameplay.

Next runtime slice: `Status::resetGame` calls `Galaxy::getStation(0)`;
inventory and isolate the additional settings, item, mission and ship
services it needs before attempting a station-to-flight transition.
