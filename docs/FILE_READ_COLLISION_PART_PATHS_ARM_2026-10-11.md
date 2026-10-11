# Collision and part resource paths (2026-10-11)

Android 2.0.16 ARM names these resources at both `FileExist` and
`OpenRead` sites:

| Loader | ARM address | Resource |
| --- | --- | --- |
| `loadWreckCollision` | `0x11fd78` | `data/bin/wreck_collisions.bin` |
| `loadStationCollision` | `0x11fc5c` | `data/bin/collision.bin` |
| `loadStaticCollision` | `0x11fe94` | `data/bin/static_collisions.bin` |
| `loadStationParts` | `0x11ffb0` | `data/bin/stationparts.bin` |
| `loadShipParts` | `0x11fad0` | `data/bin/shipparts.bin` |

The recovered functions used five different relative filenames. The
ten path literals now match the ARM calls. The collision loaders have
callers in `Level`, `PlayerStation` and `Globals`; no direct call to
the two part loaders was found in the current recovered source. The
parsing loops, ownership, and failure paths were not changed.

Native `gof2`, 206/206 ARM translation units, and 36 verifier tests
(one skipped) pass. Focused comparison of the five loaders remains
86.0% average source-shape and 45.6% strict fuzzy, with no linked- or
byte-exact match. The path correction affects resource lookup, not the
normalized instruction shape.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_space_points_full_20261011.json`, no
exact pair was lost; there are zero skipped rows and zero wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_collision_parts_full_20261011.json`.

Unverified: resource-backed parsing and runtime behavior. None of the
five original binary files is in the local extracted-resource set;
the part-loader record formats and their no-match paths require a
separate audit before any semantic changes.
