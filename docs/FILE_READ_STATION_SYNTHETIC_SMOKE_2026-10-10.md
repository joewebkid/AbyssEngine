# System-specific station loader synthetic smoke (2026-10-10)

`tools/run_station_loader_smoke.py` extracts the current
`FileRead::loadStationsBinary(SolarSystem *)` body verbatim from
`src/engine/file/FileRead.cpp` and compiles it with a synthetic `AEFile`
boundary. The test exercises the production loader loop, not a copy of its
algorithm. It supplies no game assets.

The smoke verifies four cases: unsorted IDs `{5, 1, 7}` produce stations in
file-record order `{1, 5, 7}`; duplicate ID `2` produces two station
instances; ID `200` produces an empty slot after all 135 records; and an
empty ID list returns an empty array. Every case closes its file handle
once. The test also counts constructed and destroyed station instances when
the returned array is released. The smoke passes on the native host.

The engine source is unchanged from commit `c45161c0`. The native `gof2`
build reports no work, 36 verifier tests run with one optional skip, and
four scope tests pass. The last full ARM report for the same engine source
is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_loader_audit_20261010.json`:
**4,323/4,524** originals, **91.87%** source-shape, **79.93%** strict
fuzzy, **2,313 linked-exact** and **997 byte-exact**, with no exact-pair
losses. The full corpus was not rerun for this test-only commit.

This is a loop/ownership smoke with stubbed file reads, `String`, `Station`
and `SolarSystem` boundaries. It does not validate real binary decoding,
UTF-8 conversion, actual asset ID ordering, the missing-file leak, or
whole-game station-to-Radar rendering. Those remain the next runtime
validation priorities.

On the Windows/MSYS2 host, reproduce with
`python tools/run_station_loader_smoke.py --compiler C:/msys64/ucrt64/bin/g++.exe --output-dir <scratch-directory>`.
The scratch directory contains only a generated include and executable and
can be removed after the run.
