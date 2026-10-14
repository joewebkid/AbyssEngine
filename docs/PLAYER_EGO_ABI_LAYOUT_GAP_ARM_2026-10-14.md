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

## Frame, position and docking offsets repaired (2026-10-14)

At `0x9ce86–0x9ce96`, original ARM `update` passes the geometry
position to `Vector::operator=` at `PlayerEgo+0x148`, then stores
frame time at `+0x134`. The prior C++ dropped `getPosition()`'s
return value and copied uninitialized stack floats into two Vector
components and a separate integer. It now copies the returned Vector
and assigns the frame time. The constructor also restores the
original `Player::getCombinedHP` write at `+0x130` (IDA line
80247). The header adds only the unresolved intervening storage
needed to put confirmed members at their ARM offsets and removes
the spurious integer between the position Vector and handling.

Clang 7 ARM32 record layout now confirms `lastHP +0x130`,
`shakeIntensity +0x134`, `boostTimer +0x138`, `boostingFlag +0x13c`,
`collide +0x144`, `field_0x145 +0x145`, `dockOffsetVec +0x148`,
`handling +0x154`, `autoPilotTarget +0x15c`,
`asteroidTarget +0x1bc`, and `dockingState +0x1c0`. These map to original
constructor, raw ARM update, boost/accessor, and docking evidence.

Native `gof2`, all 206 ARM units, 37 verifier tests (one skipped)
and the three existing PlayerEgo smokes pass. Full corpus
`_work/player_abi_frame_full_20261014.json` covers **4,323/4,524**
functions, with **93.17%** source-shape, **81.85%** strict fuzzy,
**2,439** linked-exact, **1,038** byte-exact and **123** extras.
Compared with the prior control-field checkpoint, exact-pair sets
have zero losses, 25 linked gains and 19 byte gains. Constructor
source-shape improves from 20.9% to 21.9%; `update` improves from
12.8% to 13.4%. Neither is exact.

The class still measures `0x398` versus original allocation `0x3a0`.
The later `miningGame`/HUD groups and the camera setter are not yet
aligned; no linked gameplay or ARM32 runtime test has been run.
These remain the next ABI priority before claiming behavior parity.

## Mining and hacking offsets repaired (2026-10-14)

Original `dockToAsteroid` writes its scaling value to DWORD 118
(`+0x1d8`) and resets DWORD 119 (`+0x1dc`) in IDA lines
85200–85206. `MiningGame` is read at DWORD 121 (`+0x1e4`) and
`HackingGame` at DWORD 122 (`+0x1e8`). Current ARM32 layout
needed a four-byte slot after the three-word approach Vector at
`+0x1c8..+0x1d3`. Adding only that unidentified `+0x1d4` slot
aligns `dockScaling`, `miningSettleTimer`, `dockStation`,
`miningGame`, and `hackingGame`; compile-time assertions guard
the confirmed offsets.

Native `gof2`, all 206 ARM units, 37 verifier tests (one skipped)
and three PlayerEgo smokes pass. The full corpus report
`_work/player_abi_mining_full_20261014.json` covers **4,323/4,524**
original functions, with **93.20%** source-shape, **81.95%** strict
fuzzy, **2,458** linked-exact, **1,050** byte-exact and **123**
extras. Relative to the preceding frame-offset report, exact
pairs have zero losses and gain 19 linked/12 byte matches.
`dockToAsteroid` rises from 98.2% to 99.4% source-shape, still
without an exact match.

The ARM32 record size is now `0x3a0`, equal to the original
allocation, but this is insufficient to establish ABI parity.
The header's `hud` remains `+0x218` against original `+0x220`;
later offsets can compensate for this within the same total size.
The camera setter's handling call and full linked/ARM gameplay
remain unverified. Next map the HUD and terminal groups against
raw ARM before claiming a completed class layout.

## Camera-handling route repaired (2026-10-14)

`Ship::getHandling()` is declared and implemented as a float. The
previous constructor assigned it to `PlayerEgo::handling`, which
was incorrectly typed `int`; this numerically truncated the
handling value. The original ARM setter at `0xa21f2–0xa21fe`
loads the raw float at `PlayerEgo+0x154`, stores the camera at
`+0x88`, and tail-branches to
`TargetFollowCamera::setShipHandling(float)`. C++ now stores
handling as float and calls that setter rather than
`resetShipHandling()`. The getter's return type is also float;
its existing ARM byte-exact match is preserved.

Focused setter source-shape improves from 82% to 100%, with a new
linked-exact match; byte-exact is not achieved. Native
`gof2`, all 206 ARM units, 37 verifier tests (one skipped) and
three PlayerEgo smokes pass. Full report
`_work/player_abi_handling_full_20261014.json` covers
**4,323/4,524** original functions at **93.20%** source-shape,
**81.97%** strict fuzzy, **2,459** linked-exact, **1,050**
byte-exact and **123** extras. Compared with the previous mining
checkpoint, exact-pair sets have zero losses and one linked gain.

HUD remains `+0x218` versus original `+0x220` despite the equal
overall `0x3a0` class size. Later offsets and full linked ARM32
runtime behavior are not yet verified; they are the next priority.

## HUD and turret aliases repaired (2026-10-14)

Original `getHUD` at IDA line 87490 reads DWORD 136 (`+0x220`),
while the former ARM32 header placed `hud` at `+0x218`. The
intervening eight bytes remain unidentified and are now reserved
without assigning guessed semantics. Original `checkForTurret` at
IDA lines 82047 and 82067 reads the byte at `+0x1a0` and pointer
at `+0x1b0`; `approachDockingPoint` at line 85693 also reads the
byte at `+0x1a0`. These are the existing `turretActive` and `cloak`
members. The late duplicate members were removed, and their three
uses now refer to the confirmed early members. ARM32 offset
assertions cover all three addresses; the record size remains
`0x3a0`.

