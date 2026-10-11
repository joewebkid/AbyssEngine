# Core binary resource paths (2026-10-11)

Five Android 2.0.16 ARM loaders probe and open `data/bin/*.bin`:

| Loader | ARM address | Resource |
| --- | --- | --- |
| `loadSystemsBinary` | `0x1207e0` | `data/bin/systems.bin` |
| `loadWanted` | `0x120490` | `data/bin/wanted.bin` |
| `loadAgents` | `0x1202b8` | `data/bin/agents.bin` |
| `loadItemsBinary` | `0x120afc` | `data/bin/items.bin` |
| `loadShipsBinary` | `0x120ca8` | `data/bin/ships.bin` |

Each recovered loader had used a different `*_binary.bin` filename in
both `FileExist` and `OpenRead`. The ten literals now match the original
calls. All five corresponding resources are present in the local evidence
set; none is imported into this repository.

Native `gof2`, all 206 ARM translation units, and the 36 verifier tests
(one skipped) pass. Focused ARM comparison: 92.7% average source-shape,
59.1% strict fuzzy, zero linked- or byte-exact across these five
functions. These scores are unchanged from the preceding snapshot: the
correction addresses asset lookup rather than instruction shape.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_stations_path_full_20261011.json`, no
exact pair was lost; there are zero skipped rows and zero wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_core_loaders_full_20261011.json`.

Unverified: actual game-runtime asset mounts, each loader's complete
record semantics, and downstream behavior when a resource is absent.
