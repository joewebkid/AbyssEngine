# PlayerEgo pre-collision transforms (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerEgo::update(int, Radar*, Hud*, Radio*, LevelScript*, int, bool, int)`
at `0x9ce38`, instruction window `0x9d940–0x9da58`. The local IDA
decompilation provides the control-flow outline; the shipped ELF
instructions and field offsets are the primary evidence.

Immediately before the recovered frame-to-collision dispatch, the
original advances the active throttle timer and clears its flag at
2,000. In turret mode, when automatic turret equipment is absent and
equipment sort 35 exists, it updates the turret geometry transform and
its optional child. If the auxiliary geometry at Ego `+0x34` exists
and the low byte at `+0x38` is set, it scales from the float at `+0x3c`,
faces the normalized star light with world-up, follows the ship geometry,
and for ship index 8 moves 300 units along the normalized ship direction.
It updates that transform with the stored frame time, then always updates
the root transform before collision dispatch. The previous C++ `update`
omitted this sequence. Ego `+0x3c`, previously declared as a pointer but
written as a float by `checkForTurret`, is now typed as the effect scale;
the ARM offset and 32-bit layout are unchanged.

The source-extracted smoke checks the timer threshold, turret/equipment
gates, auxiliary orientation/position/scale, ship-index-8 offset and
root-transform update at `-O0/-O2`. The existing beam, collision-dispatch
and collision-method smokes pass. All 31 PlayerEgo smoke scripts were run:
25 pass; the same six older scripts still fail outside this block, so
there is no green full-class smoke gate. Native UCRT64 `gof2`, all 208
NDK r18b ARM translation units and 93 verifier tests pass (one existing
skip). Focused `update` source-shape/strict fuzzy changes **82.4%/25.4%
→ 85.5%/37.9%**; it remains nonexact.

The final full NDK r18b corpus compares **4,323/4,524** original
functions across **4,736** pairs at rounded **94.09%/83.41%** mean
source-shape/strict fuzzy. It retains **2,557 linked-exact** and
**1,082 byte-exact** pairs, with zero exact-pair losses, skipped or
wrong-type symbols; 201 original functions remain missing. Only
`PlayerEgo::update` changed score among existing pairs. Full report:
`ego-update-transform-final-full.json` (SHA-256
`93b4f075bd669f450bf5316c01d50627f69a8f91c819a382a77b36d1631b949a`).

This smoke does not prove a real game-frame render or collision with
original assets. The next priority is an original-asset frame gate
through this transform/collision path, including FMOD and Canvas state.
