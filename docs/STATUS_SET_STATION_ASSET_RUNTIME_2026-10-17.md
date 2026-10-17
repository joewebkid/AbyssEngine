# Asset-backed Status station transition (2026-10-17)

The native file-stack smoke now executes the unchanged production
bodies of `Galaxy::Galaxy`, `Galaxy::getSystem`, `Status::Status`,
`Status::setStation` and `Status::~Status`, with the real AEFile and
FileRead resource loaders. It also uses production Station accessors.
Only unreachable player-status methods and the catalog-only Station
destructor remain fixture boundaries. The test explicitly releases
the auxiliary arrays that the tested Status destructor does not own;
it does not audit full Status destruction.

With the local Android 2.0.16 `systems.bin` and `stations.bin`, the
test first selects station 91 (Dima, system 18) while the campaign
step is zero, then selects station 58 (B'akrram, system 20 K'ontrr)
after setting a nonzero campaign step. It verifies system pointers,
visited-system flags, empty names at step zero, and every planet name
and texture in K'ontrr against the independently loaded station
catalog in the system's input-ID order. A repeated selection of the
same station performs no additional file open. Host `-O0` and `-O2`
pass with **47 opens / 47 closes** across the seven mounted catalog
assets and all per-system station tests.

Android ARM `Status::setStation(Station*)` at `0x0a90b8` looks up the
galaxy system, calls `FileRead::loadStationsBinary(SolarSystem*)` at
`0x0a9110`, then searches returned Station objects by ID for each
system input ID at `0x0a9194`–`0x0a91c0`. The restored C++ follows
that search, so K'ontrr's unsorted input list is not treated as
positionally aligned with the loader result. The source was not
changed. The latest ARM corpus still reports `setStation` at **86.9%
source-shape / 47.7% strict fuzzy**, nonexact; global counts remain
**4,323/4,524** originals compared, **93.76%/82.91%** average
source-shape/strict fuzzy, **2,521 linked-exact / 1,076 byte-exact**.
These figures are inherited, not gains from the host runtime test.

The native `gof2` and `gof2_host_file` targets build; 40 verifier
tests pass with one pre-existing skip. This is a real call through
`Status::setStation`, **not** an execution of `Status::resetGame`, a
rendered orbit, save loading or a station-to-flight scene. Error
paths for missing files or invalid station IDs remain untested. Next
priority: establish the smallest complete `Status::resetGame` or
station-departure slice, identifying platform calls that still block
an executable scene.
