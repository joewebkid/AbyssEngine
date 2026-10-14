# PlayerEgo ARM32 layout gap (2026-10-14)

## Verified contradiction

The Android 2.0.16 binary allocates `PlayerEgo` with `operator
new(0x3a0)` in the game setup path. Both independent exports show
it: `analysis/gof2_libgof2hdaa_full_ida.c` line 102320 and
`analysis/gof2_gidra_engine_full_source.c` line 86439.

Using the repository's existing NDK r18b Clang 7 ARM flags from
`_work/verify_integrated_20261013/.flags`, a record-layout dump of
`src/game/ship/PlayerEgo.cpp` reports `PlayerEgo` size `800`
(`0x320`). The dump is reproducible with the local NDK `clang++.exe`
and `-Xclang -fdump-record-layouts -fsyntax-only` plus those flags.
Selected offsets, in hexadecimal:

| Field | Current ARM C++ | Original Android ARM | Evidence |
| --- | ---: | ---: | --- |
| `targetFollowCamera` | `0x48` | `0x88` | setter/getter instructions at `0xa21f8`, `0xa2202` |
| `shakeIntensity` | `0xc0` | `0x134` | `approachAsteroid` mining-update argument at `0x9f718` |
| `field_0x145` | `0xca` | `0x145` | `update` store at `0x9dd24` |
| `asteroidTarget` | `0x144` | `0x1bc` | `update` load at `0x9dd1e` |
| `dockingState` | `0x148` | `0x1c0` | `update` load at `0x9dd18` |
| `dockingPointIndex` | `0x14c` | `0x1c4` | `approachAsteroid` load at `0x9f59e` |
| `dockScaling` | `0x15c` | `0x1d8` | `dockToAsteroid` entry in IDA lines 85200–85207 |
| `miningGame` | `0x168` | `0x1e4` | `approachAsteroid` load at `0x9f70e` |
| `hud` | `0x19c` | `0x220` | `update` load at `0x9ce5a` |

The earliest named member offsets (`player`, `field_0x14`, `freeze`)
do line up, but the gap varies from `0x40` near the camera to
`0x84` at HUD. This cannot be repaired by one padding insertion.
The existing `PlayerEgo.h` assertion that the camera member is at
`0x48` confirms only the current source layout; it contradicts the
original ARM accessor at `0x88`.

## Impact and next work

Recent docking/mining changes recover several original control and
service routes. Their host smokes compile production method bodies
against minimal doubles, so they do not test the real `PlayerEgo`
memory layout. The native `gof2` target is a static library, and
compilation of all 206 ARM translation units also does not prove
correct field addresses or a runnable linked game. Source-shape
and exact counts remain measurements of compiled code, not an ABI
validation of this class.

No production source was changed in this audit. At the latest
full-corpus checkpoint (`_work/approach_asteroid_guard_full_20261014.json`),
coverage is **4,323/4,524**, average source-shape **93.10%**,
strict fuzzy **81.52%**, linked-exact **2,403**, byte-exact
**1,008** and extras **123**; the exact-pair sets were unchanged
by the prior mining-phase patch. Those values are not promoted to
runtime confidence by this document.

Next, reconstruct `PlayerEgo` offset groups against raw ARM and
the constructor/allocation, migrate the header in bounded steps,
and compare full exact-pair sets after each step. Only then resume
the undefined stage-zero `PE_aa_approachStep` path. Do not add a
padding guess solely to make the size `0x3a0`.
