# Pan pre-dead Fighter and KIPlayer::setDead — ARM and host (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `KIPlayer::setDead` at `0xa6208`. It stores state 4 at `+0x88`,
passes false, and tail-branches through the `0x19be08` veneer to
`KIPlayer::setActive(bool)@plt` at `0x632f4`, without loading the Player pointer.
The old source called `Player::setActive` directly and added that load.
Calling `KIPlayer::setActive(false)` instead preserves behavior while
matching the original four-instruction source shape: focused source-shape
**91.6% → 100%**, strict fuzzy **88.9% → 100%**. It remains nonexact
under linked and byte criteria. The unchanged `KIPlayer::setRoute` and
`setJumper` are already 100% source-shape/strict fuzzy; `setRoute` is
linked-exact, `setJumper` byte-exact.

`tools/run_level_stage1_dead_friend_pan_gate.py` extends the original-
asset Pan host gate beyond all ordinary friends through one iteration of
the following `field_164` loop. The fixture keeps the `nextInt(2)` draw
but forces `field_164 = 1` so this branch runs on every test seed;
security remains the inherited synthetic value 3. It executes source-
extracted `createShip`, `KIPlayer::setDead`, `setRoute`, `setJumper`,
`KIPlayer::setActive`, and the production Route/Player destructors;
the Gun destructor is linked as a cleanup dependency, not asserted to run.
Test-only instrumentation confirms that replacing the route destroys
the old one exactly once.

`PlayerFighter` has its own `route` at `+0x144`; base `KIPlayer::route`
at `+0x6c` is the Fighter's `routeClone`. Thus `KIPlayer::setRoute`
replaces `routeClone` and updates `initialRoute`; it does not replace
the Fighter's main route. The gate checks this distinction, state 4,
inactive Player, jumper flag, the new waypoint's coordinate bounds,
material requests, enemy slots and ordered LOD objects. Windows
SwiftShader `-O0/-O2` passes with **4+1** and **3+1** Fighters in the
two runs. The original assets were read-only.

Native `gof2` builds, **208/208** ARM translation units compile, and
the **93** verifier unit tests pass (one skip). The full ARM corpus
compares **4,323/4,524** original functions over **4,736** pairs, mean
**94.19%** source-shape and **83.54%** strict fuzzy, **2,731 100%-fuzzy**,
**2,564 linked-exact** and **1,082 byte-exact**; **201** are missing,
zero wrong-type and zero skipped. Against the previous full report,
strict fuzzy rises from **83.53%** and 100%-fuzzy from **2,730**;
the linked-exact and byte-exact identity sets have **zero losses**.
The focused `KIPlayer` unit retains all **56 linked-exact** and
**35 byte-exact** matches across its **69** comparable functions.

The gate still stops before the `field_168` loop. It does not establish
Fighter teardown, full stage-one return, rendering, or live Android
behavior; ADB listed no device. Next: verify the following
static/outpost branch with Android ARM evidence and run the host gate
on ARMv7 hardware when available.
