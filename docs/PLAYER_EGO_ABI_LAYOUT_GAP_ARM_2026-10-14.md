# PlayerEgo ARM32 layout gap (2026-10-14)

## Initial verified contradiction

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

At the initial audit checkpoint, no production source was changed.
At the full-corpus checkpoint (`_work/approach_asteroid_guard_full_20261014.json`),
coverage was **4,323/4,524**, average source-shape **93.10%**,
strict fuzzy **81.52%**, linked-exact **2,403**, byte-exact
**1,008** and extras **123**; the exact-pair sets were unchanged
by the prior mining-phase patch. Those values are not promoted to
runtime confidence by this document.

Next, reconstruct `PlayerEgo` offset groups against raw ARM and
the constructor/allocation, migrate the header in bounded steps,
and compare full exact-pair sets after each step. Only then resume
the undefined stage-zero `PE_aa_approachStep` path. Do not add a
padding guess solely to make the size `0x3a0`.

## First verified offset group repaired (2026-10-14)

Original constructor line 80098 constructs a Matrix at `PlayerEgo+0x40`.
Original `up`/`down` maneuver writes at IDA lines 86202 and 86318
target `+0x7c`; `right` at line 85987 targets `+0x80`. The raw ARM
camera setter/getter at `0xa21f8`/`0xa2202` use `+0x88`. The header
now constructs the separate `+0x40..+0x7b` Matrix and has an
unidentified `+0x84..+0x87` slot, with compile-time ARM32 offset
assertions for the confirmed members. Its size becomes `0x360`.

NDK r18b Clang record layout, native `gof2`, and all 206 ARM
translation units pass. The constructor's focused source-shape
improves from 18.5% to 18.9% but remains non-exact. Focused
`getTargetFollowCamera` is now
100% source-shape, linked-exact and byte-exact. The full corpus
`_work/player_abi_matrix_full_20261014.json` gives **4,323/4,524**
covered, **93.11%** source-shape, **81.54%** strict fuzzy,
**2,404** linked-exact, **1,009** byte-exact and **123** extras.
Comparing exact pairs with the initial checkpoint shows zero losses
and exactly the camera getter as the one new linked/byte exact pair.

This does not close the ABI gap. Remaining Clang offsets include
`shakeIntensity +0x100` versus original `+0x134`,
`asteroidTarget +0x184` versus `+0x1bc`,
`dockingState +0x188` versus `+0x1c0`,
and `hud +0x1dc` versus `+0x220`; allocation remains `0x3a0` in
the original. The existing camera setter calls
`resetShipHandling()`, whereas original ARM calls
`setShipHandling()` with a value loaded from `PlayerEgo+0x154`.
That value's field mapping and gameplay effects are not yet verified.
The next offset group must be established from ARM evidence before
changing later members or this setter.

## Control fields and boost initialization repaired (2026-10-14)

Original `shouldSwitchToStandardCam`/`shouldSwitchToFreeLookCam`
access bytes `+0xb0`/`+0xb1`; `setSpeed`/`getSpeed` access `+0xb8`,
`setThrust`/`getThrust` access `+0xbc`, and free-look accessors use
`+0xc4`. Constructor lines 80248–80257 assign `2.0f` speed and
`1.0f` thrust, derive boost speed at `+0xc8`, and store boost time
and delay at `+0xcc`/`+0xd0`. The header now has the intervening
unidentified storage, overlapping byte flags at `+0xb0..+0xb2`,
and compile-time assertions for the confirmed fields. The
constructor restores those confirmed values and writes handling
to its intended named member instead of overwriting the camera
flags. The actual handling member offset is still unresolved.

NDK r18b Clang records `PlayerEgo` size `0x378`, compared with
original allocation `0x3a0`. Native `gof2`, all 206 ARM units and
37 verifier tests pass (one skipped). Focused verification gives
nine linked- and byte-exact methods in this group; the full corpus
also gains `changeThrust`. The full report
`_work/player_abi_controls_final_full_20261014.json` covers
**4,323/4,524** functions, with **93.13%** source-shape,
**81.64%** strict fuzzy, **2,414** linked-exact, **1,019**
byte-exact and **123** extras. Compared with the prior Matrix
checkpoint, exact-pair sets have zero losses and ten gains.
Constructor source-shape rises from 18.9% to 20.9%, but remains
non-exact.

Later fields still diverge: `shakeIntensity +0x118` versus ARM
`+0x134`, `handling +0x134` versus `+0x154`,
`asteroidTarget +0x19c` versus `+0x1bc`, and `hud +0x1f4`
versus `+0x220`.
The camera setter still calls `resetShipHandling()` instead of
passing the original `+0x154` value to `setShipHandling()`.
Runtime ABI, the full constructor and gameplay semantics remain
unverified. Next, map the intervening fields from ARM before
moving handling and docking members.
