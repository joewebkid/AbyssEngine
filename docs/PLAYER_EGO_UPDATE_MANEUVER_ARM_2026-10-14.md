# PlayerEgo maneuver update recovery (2026-10-14)

Android Thumb `PlayerEgo::updateManeuver()` at `0x9c1f8–0x9c684` is
the primary source, cross-checked against the IDA and Ghidra dumps.
The old body relied on three declared-only helpers and incremented
the maneuver timer at `+0x350` with an unrelated pointer instead of
the frame time at `+0x134`. `initManeuver` uses the same timer field.

For types 1 and 2, the recovered path moves along the ship's right
vector with a 1,200 ms fade, reverses direction for type 2, moves
forward, and rotates the ship about Y. It writes the calculated sway
at `+0x27c`, updates the camera target offset and no-update
translation, composes the Player transform, and resets the camera
when the timer reaches 1,200 ms. For type 3, it first aims at the
ship position plus forward/up vectors scaled by 10,000; after 900 ms
it aims at the saved target. It calls `moveToPosition` with speed
`7.0f`, rotates about Z, translates the camera along the stored
facing vector, and restores the camera's normal mode at 3,000 ms.

The production-body host smoke covers inactive, both dodge
directions, type-3 target switch, movement/camera effects, and both
completion thresholds and the `+0x27c` sway write. The existing
`initManeuver` smoke also checks the renamed timer. Native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped)
pass. Focused `updateManeuver` source-shape/strict fuzzy changes
**30.1%/20.0% → 93.0%/51.7%**; `initManeuver` remains at
**99.4%/89.4%**. Neither function is exact.

Full report `_work/player_update_maneuver_full_20261014.json` covers
**4,323/4,524** original functions, with **93.35%** average
source-shape, **82.25%** strict fuzzy, **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, **201 missing** and zero skipped.
The linked- and byte-exact pair sets have no losses or gains against
`_work/player_move_to_position_full_20261014.json`.

The host fixture uses simplified matrices and does not prove live
flight animation or ARM32 runtime. Next priority: the adjacent
`dockToDockingPoint` ease-matrix helper still declared without a body.
