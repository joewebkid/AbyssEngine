# PlayerEgo turret-view camera recovery (2026-10-14)

Android `PlayerEgo::handleTurretView(int)` at `0x9b96c` is the primary
source, cross-checked against the IDA and Ghidra dumps. Thumb
`0x9b98c–0x9b9c8` gates forward motion on both the autopilot/stream
condition and the docking-point condition. The old source initialized
`move` to true and did not enforce the former condition. Thumb
`0x9bb66–0x9bb76` passes the camera handle from PlayerEgo `+0x174`
and the composed matrix; the old source instead passed a canvas pointer
as the handle and interpreted camera-handle storage as a matrix.

The recovered method composes the ship, camera-node and camera-mid
matrices. When hit shake or boost is active, it forms a look-at target
from matrix position minus direction. Hit shake advances its timer,
clears the flag after 1,000 ms, and perturbs each eye coordinate using
three `nextInt(12)` calls at 0.001 scale. Boost clamps the `+0xc8`
strength to 2–8 and perturbs each target coordinate using three
`nextInt(2 * strength)` calls multiplied by boost percentage and
0.0015. It then updates the correct camera, clears the pitch/yaw
direction accumulators, calls `roll`, and assigns the product of the
ship/body local transforms to the Player matrix. The call order and
field offsets are visible in Thumb `0x9b9c8–0x9bbb0`.

The production-body host smoke checks both movement gates, matrix and
camera arguments, shake timeout and RNG bounds/order, boost clamps,
roll and Player transform assignment. Native `gof2`, 206/206 ARM
translation units and 37 verifier tests (one skipped) pass. Focused
source-shape/strict fuzzy rises from **80.8%/59.3%** to
**97.8%/80.4%**; the method is not exact.

The full ARM corpus report
`_work/player_turret_view_full_20261014.json` compares
**4,323/4,524** original functions at **93.31%** average
source-shape and **82.22%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped. Compared with
the preceding auto-turret aim report, the exact-pair sets have no
losses or gains.

The smoke uses simplified matrix arithmetic and deterministic RNG;
it cannot prove live camera feel, visual shake or ARM32 runtime. Other
`PlayerEgo` methods retain declared-only helpers and need independent
recovery. Next priority: the adjacent `handleShip`/`roll` path and
its remaining wrapper calls.
