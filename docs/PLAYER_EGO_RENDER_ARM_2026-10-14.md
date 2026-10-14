# PlayerEgo render dispatch (2026-10-14)

Android Thumb `PlayerEgo::render(bool)` at `0xa1b78–0xa1c56`
is the primary behavior reference; IDA lines 87170–87237 confirm
the branch structure and field roles. The dead branch renders the
first explosion, briefly continues the ship model only through
timer 2999, then renders the second explosion and enables moving
stars. The live branch requires the byte at `PlayerEgo+0x309`,
renders the first explosion and ship once each, conditionally renders
the geometry at `+0xac`, then the auxiliary and turret geometry,
tractor beam and repair beams. The retained turret extra at `+0x30`
tracks `turretActive`; the root at `+0x2c` is rendered afterward.

The previous source tested the separate `+0x308` byte, duplicated
ship/explosion draws, omitted the actual emergency geometry, and
enabled moving stars when frozen or HUD rendering was disabled.
ARM instead enables stars for a live ship only when not frozen,
HUD rendering is allowed, and `dockingPointIndex != 1`.

The extracted production-method smoke checks dead timer boundaries,
the live visibility gate, draw order, the turret extra's visible
state, repair beams, and all moving-stars conditions. Native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped)
pass. Focused `render` source-shape/strict fuzzy changes from
**86.8%/41.4%** to **97.2%/60.4%**; it is neither linked- nor
byte-exact.

Full report `_work/player_render_full_20261014.json` compares
**4,323/4,524** original functions, with **201 missing**, zero
skipped and **123 extras**. Average source-shape changes
**93.38% → 93.39%**, strict fuzzy **82.28% → 82.29%**. The
**2,482 linked-exact** and **1,065 byte-exact** pair sets lose
and gain zero members against
`_work/player_check_turret_full_20261014.json`.

Live ARM32 rendering remains unverified. The `+0xac` pointer is
still represented as a legacy integer in the header; the ARM32
address is confirmed, but ownership is not. Next priority:
`toggleCloaking`, whose material and state branches still diverge
from the Android ARM evidence.
