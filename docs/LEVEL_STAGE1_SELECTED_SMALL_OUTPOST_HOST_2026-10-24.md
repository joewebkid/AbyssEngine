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
feeds the original `AERandom::reset`; no `field_164`, `field_168`, or
outpost flag is forced. Three checked seeds each yield one pre-dead
Fighter, one class-one slot, no ninjas, and a small-outpost roll inside
the original `<= 29` predicate:

| Clock seed | Small-outpost roll | First Fighter type |
| ---: | ---: | ---: |
| `1792800112` | 22 | 9 |
| `1792800340` | 13 | 41 |
| `1792800107` | 17 | 39 |

The extracted branch
asserts `smallOutpost && !largeOutpost && !alternateLarge`. An independent
host oracle checks the RNG-derived counts, waypoint coordinates, first
ship type and health, and the resulting enemy-array size. The inherited
gate then checks the static carrier and five distinct turret poses,
meshes, collisions, dock-file lifetime, health and Level membership.

All three seeds pass Windows SwiftShader `-O0/-O2` with original assets.
`Globals::getRandomEnemyFighter` uses the
[ARM-audited faction-one DLC branches](GLOBALS_RANDOM_ENEMY_FIGHTER_ARM_2026-10-12.md);
fresh focused source-shape/strict fuzzy is **98.9%/95.8%**,
nonexact. Native
`gof2` builds (no-op). The existing shared NDK r18b toolchain compiled
**208/208** ARM translation units with zero failures. Fresh focused
source-shape/strict fuzzy is **98.5%/40.3%** for `createMission` and
**98.9%/44.3%** for `createStaticObject`, both nonexact. The full ARM
corpus compares **4,323/4,524** originals over **4,736** pairs, mean
**94.19% source-shape / 83.54% strict fuzzy**, **2,731 100%-fuzzy**,
**2,564 linked-exact**, **1,082 byte-exact**, zero skips and **201**
absent originals. The fresh JSON report is byte-identical to the
previous full report (SHA256
`013ea037f3236c737dd80bb8430ccf25e6b040b2756f66ae71f073a76c7c8249`);
there are no exact-set losses. ADB listed no device.

The recovered resource table contains materials **32539** and
**32541**, but not the per-ship material IDs **32239** and **32241**
computed by `getShipGroup`. The host permits the latter requests as
no-ops, matching current `PaintCanvas` behavior. Android ARM
`BuildResourceList` loads 32539 at `0xf35ea` and 32541 at `0xf3642`;
`loadLowTexturesAndMaterials` loads 32236 at `0x119898`. A scan of
the original disassembly for those two complete function ranges found
no direct 32239/32241 immediate loads. Computed or table-driven
registrations are not excluded, so original absence remains **needs
confirmation**; this gate must not be read as proof that DLC ship
materials render correctly in the original game.

This is condition-driven selection *within a synthetic boundary*, not
an ordinary faction-one post-140 campaign run. Stage zero still loads
Pan as faction zero, and the test changes faction/mission only before
`createMission`; the clock and some host services are test-only. Live
Android, turret combat/teardown, and an end-to-end campaign-141 station
remain unverified. Next: confirm original registration of the DLC
material IDs, then build a coherent faction-one station/system and
campaign fixture without the boundary overrides.
