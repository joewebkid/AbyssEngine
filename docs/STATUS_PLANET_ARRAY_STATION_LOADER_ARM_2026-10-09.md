# Status planet arrays and station loader calls (2026-10-09)

The Android 2.0.16 ARM `Status::getPlanetNames` at `0x000ac5d4` returns the
pointer at `Status + 0x1a8`; `getPlanetTextures` at `0x000ac5da` returns the
pointer at `Status + 0x1ac`. These fields hold `Array<String *> *` and
`Array<int> *` respectively, not 32-bit integer representations of host
pointers. The fields, getters, `setStation` ownership path and direct Radar
consumers now use those types. Both getters remain 2/2-instruction
linked-exact and byte-exact. ARM32 offset checks and a native compile-time
pointer-contract check cover the declarations.

The original `Status::setStation` calls
`FileRead::loadStationsBinary(SolarSystem *)` at `0x000a910c`, passing the
current system in `r1`. `StarSystem`'s constructor makes the same call at
`0x00133e18`. Both recovered callers previously invoked the unrelated
no-argument loader, which is currently a zero-return stub. Both now invoke
the already implemented system-specific overload; their rebuilt ARM objects
refer to the matching `_ZN8FileRead18loadStationsBinaryEP11SolarSystem`
symbol. `Status::setStation` also reads the system's `Array<int>` station IDs
as integers instead of casting them to `Station *`.

Validation: native `gof2` builds, 206/206 ARM translation units compile,
the pointer-contract smoke compiles, 36 verifier tests run (one optional
skip), and four scope tests pass. Focused `Status::setStation` source-shape
rises **86.5% to 86.9%**, strict fuzzy **41.8% to 47.7%**. `StarSystem`'s
constructor rises **78.5% to 78.9%** source-shape while strict fuzzy falls
**18.4% to 18.2%**; `Radar::drawCurrentLock` stays **98.2% / 93.7%**.

The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_planet_arrays_20261009.json`:
**4,323/4,524** original functions across **4,689** rows, **91.87%**
source-shape, **79.93%** strict fuzzy, **2,313 linked-exact** and
**997 byte-exact**. There are 201 absent originals, zero wrong-type misses
and zero skipped units. Comparison-row and both exact-pair sets have zero
losses or gains against the preceding standing-pointer report.

The system-specific loader itself is **94.0%** source-shape, not exact. No
host fixture or whole-game station-to-Radar run was performed. Its station
ordering, ownership and missing-file behavior have since been audited
against the original ARM in [the loader follow-up](FILE_READ_STATION_ORDER_ARM_2026-10-10.md).
The representative lawful-data host fixture remains to be done.
