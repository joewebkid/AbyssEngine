# Level station landmark in createSpace (2026-10-17)

Android 2.0.16 `libgof2hdaa.so` `Level::createSpace` at `0xae5b8`
allocates a four-entry landmark array, then creates a `PlayerStation` in
slot 0 unless the orbit is empty. A non-alien empty orbit still gets the
station for indices 27, 110 and 111 (`0xaebcc`–`0xaebee`). The station's
root geometry is registered with `LODManager` (`0xaec00`). After DLC 1 is
won, the object's name comes from a separately loaded station 101, which is
then destroyed (`0xaec08`–`0xaec46`). The previous source stopped immediately
after allocating the array.

The source-extracted branch passes an `-O0/-O2` host smoke for ordinary,
empty, exceptional, alien and DLC-renaming paths. It uses test doubles for
the `PlayerStation` constructor, LOD and asset lookup; no full `createSpace`
runtime claim follows from it. Native `gof2`, 208/208 ARM TUs and 40 verifier
tests pass (one existing skip). Focused `createSpace` source-shape / strict
fuzzy rises from 22.2% / 11.6% to 32.0% / 15.2%, still nonexact.
Reports: `A:/Galaxy on Fire new version/_work/level_create_space_before_20261017.json`
and `A:/Galaxy on Fire new version/_work/level_create_station_after_20261017.json`.

The function still lacks the original jumpgate and wormhole construction,
their resource setup and several skybox operations. Consequently
`landmarks[2]` is still absent and special stream-out placement in
`Level::init` remains unsafe. Recovering jumpgate creation and its constructor
dependencies is the next priority.

The full ARM report at
`A:/Galaxy on Fire new version/_work/level_create_station_full_20261017.json`
compares 4,323/4,524 originals in 4,730 rows. Mean source-shape / strict
fuzzy remains 93.83% / 82.99% after rounding; 2,528 linked-exact and
1,076 byte-exact are unchanged. Pairwise comparison with
`level_entry_position_full_20261017.json` found zero exact-pair losses or
gains. Missing remains 201, skipped and wrong-type zero.
