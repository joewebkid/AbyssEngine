# PlayerEgo docking-point ease recovery (2026-10-14)

Android Thumb `PlayerEgo::dockToDockingPoint(KIPlayer*, Radar*)` at
`0x9f9f4–0x9fcb4` is the primary evidence, cross-checked with IDA and
Ghidra. The successful navigation-point branch loads `Globals::Canvas`
at `0x9fa6a`, passes `PlayerEgo+0xc/+0x10` to `resetCamera` at `0x9fa74`,
and calls `setLookAtCam(true)` at `0x9fa8e`. The previous body used a
different canvas expression, read `LevelScript::m_pLevel`, and passed
`false` to the camera.

Thumb `0x9fac0–0x9fae8` fetches the navigation point again, copies the
current 60-byte ship matrix, and replaces its translation with the
point's first three floats. `0x9faec–0x9fb5e` allocates an
`EaseInOutMatrix` from the current to the translated matrix with a
3,000 ms duration. This now replaces the declared-only
`PE_dtdp_makeEase` shim; ownership and deletion of the previous ease
object stay in the same branch.

A production-body host smoke checks initial docking, the successful
point path, old-ease destruction, the new matrix/duration and camera
state, missing-point and explicit undock, and dead-player early exit. Native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy changes **77.7%/54.3% →
97.7%/66.4%**; the function is not exact.

Full report `_work/player_dock_point_full_20261014.json` compares
**4,323/4,524** original functions at **93.36%** average source-shape
and **82.25%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, **201 missing** and zero skipped.
The linked- and byte-exact pair sets have no losses or gains against
`_work/player_update_maneuver_full_20261014.json`.

The fixture does not prove a live docking sequence or ARM32 runtime.
The declared-only `PE_aa_approachStep` path belongs to
`approachAsteroid`, not `approachDockingPoint`; it is recovered in
[the following asteroid-approach audit](PLAYER_EGO_ASTEROID_APPROACH_STAGE0_ARM_2026-10-14.md).
