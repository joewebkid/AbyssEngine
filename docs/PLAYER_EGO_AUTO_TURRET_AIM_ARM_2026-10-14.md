# PlayerEgo auto-turret aim and fire (2026-10-14)

Android `PlayerEgo::handleAutoTurret(int)` at `0x9ca78` no longer
needs the declared-only `PE_hat_aimAndFire` shim. IDA, Ghidra and
Thumb `0x9cb70–0x9cde4` show the complete post-selection path. It
copies the target Player matrix, obtains the target's virtual
position, and adds 1,500 units along the normalized matrix direction.
That predicted point is stored at PlayerEgo `+0xec`. A product of
the ship and turret geometry matrices transforms it into a normalized
local direction. Horizontal error beyond ±0.05 rotates both the
roll and turret geometries; vertical error beyond ±0.05 adjusts the
look accumulator and rotates the turret geometry. The ARM literals
establish the 200-unit divisor, `0.000244140625f`, `6.2831855f`,
the `-500/70` vertical limits and the 500 ms fire-stop threshold.
When aligned, the original calls `Player::shoot(2, dt, false, matrix)`,
updates both transforms and clears the fire timer.

Focused ARM comparison also exposed an adjacent ABI error:
`KIPlayer::noTargetFlag` was mapped to `+0x6c`, overlapping the route
pointer, while Thumb at `0x9caf6` reads `+0x74`. The header now
aliases `noTargetFlag` to the existing `field_0x74`, with a 32-bit
offset assertion. Other callers in `Level` already write that byte.

The production-body host smoke covers target filtering, the predicted
point, aligned firing, both horizontal/vertical rotations, the
vertical limits, transform updates and the timeout stop. Native
`gof2`, 206/206 ARM translation units and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy rises from
**45.7%/24.0% to 93.8%/54.1%**; the whole method is not exact.

The full ARM corpus report
`_work/player_auto_turret_aim_full_20261014.json` compares
**4,323/4,524** original functions at **93.31%** average
source-shape and **82.22%** strict fuzzy. It retains **2,482
linked-exact** and **1,065 byte-exact** pairs, with **123 extras**
and zero skipped. Compared with the selection baseline report,
neither exact-pair set gained or lost a member.

The fixture simplifies matrix math and does not establish live aim
accuracy, animation side effects, game-asset behavior or ARM32
runtime. Other `PlayerEgo` methods still reference declared-only
vector and matrix wrappers. Next: recover the adjacent
`handleTurretView` camera/vector path and validate the auto-turret
with real transforms on ARM32 when that runtime is available.
