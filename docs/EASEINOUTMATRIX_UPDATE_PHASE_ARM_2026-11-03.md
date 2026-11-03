# EaseInOutMatrix interpolation phase (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

In original `UpdateCurrentValue()` at `0x6ec52–0x6ec7e`, the
interpolation parameter at `+0x74` is multiplied by the float literal
`0x40c90fdb` at `0x6ed00`, which is `6.283185482f` (approximately
`2π`). The result is passed to `AEMath::Sinf`. The sine result is
converted from float to double, multiplied by `0.5` and added to
`0.5` in double, then converted back to float. That weight drives
the quaternion and position interpolation. At parameter `1.25f`,
the separate branch copies the maximum matrix directly.

The reconstruction used a `π` multiplier and float-only weight
arithmetic. At the minimum setting `m_t = 0.75f`, its idealized
weight was about `0.854` instead of approximately zero, so
`SetToMinValue()` could select a transform near the maximum. The
source now uses the confirmed `2π` literal and double intermediate.
The float literal rounds to the same `0x40c90fdb` bits as the
original. Quaternion and translation call order remains unchanged.

Focused ARMv7 verification compiles 209/209 translation units.
`UpdateCurrentValue` source-shape improves from 92.5% to 94.6% and
strict fuzzy from 46.5% to 52.2%. It remains nonexact, with 78
reconstructed instructions against 79 original. The
`EaseInOutMatrix` unit retains nine linked-exact and one byte-exact
symbol. Focused report: `_work/ease_matrix_update_phase_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. Live animation
and camera motion on a device remain unverified; Redmi testing stays
stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
remains 96.60% and mean strict fuzzy 86.49% at two-decimal rounding.
Linked-exact remains 2,767 and byte-exact remains 1,099. Neither
comparison pairs nor exact-match sets lose entries. Full report:
`_work/ease_matrix_update_phase_full_20261103.json`.

Next priority: recover a separate behaviorally incomplete area,
starting with `HackingGame::render2D` (currently 18.7% source-shape),
while reserving live easing validation for a permitted device.
