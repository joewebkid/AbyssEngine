# Level jumpgate landmarks in createSpace (2026-10-17)

Android 2.0.16 `libgof2hdaa.so`, `Level::createSpace` at `0xae5b8`,
creates jumpgates in landmark slots 1 and 2 after the station branch.
Slot 1 is conditional on `currentOrbitHasWarpGate`; slot 2 is always
created and has its collision radius zeroed. The source now follows the
observed station-seeded angle generation and Y-axis placement, race-selected
meshes from rodata `0x1fdc54`, two child transforms, the jump animation
transform, LOD meshes from `0x1fb7f8`, and the race-1 child LOD pair.
The RNG is reset after the two slots. The `PlayerJumpgate` constructor and
animation methods now use the initialized `Status::gStatus` and
`PaintCanvas::gCanvas` pointers rather than file-local null placeholders;
startup assigns these alongside `Globals::status` and `Globals::Canvas`.

The source-extracted jumpgate branch passes `-O0` and `-O2` host smokes for
normal, no-warp, alien, and no-station paths. The preceding station smoke
also passes. These smokes use test doubles and do not establish full scene
initialization. Native `gof2`, 208/208 ARM translation units, and 40 verifier
tests pass (one existing skip). Focused `createSpace` source-shape rises
from 32.0% to 55.0%; strict fuzzy changes from 15.2% to 11.3%, with no
exact match. Focused `PlayerJumpgate` constructor source-shape is 97.8%
for both symbols, strict fuzzy 86.0%; neither is exact.

Evidence and reproducibility: original pseudocode addresses
`0xaec4c`–`0xaefc0`; original ARM rodata `0x1fdc54` and `0x1fb7f8`;
`tools/run_level_jumpgate_landmark_smoke.py`;
`A:/Galaxy on Fire new version/_work/level_create_space_before_20261017.json`,
`level_create_space_after_20261017.json`, and
`player_jumpgate_ctor_after_20261017.json` in the same directory.

The full ARM report at
`A:/Galaxy on Fire new version/_work/level_jumpgate_full_20261017.json`
compares 4,323/4,524 original functions in 4,730 rows, with 201 missing,
zero skipped or wrong-type. Mean source-shape rises 93.83% → 93.84% and
strict fuzzy 82.99% → 83.02%. Linked-exact stays 2,528 and byte-exact
stays 1,076; pairwise comparison against
`level_create_station_full_20261017.json` found zero exact-pair losses.

The alien-orbit gate relocation/direction, slot-3 wormhole, final
`field_138` adjustment, and remaining skybox behavior are still absent.
The next bounded stage is to restore these in source-backed slices and then
exercise a real `Level::init` path with game resources.
