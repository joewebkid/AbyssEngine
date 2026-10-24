# Condition-selected small outpost — Pan host gate (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` SHA256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`
has the faction-one small-outpost branch in `Level::createMission` at
`0xb2f3c–0xb3078`, with the static type-19051 call at `0xb2fc2` and
five type-6774 turret records at `0x1fcedc`. The earlier
[forced-variant gate](LEVEL_STAGE1_SMALL_OUTPOST_ARM_HOST_2026-10-24.md)
covered object construction, not the branch predicate.

`tools/run_level_stage1_selected_small_outpost_pan_gate.py` reuses the
source-extracted original-asset Pan chain. At the `createMission` boundary
only, it sets faction **1** and campaign mission **141**. A fixed host clock
(`1792800112`) feeds the original `AERandom::reset`; no `field_164`,
`field_168`, or outpost flag is forced. The checked RNG sequence yields
one pre-dead Fighter, one class-one slot, no ninjas, and a small-outpost roll
of **22** (inside the original `<= 29` predicate). The extracted branch
asserts `smallOutpost && !largeOutpost && !alternateLarge`. An independent
host oracle checks the RNG-derived counts, waypoint coordinates, first
ship type and health, and the resulting enemy-array size. The inherited
gate then checks the static carrier and five distinct turret poses,
meshes, collisions, dock-file lifetime, health and Level membership.

Windows SwiftShader `-O0/-O2` both pass with original assets. Native
`gof2` builds (no-op). No production C++ changed, so the last validated
full ARM corpus remains **4,323/4,524** originals over **4,736** pairs,
mean **94.19% source-shape / 83.54% strict fuzzy**, **2,564
linked-exact**, **1,082 byte-exact**. Fresh focused ARM verification was
unavailable: the local r18b objdump is absent; the focused command
returned no comparable functions. The last recorded focused values
remain **98.5%/40.3%** for `createMission` and **98.9%/44.3%** for
`createStaticObject`, both nonexact, but were not remeasured here.
ADB listed no device.

This is condition-driven selection *within a synthetic boundary*, not
an ordinary faction-one post-140 campaign run. Stage zero still loads
Pan as faction zero, and the test changes faction/mission only before
`createMission`; the clock and some host services are test-only. Live
Android, turret combat/teardown, and an end-to-end campaign-141 station
remain unverified. Next: a coherent faction-one station/system and
campaign fixture without the boundary overrides.
