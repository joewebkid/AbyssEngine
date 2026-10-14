# PlayerEgo ship-control recovery (2026-10-14)

Android Thumb `PlayerEgo::handleShip(int)` at `0x9bdf0–0x9c12c` is the
primary evidence, cross-checked against the IDA and Ghidra dumps. The
previous source called a declared-only orientation helper, obtained a
canvas value from an unrelated field, and used an uninitialized sound
pointer. It also cleared the direction flags at PlayerEgo `+0xf4/+0xf8`
instead of the angle accumulators at `+0x100/+0x104`.

The recovered body sets two engine-event parameters, rotates the ship
transform from pitch/yaw input, composes optional roll, advances along
the forward vector, rebuilds the three normalized basis vectors,
applies and decays strafe displacement, and translates the follow
camera without an update. It writes the local transform, resets the
pitch/yaw ramps and angle accumulators, then assigns the composed
ship/body transform to Player. The adjacent `handleTurretView` uses
the same accumulator reset and is corrected in this package.

The host smoke extracts the production `handleShip` body and checks
sound parameters, rotations, transform composition, forward and strafe
movement, camera translation, the low-strafe and roll branches, and
resets. The turret-view smoke separately checks that angle accumulators
are cleared while direction flags stay intact. Native `gof2`, 206/206
ARM translation units, both host smokes and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy for `handleShip`
rises **40.8%/23.7% → 92.1%/60.4%**; it is not exact. The adjacent
`handleTurretView` rises **97.8%/80.4% → 97.9%/80.8%** with its reset
correction and is not exact.

The full ARM corpus report
`_work/player_handle_ship_full_20261014.json` compares **4,323/4,524**
original functions at **93.33%** average source-shape and **82.23%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras** and zero skipped. Against
`_work/player_roll_full_20261014.json`, both exact-pair sets have
zero losses and zero gains. The **201 missing** original functions are
unchanged.

The smoke's matrix fixture is intentionally simplified; it cannot
prove live ship motion, sound behavior, camera feel or ARM32 runtime.
The remaining source-shape gap is mostly register allocation and
instruction scheduling, but exact compiler behavior is not established.
Next priority: audit an adjacent PlayerEgo control path with an
unresolved helper or a concrete runtime-risk mismatch.
