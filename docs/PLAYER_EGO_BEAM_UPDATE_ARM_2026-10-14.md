# PlayerEgo beam updates (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::update` in the 2.0.16
`libgof2hdaa.so`: at `0x9d8fe–0x9d93c` it tests the tractor-beam
pointer at `+0x1b4`, calls `TractorBeam::update`, then tests the
repair-beam array at `+0x1b8` and calls `RepairBeam::update` for
each entry. It passes the stored frame time at `+0x134`, the
current radar, level at `+0xc`, and HUD. IDA lines 83966–83981
independently show the same dispatch and order. The existing
`setShip` constructed these beams and `render` drew them, but the
recovered `update` omitted both update calls.

The extracted production-fragment smoke covers no beams, tractor
only, repair array only, both, and an empty repair array. It checks
call order and all four arguments. Native `gof2`, 206/206 ARM
translation units, and 37 verifier tests (one skipped) pass.

Focused `update` source-shape rises **66.1% → 67.5%** while strict
fuzzy falls **28.6% → 20.5%**; the original has 2,913 instructions
and the generated body grows from 1,653 to 1,683. This is a
behavioral recovery, not an exact match or a whole-method claim.
Full report `_work/player_update_beams_full_20261014.json` compares
**4,323/4,524** original functions, with **201 missing**, zero
skipped and **123 extras**. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. The **2,482 linked-exact**
and **1,065 byte-exact** pair sets lose and gain zero members against
`_work/player_set_ship_full_20261014.json`.

The original `PlayerEgo` destructor releases the repair-beam array
and its elements but has no release of emergency geometry at
`+0xac`. Ownership of that geometry is unresolved, so this package
does not invent a deletion. The smoke does not execute the whole
`PlayerEgo::update` or live ARM32 beam effects. Next priority:
recover the adjacent cloak/material and transform update blocks
around ARM `0x9d940+`, then validate their branches independently.
