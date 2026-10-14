# PlayerEgo emergency-system visual lifecycle (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::update` in the 2.0.16
`libgof2hdaa.so` at `0x9e60c–0x9e748`, cross-checked with Android IDA
lines 84435–84437 and 84769–84810. The branch requires both geometry
at `PlayerEgo+0xac` and a positive timer at `+0x30c`. The previous C++
decremented this timer near the cloak recharge handling at the start of
`update`, independently of geometry. ARM instead advances it after ship
repair and before `Player::update`.

While active, ARM clears player vulnerability, subtracts frame time,
sets the effect matrix from `Player::transformMatrix`, rotates the
saved offset at `+0x314` through that matrix, and translates the effect.
It advances `Mesh+0x24` (`shaderAnimValue1`) by `dt * 0.001` and scales
the geometry using 5% entry/exit ramps around the duration at `+0x310`,
multiplied by the size at `+0x320`. The transform is updated even in the
frame that exhausts the timer. Only then does ARM delete the geometry,
clear `+0xac`, restore vulnerability, stop sound 1115, and clear
`Status+0x13c`. The restored code keeps this order and does not clamp the
last frame's scale, because the original does not.

The extracted production-fragment smoke checks inactive gates, entry,
steady state, exit ramp, transform update, sound and status teardown,
and one-time geometry deletion. The existing cloak recharge check now
also confirms that the emergency timer is not decremented in the early
subsystem block. Native `gof2`, all 206 ARM translation units, the
focused smoke, cloak/setShip/render smokes, and 37 verifier tests (one
skipped) pass.

Focused `PlayerEgo::update` source-shape rises **77.0% → 79.5%** and
strict fuzzy **31.1% → 32.3%**; generated/original instruction counts
change **2,043/2,913 → 2,138/2,913**. It remains non-exact.
Full report `_work/player_emergency_full_20261014.json` compares
**4,323/4,524** original functions with **201 missing**, **123 extras**
and zero skipped. Corpus averages remain **93.39%** source-shape and
**82.28%** strict fuzzy. The **2,482 linked-exact** and **1,065
byte-exact** pair sets lose and gain zero members versus
`_work/player_cloak_lifecycle_full_20261014.json`.

This smoke does not execute the entire `update` method or live ARM32
rendering. Lifetime of an unused emergency geometry when a ship is
replaced or the ego is destroyed remains unverified; that ownership
path is the next priority.
