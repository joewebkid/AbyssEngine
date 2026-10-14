# PlayerEgo move-to-position recovery (2026-10-14)

Android Thumb `PlayerEgo::moveToPosition(Vector, bool, float)` at
`0x9c6d4–0x9ca52` is the primary source, cross-checked against the
IDA and Ghidra dumps. The old C++ called a declared-only steering shim,
did not update its destination/current-direction vectors, and treated
the transformed strafe vector as a world-space X-only displacement.

The ARM path stores the target at PlayerEgo `+0xec`, subtracts the
ship position, normalizes it, blends it with the current normalized
direction using truncated frame time × requested speed divided by
4,096, and stores the normalized result at `+0x164`. Optional steering
computes the signed right-side angle, maintains a five-slot angle
history at `+0x290`, and returns a turn value clamped to the limit at
`+0x284` with scale at `+0x288`. Thumb explicitly moves this value
into the soft-float return register. The ship then faces the heading,
moves forward, rolls, applies any rotated strafe displacement, and
updates the Player matrix and position cache.

The constructor at `0x998cc` initializes the limit/scale, clears the
five-slot history, and resets its index/full flag at `+0x2a4/+0x2a8`.
The old `setAutoPilot(nullptr)` source reset `+0x284` instead of the
index; Thumb `0x9b808` confirms the correct offsets. The host smoke
extracts both production methods and checks steering branches/history,
forward and strafe motion, NaN/low-strafe behavior, camera translation,
Player/position updates and autopilot reset without erasing the limit.
Native `gof2`, 206/206 ARM translation units and 37 verifier tests
(one skipped) pass. Focused `moveToPosition` source-shape/strict fuzzy
rises **43.8%/22.2% → 92.9%/48.0%**; it is not exact. The constructor
improves **41.9%/25.2% → 44.7%/28.2%** and `setAutoPilot`
**99.4%/91.7% → 100.0%/91.7%**; neither is exact.

The full ARM report `_work/player_move_to_position_full_20261014.json`
compares **4,323/4,524** original functions at **93.34%** average
source-shape and **82.24%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, **201 missing** and zero skipped.
Both exact-pair sets have zero losses and zero gains against
`_work/player_handle_ship_full_20261014.json`.

The fixture uses simplified geometry and cannot prove live flight
behavior or ARM32 runtime. Remaining disassembly differences include
temporary/vector-copy ordering, register allocation, and call
scheduling; do not force those at the cost of semantics. Next priority:
the adjacent `updateManeuver` path, which still contains declared-only
movement helpers.
