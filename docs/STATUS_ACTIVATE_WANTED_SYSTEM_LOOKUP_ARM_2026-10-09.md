# Wanted-system array lookup (2026-10-09)

`Status::activateNewWanted` used to cast the station's numeric system index to
`SolarSystem *` before calling `getRoutes()`. That is invalid on both 32-bit
ARM and 64-bit native hosts. The `systems` array is already loaded by
`FileRead::loadSystemsBinary()` in this function.

The Android 2.0.16 ARM function at `0x000acedc` loads the system pointer from
the array before each `getRoutes()` call. At `0x000ad078`-`0x000ad080`, it
loads the array data pointer and indexes it by the first station's system
number (`r4`). At `0x000ad0fe`-`0x000ad106`, it performs the same lookup for
the second station's number (`r8`). Both source sites now use
`(*systems)[index]->getRoutes()`. The obsolete index-to-pointer helper is
removed. The rebuilt ARM object also contains both indexed loads before the
calls, confirming the recovered dataflow.

Validation: native `gof2` builds, 206/206 ARM translation units compile, 36
verifier tests run (one optional skip), and all four scope tests pass. The
full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_activate_wanted_20261009.json`:
**4,323/4,524** original functions compared across **4,689** rows,
**91.87%** corpus source-shape, **79.93%** strict fuzzy,
**2,313 linked-exact** and **997 byte-exact**. There are 201 absent originals,
zero wrong-type misses and zero skipped units. Compared with the preceding
system-pointer report, the `(unit, symbol)` comparison-row, linked-exact and
byte-exact sets have zero losses and zero gains.

This is a semantic repair, not a metric gain: this function's source-shape
falls from **88.7% to 88.4%**, and strict fuzzy from **42.1% to 39.2%**.
The array lookup is nevertheless directly supported by the original ARM
instructions. Other branches and cleanup paths in this long function, and
native whole-game behavior, have not been validated end to end.
