# Systems catalog asset cross-check (2026-10-17)

The next file-stack integration package extends the existing host
test from station records to their containing solar systems. The
unmodified production bodies of `FileRead::loadSystemsBinary`,
`SolarSystem::SolarSystem`, `SolarSystem::~SolarSystem` and
`SolarSystem::stationIsInSystem` are compiled into the test, alongside
the unchanged AEFile stack. The only host boundary is the native
read-only loose-asset `FileInterface` and existing unreachable
player-status stubs. The local `systems.bin` is Android 2.0.16 data;
it is not committed.

At both `-O0` and `-O2`, all **34** records load in index order.
Selected names, map coordinates, jumpgate IDs, station membership
and visibility match independently decoded `systems.json` for
systems 0, 18 and 33. Every one of the **135** separately loaded
station IDs is contained in the system named by the station record.
Before mounting assets, `loadSystemsBinary` returns null. Across the
five tested files, the follow-up per-system loader brings the total
to **42** opens and **42** closes. The native `gof2`
build and verifier unit tests pass.

The Android C++ source and ARM target are unchanged. The most recent
full ARM corpus compares 4,323/4,524 original functions; averages are
93.76% source-shape and 82.91% strict fuzzy, with 2,521 linked-exact
and 1,076 byte-exact. `loadSystemsBinary` in that report is 97.1%
source-shape and 63.4% strict fuzzy, neither exact. These are inherited
metrics, not an improvement from this host test.

A direct native link of the whole `FileRead.cpp`, `Item.cpp`,
`Ship.cpp` and `Station.cpp` was also attempted. It still fails on
unrecovered neighboring dependencies (Status, Mission, Agent, math
and other world services); the fixture therefore continues to use
extracted, unmodified production bodies. No gameplay scene, save
path or complete `gof2` executable is claimed. The next priority is
to turn the station/system catalog into a controlled station-to-flight
entry with explicit platform-boundary failures. The nonascending
K'ontrr list is detailed in
[the station-order audit](FILE_READ_SYSTEM_STATION_ORDER_ARM_2026-10-17.md).
