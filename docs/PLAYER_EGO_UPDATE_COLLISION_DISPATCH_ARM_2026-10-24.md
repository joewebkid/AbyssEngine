# PlayerEgo frame-to-collision dispatch (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerEgo::update(int, Radar*, Hud*, Radio*, LevelScript*, int, bool, int)`
at `0x9ce38`. Live REA/IDA cross-references to `calcCollision` resolve
three call sites, `0x9db2e`, `0x9db3c` and `0x9db4a`, inside `update`.
The ELF instruction window `0x9da58–0x9db60` independently confirms
their branch conditions, list order and surrounding state writes.

The old C++ `update` never called `calcCollision`, leaving the recovered
collision logic unreachable from the frame loop. The recovered block
resets the waypoint-arrival and movement-state bytes; computer-controlled,
planet-docking or stream-docking frames advance the geometry and update
the Player matrix, with the planet timer advancing only during planet
docking. When collision is enabled and Player is active, it clears the
station flag and tests landmarks, enemies and asteroids in that order.
If a collision sets the station flag in turret mode, it updates the
turret view. The code uses existing fields and methods without ABI edits.

The source-extracted dispatch smoke checks inactive/disabled guards,
list order, station-flag reset and turret reaction, movement and the
planet timer at `-O0/-O2`. The existing production `calcCollision` smoke
passes separately. All 30 PlayerEgo smoke scripts were run: 24 pass;
the same six older scripts that failed before this change still fail
outside the modified block. Those failures are not a green class gate.

Focused `update` source-shape/strict fuzzy changes **80.6%/30.8% →
82.4%/25.4%**. The fuzzy decline is retained because the dispatch and
movement are directly supported by ARM, not inferred from the metric.
All 208 NDK r18b ARM translation units, native UCRT64 `gof2`, and 93
verifier tests (one existing skip) pass.

The full NDK r18b corpus still compares **4,323/4,524** original
functions across **4,736** pairs at rounded **94.09%/83.41%** mean
source-shape/strict fuzzy. It retains **2,557 linked-exact** and
**1,082 byte-exact** pairs, with zero exact-pair losses, skipped or
wrong-type symbols; 201 original functions remain missing. Only
`PlayerEgo::update` changed score among existing pairs. Full report:
`ego-update-collision-full.json` (SHA-256
`14ed1651f51d6f8c91f20c25a8f9a71ece1cc7fcc4f3d700e59fe5a079d37ac7`).

A real game frame using original collision volumes, FMOD and Canvas
state remains unverified. The preceding transform-update segment in
`update` is not recovered by this patch. Next priority is to validate
that segment and then run a bounded original-asset collision frame gate.
