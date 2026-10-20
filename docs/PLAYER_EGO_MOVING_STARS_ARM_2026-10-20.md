# PlayerEgo moving-stars slot and steering dispatch — 2026-10-20

The Android 2.0.16 ARM binary (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
creates a 0x1c-byte `MovingStars` object at `0x99ae2`–`0x99aee`, stores its
pointer at `PlayerEgo+0xf8` at `0x99af4`, and only then calls
`Player::getCombinedHP`. The previous C++ used an undefined synthetic
`MovingStars_ctor` and stored the result in `dockCameraNode` instead. That
camera field is at `+0x178`; the original constructor clears it at `0x999c4`.
The original `PlayerEgo` destructor does not release the `+0xf8` object, so
this correction does not invent a new ownership rule.

`down/up` write signed direction values at `+0x100` (`0xa0cc0`, `0xa0f2e`);
`right/left` write them at `+0x104` (`0xa07fe`, `0xa0a46`). The previous
header incorrectly placed direction fields at `+0xf4/+0xf8`; `+0xf4` is the
third waypoint coordinate. ARM `turnVertical` and `turnHorizontal` dispatch
to those movement functions, rather than modifying local float accumulators.
The header now asserts the recovered ARM slots and the source follows these
calls. REA pseudocode was used as a cross-check; the offsets and branches
above were also checked in the original instructions.

Verification: native `gof2` build, 208/208 ARM translation units, the
Player-allocation/layout smoke, and 91 verifier tests pass (one existing
skip). The focused `PlayerEgo` report contains 173 comparisons with 99
linked-exact and 61 byte-exact, unchanged from the preceding full report.
Both steering-dispatch functions now score 100% source-shape and strict
fuzzy, up from 53.0%/34.8% and 72.7%/66.7%; neither is byte- or
linked-exact. The constructor scores 78.5% source-shape, up from 77.8%,
and still has no exact match. The final full corpus compares 4,323/4,524
originals in 4,735 pairs at 93.95% mean source-shape and 83.24% strict
fuzzy. Its 2,551 linked-exact and 1,079 byte-exact `(unit, symbol)` sets
have no losses or gains relative to the preceding report; ARM skipped
units remain zero. The report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/player-ego-slot-final-full.json`
(SHA-256 `15bdcdc9a36710df8d91717f08a6019783d92baa949857ff3b84b556db8de5c3`).

This verifies ABI slots and static source-shape, not a full `PlayerEgo`
runtime. The original-asset Level gate still stops before `PlayerEgo`
allocation. The next bounded step is to provide the real Ship/equipment and
graphics dependencies needed to execute this constructor and `setShip` in
the continuous host/ARMv7 gate.
