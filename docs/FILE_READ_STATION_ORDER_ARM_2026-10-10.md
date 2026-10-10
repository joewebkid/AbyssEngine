# System-specific station loader audit (2026-10-10)

The Android 2.0.16 ARM `FileRead::loadStationsBinary(SolarSystem *)` body is
at `0x0012014c..0x001202b7`. It allocates an `Array<Station *>` before
checking `stations_binary.bin`. If the file is absent, `0x00120250` sets
the eventual result to null; the allocated array is not freed on that path.
This original leak is recorded, not changed here.

For a present file, `0x00120196..0x001201a2` calls
`SolarSystem::getStations()` and sizes the output array from its ID count.
The outer loop reads at most 135 station records in file order. The inner
loop at `0x001201e8..0x0012023e` compares each ID with the current record
number; a match stores the new `Station *` at the next sequential output
slot (`r8`), not at the ID-array slot (`r6`). The loader closes the file on
both normal return routes. The current source already follows that
file-order and close behavior, and the callers release the returned array
and station objects. The one confirmed source divergence in this audit was
a direct read of `system->stationIds`; it now calls `getStations()` as the
original does.

Native `gof2` and all 206 ARM units build; 36 verifier tests run with one
optional skip, and four scope tests pass. The focused loader changes from
**94.0% to 93.7% source-shape** and **48.3% to 56.2% strict fuzzy**. The
rebuilt ARM object references `_ZN11SolarSystem11getStationsEv`, matching
the original call. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_loader_audit_20261010.json`:
**4,323/4,524** original functions across **4,689** rows, **91.87%**
source-shape, **79.93%** strict fuzzy, **2,313 linked-exact** and
**997 byte-exact**. There are 201 absent originals, zero wrong-type misses
and zero skipped units. Compared with the preceding planet-array report,
comparison-row and both exact-pair sets have zero losses or gains.

A synthetic host fixture has since exercised the loader body and its
file-order output; see [the smoke](FILE_READ_STATION_SYNTHETIC_SMOKE_2026-10-10.md).
It does not establish whether ID lists in real assets have the order assumed
by `StarSystem`, nor does it validate binary decoding or whole-game behavior.
Those and the remaining loader instruction differences are the next bounded
priorities.