Native `gof2`, all 206 ARM translation units, 37 verifier tests
(one skipped), and three PlayerEgo smokes pass. Full corpus
`_work/player_abi_hud_full_20261014.json` covers **4,323/4,524**
original functions at **93.20%** average source-shape and **81.98%**
strict fuzzy, with **2,461 linked-exact**, **1,051 byte-exact** and
**123 extras**. Relative to the camera-handling checkpoint, exact
pair sets lose none and gain two linked matches (`getHUD` and
`setTurretPosition`) and one byte match (`getHUD`). `getHUD` is now
100% source-shape and byte-exact. The two larger turret/docking
functions remain non-exact.

This does not prove full `PlayerEgo` ABI or runtime behavior. The
current roll Matrix is at `+0x27c` versus original construction at
`+0x2ac`; `rotateX` is at `+0x2b8` versus original `+0x2e8`, and
the explosion timer is at `+0x2c8` versus original `+0x2f8`.
The trailing Matrix in the C++ has no confirmed original counterpart.
Next, map the intervening movement/rotation fields from ARM and
remove or relocate members only when their addresses are supported.

## Late movement, visibility and terminal offsets repaired (2026-10-14)

The original `resetMovement` at IDA lines 81026–81035 zeros
`+0x268..+0x27f`. `roll` at lines 82414–82503 uses a direction
byte at `+0x2a9`, a Matrix at `+0x2ac`, a rolling flag at
`+0x2f4` and auto-level at `+0x324`; the constructor calls the
Matrix constructor at `+0x2ac` (line 80119). Original `rotate`
and `setRotation` use angles at `+0x2e8..+0x2f0`, and
`explosionEnded` reads its timer at `+0x2f8`. The C++ now reserves
the intervening unidentified ranges at `+0x254..+0x267` and
`+0x288..+0x2a7`, placing these members at the confirmed offsets.
The trailing C++ Matrix was removed: the original constructor has
no corresponding third Matrix construction.

Original `setVisible` (IDA line 87263) writes `+0x309` and
`+0x32e`, then calls `setExhaustVisible`, which writes `+0x32f`
and updates particle emission. The C++ had conflated the first
flag with the separate render-visibility byte at `+0x308` and
omitted the exhaust call. Both flags and the call are now
restored. Original accessors/constructor also support dock totals
at `+0x360/+0x364`, material-list pointer at `+0x394`, and
volatile/mining flags at `+0x398/+0x399`; bounded unidentified
storage aligns them. ARM32 assertions cover the named anchors and
the class remains `0x3a0` bytes.

Native `gof2`, all 206 ARM translation units, 37 verifier tests
(one skipped), and three PlayerEgo smokes pass. Full corpus
`_work/player_abi_late_final_full_20261014.json` covers
**4,323/4,524** original functions at **93.23%** source-shape
and **82.09%** strict fuzzy, with **2,472 linked-exact**,
**1,060 byte-exact** and **123 extras**. Against the preceding
HUD checkpoint, exact-pair sets lose none and gain 11 linked and
9 byte matches. `setVisible` source-shape rises from 53.3% to
100%, but is not exact. Constructor source-shape falls from 21.7%
to 19.6% after removing the unsupported third Matrix; its full
initialization is still not reconstructed.

The placeholder ranges have no assigned semantics, and the
existing host smokes do not exercise `setVisible` or linked ARM32
gameplay. Next, inspect the constructor's large initialization
gaps and the interaction of render/requested/exhaust visibility
in original update paths before claiming full class behavior.

## HUD, cargo and handling constructor group (2026-10-14)

Original constructor lines 80339–80349 save the ship's agility at
`+0x254`, scale handling at `+0x154` by
`(1 + agility / 100) * 20`, and set the hard-core flag at `+0x235`.
Lines 80415–80439 initialize the volatile-goods flag at `+0x398`,
the previous free-space value at `+0x250`, and five 2D image
handles at `+0x23c..+0x248` and `+0x238`. Original `draw` uses
the four crosshair handles at `+0x23c..+0x248` and the blink
timer at `+0x24c` (IDA lines 87313–87347); `drawThrottle`
uses the fifth handle at `+0x238` (lines 87390–87410).
The former C++ overlaid these members with pitch/yaw ramps and
omitted image creation. The header now has distinct members,
with pitch/yaw ramps at the ARM-confirmed `+0x258/+0x25c`.
The constructor initializes the confirmed gameplay fields and
creates the images in the original resource-ID order. `draw`
selects each matching image and updates its separate timer.

Native `gof2`, all 206 ARM translation units, 37 verifier tests
(one skipped), and three PlayerEgo smokes pass. Full corpus
`_work/player_hud_ctor_full_20261014.json` covers
**4,323/4,524** original functions at **93.24%** average
source-shape and **82.11%** strict fuzzy, with **2,473
linked-exact**, **1,061 byte-exact** and **123 extras**.
Compared with the late-layout checkpoint, exact-pair sets lose
none and gain the linked/byte-exact `collidesWithStation` pair.
Constructor source-shape rises from 19.6% to 29.0%; it remains
non-exact and far from fully reconstructed.

The saved free-space/volatile-goods update during gameplay is
still absent from the C++ `update` path, and the current
`checkForTurret` position argument aliases the throttle image
handle as a Vector. Neither has been validated in linked ARM32
gameplay. These are the next semantic priorities; do not infer
full constructor or HUD correctness from the higher score.
