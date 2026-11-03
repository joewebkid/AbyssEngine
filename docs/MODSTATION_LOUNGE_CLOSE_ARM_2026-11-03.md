# Station SpaceLounge close route

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The `ModStation::OnTouchEnd` block at `0xd8138–0xd81b4` was
checked against the original instructions with the NDK ARM objdump.

When the lounge's `OnTouchEnd(x,y)` returns nonzero, the original
clears `this+0x65`, resets the idle camera and light, then calls
`Globals::sound->setParamValue(0, currentMusicEvent, 0.0f)`,
`stop(0x6c)`, and `play(0x7a, nullptr, nullptr, 0.0f)` in that order.
It calls `CutScene::checkForTurret()` and, when the existing
SpaceLounge requests a hangar update, destroys the Hangar window
and clears `this+0x78`.

The reconstruction previously passed a sound pointer as an event
ID, omitted the `0x6c` stop ID, and used unbound wrappers for the
turret and hangar-update calls. The route now uses the actual sound,
cutscene, lounge, and hangar objects. Accepted and rejected closes,
audio arguments and call order, both reset calls, update/no-update
paths, and hangar destruction are exercised from the production
branch in a host smoke at `-O0` and `-O2`.

Focused ARM verification compiles **209/209** translation units.
For the whole 1,925-instruction `OnTouchEnd`, source-shape improves
from **72.2% to 72.5%**, strict fuzzy remains **4.4%**, and generated
instructions rise from 1,089 to 1,098. The `ModStation` unit keeps
**9 linked-exact / 3 byte-exact** matches. Windows native and ARM64
static builds pass. Live audio and lounge behavior remain unverified;
Redmi testing remains stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs, at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets are identical to the
preceding full report; there are no losses.

Next: reconstruct the Hangar close route in the same function,
including cargo adjustment, sound state and global submenu positions.
