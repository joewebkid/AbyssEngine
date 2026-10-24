# Pan first-friend Player setup (2026-10-24)

## ARM source and executed boundary

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
enters `Level::createShip` at `0xc0214`. It obtains three bounded
`nextInt(40000)` draws at `0xc0266–0xc0282`, applies level/campaign HP
rules, allocates and constructs `Player` at `0xc03a0–0xc03c4`, computes
the jittered coordinates, and calls `Player::setEmpData` at `0xc03ee`.
The fighter-class branch begins at `0xc03f6`; its `PlayerFighter`
allocation and constructor are at `0xc06b6–0xc06f8`.

`tools/run_level_stage1_first_friend_player_pan_gate.py` extends the
original-asset Pan mission chain through the source-extracted production
`Level::createShip` prelude, production `Player::Player` and
`Player::setEmpData`. It stops immediately before the `PlayerFighter`
branch. With campaign mission 50, difficulty `1.0f` and non-hardcore
status, the fixture checks the ARM-supported 270-HP campaign override
scaled to 405 HP, radius 1000, level-derived EMP points and duration
15000. A replay of the three new RNG draws checks the generated position
and final RNG state. The replay shares the reconstructed `AERandom`
algorithm and does not independently certify that implementation.

## Validation and limits

Windows SwiftShader runs at `-O0/-O2` with original Pan assets both exit
zero. NDK r18b ARMv7 `-O0/-O2` executables compile and link, and the
native `gof2` build is green. At the initial checkpoint ADB listed no
device; the follow-up below adds hardware execution. The `Player` object exists
in the host gate, but the real `PlayerFighter` constructor, `setShipGroup`,
`setLevel`, enemy-array insertion, remaining population and stage-one
return are still unverified. The gate is not a playable frame.

No production C++ changed at this checkpoint. Its then-current full ARM corpus was
**4,323/4,524** originals across **4,736** pairs with mean
**94.09%/83.41%** source-shape/strict fuzzy, **2,557 linked-exact** and
**1,082 byte-exact** counts, zero skipped and wrong-type symbols; the
test-only addition did not remeasure pair identities. Later corpus metrics
are in `PROJECT_STATUS.md`. Next, execute the
real `PlayerFighter` construction and `Level::createShip` return, then
verify the first array slot and subsequent ordinary ships.

Device follow-up (2026-10-24): the later first-friend Route-clone gate
inherits this production `Player`/EMP setup and passed at `-O0/-O2` as
ELF32 ARM PIE on the connected Android 16 device, using original Android
OBB bytes through the gate's ADB-reverse file service. This still does
not execute the real `PlayerFighter` constructor or complete
`Level::createShip`.
