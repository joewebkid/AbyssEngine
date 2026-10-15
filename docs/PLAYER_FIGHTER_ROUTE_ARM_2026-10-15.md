# PlayerFighter constructor route recovery — 2026-10-15

## Android evidence

The reference is Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
REA/IDA disassembly of `PlayerFighter::PlayerFighter` at `0xdc2f8`
shows twelve bounded `AERandom::nextInt` calls for four candidate
waypoints, `nextInt(3) * 3 + 6` route coordinates, and a retrying
`nextInt(4)` choice without duplicate waypoint indices (`0xdc374–0xdc55a`).
The random source is `Globals::rnd`, not the separate `AERandom::gRandom`.
The constructor allocates a coordinate array, constructs a copying
`Route`, then deletes the temporary array (`0xdc55c–0xdc572`).

At `0xdc576–0xdc5a0`, the constructor creates the shared
`stationRouteAliens` only when null. Its twelve coordinates are copied
from ARM `.rodata` at `0x1fe100`; the ELF load segment maps this virtual
address to the same file offset. Read-only bytes decode to four points:
`(40000,0,40000)`, `(40000,0,-40000)`, `(-40000,0,-40000)`,
`(-40000,0,40000)`. Both routes receive `setLoop(true)` at
`0xdc654–0xdc65e`. At `0xdc622–0xdc632`, the object's working position
receives the constructor's x/y/z arguments.

## Correction and checks

The prior constructor used an unbounded RNG for array indices, a null
shared-route pointer, and a one-word placeholder as the source of a
48-byte copy. It also disabled route looping and wrote the bool argument
as a float position. This package restores the bounded draws, four-byte
used-index array, real static route, correct position and looping.
It removes source-level stack-canary and Matrix-constructor placeholders:
the stack guard belongs to the compiler, and the two Matrix members
already run their C++ default constructors before the body.

`python tools/run_player_fighter_route_smoke.py` compiles the production
route-setup statements with recording RNG and Route dependencies. It
checks 2/3/4-point results, a repeated candidate index, the complete
draw sequence and bounds, shared-route reuse, station coordinates,
looping and the x/y/z position. The pre-existing `setLevel` smoke also
passes. Native `gof2`, 206/206 ARM translation units and 37 verifier
tests (one skip) pass. The constructor's focused source-shape is
**94.5%** (previously **85.2%**) and strict fuzzy is **37.9%**;
it is not exact. Full corpus: **4,323/4,524** original functions compared,
**201 missing**, **123 extras**, zero skipped, averages **93.52%**
source-shape and **82.55%** strict fuzzy. Exact sets are unchanged by
`(unit, symbol)` comparison: **2,499 linked-exact**, **1,074 byte-exact**,
zero losses. Local reports are
`_work/player_fighter_route_focused_20261015.json` and
`_work/player_fighter_route_full_20261015.json` in the parent workspace.

## Limits and next priority

The smoke runs an extracted production fragment with mocks; it does
not execute the original Android binary or the full Fighter constructor.
Other constructor effects, ownership and dependent class behavior remain
to be checked before treating `Level::createShip` as runtime-ready.
The next priority is an ARM32 object-lifecycle test and recovery of the
remaining constructor dependencies without sacrificing existing exact
matches.
