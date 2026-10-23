# PlayerEgo volatile-cargo and gamma-failure update (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerEgo::update(int, Radar*, Hud*, Radio*, LevelScript*, int, bool, int)`
at `0x9ce38`, instruction window `0x9db60–0x9dd18`. The local IDA
decompilation supplies the control-flow outline; the shipped ELF
instructions confirm the fields, constants and call sequence.

The recovered C++ previously jumped directly from collision dispatch
to asteroid docking. ARM instead handles volatile cargo first: event 35
starts if needed; the maximum absolute change of the two steering
components at Ego `+0x268/+0x26c` and `+0x270/+0x274` accumulates
`Player::flShake` with `dt * 0.001 * 0.5`; previous components are
updated. Shake at least 1 kills the Player unless planet or stream
docking is active, and starts a type-12 explosion at the current ship
position if one is not already owned. The explosion scale is 0.7.
The sound parameter is the shake value while alive and zero when dead.
Without volatile goods, shake is reset to zero. Independently, depleted
gamma HP kills the Player unless planet docking or the low `docked`
byte is set. The stream-docking byte does not suppress that gamma check.

The source-extracted smoke exercises sound start/reuse, positive and
negative steering changes, sample updates, fatal shake, explosion
ownership, docking exclusions, sound parameter, non-volatile reset and
gamma failure at `-O0/-O2`. Native UCRT64 `gof2`, all 208 NDK r18b ARM
translation units and 93 verifier tests pass (one existing skip).
Of 32 PlayerEgo smoke scripts, 26 pass; the same six pre-existing
scripts fail outside this block. Focused `update` source-shape/strict
fuzzy changes **85.5%/37.9% → 90.1%/25.1%**; it remains nonexact.
The fuzzy decline is not used to discard ARM-confirmed behavior.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,736** pairs at rounded **94.09%/83.41%** mean
source-shape/strict fuzzy. It retains **2,557 linked-exact** and
**1,082 byte-exact** pairs with zero exact-pair losses, skipped or
wrong-type symbols; 201 original functions remain missing. Only
`PlayerEgo::update` changed score among existing pairs. Full report:
`ego-update-volatile-full.json` (SHA-256
`9c41993903be895b969cafa7264d3525376332cce15555f7c8ddb6e2df7b9fd2`).

The original-asset Pan gate currently stops in `Level::createMission`
after initial hostile/friendly counts, before a runnable game frame.
The original assets and an Android ARMv7 device are available, but a
collision-frame/FMOD/Canvas gate cannot yet be claimed. The next gate
priority is to advance stage-one mission and host initialization to a
frame boundary, then execute the production collision path with original
geometry. This smoke alone does not establish that end-to-end behavior.
