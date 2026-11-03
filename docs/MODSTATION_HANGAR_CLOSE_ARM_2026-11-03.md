# Station Hangar close route

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The `ModStation::OnTouchEnd` route at `0xd7ddc–0xd7ea0` and its
shared continuation at `0xd8272–0xd8302` were checked against the
original instructions with the NDK ARM objdump.

After `HangarWindow::OnTouchEnd(x,y)` accepts the close, the original
sums cargo item amounts for indices **132–153** inclusive. It adds
only the positive increase over `ModStation::field_0xd0` to
`Status::field_a8`. It clears Hangar sell mode, resets the idle camera
and the Hangar-open flag. A changed-ship flag on the Hangar triggers
`CutScene::replacePlayerShip(ship index, race)`, followed by a turret
check when the cutscene exists. The audio route calls `stop(0x5f)`,
`play(0x7a, nullptr, nullptr, 0.0f)`, then
`setParamValue(0, currentMusicEvent, 0.0f)`. Finally it refreshes
global submenu coordinates for at most ten buttons in `buttonRow`.

The prior reconstruction used unbound wrappers, read the cargo
baseline from `buttonLaunch` instead of `field_0xd0`, and wrote a
five-button local cache instead of global submenu coordinates.
The route now uses the typed game objects and the actual sound event
ID. A host smoke compiles the production branch at `-O0` and `-O2`.
It checks rejected and accepted closes, item-index boundaries,
positive-only cargo credit, ship replacement, audio IDs and call
order, and the ten-button coordinate limit.

Focused ARM verification compiles **209/209** translation units.
For the whole 1,925-instruction `OnTouchEnd`, source-shape improves
from **72.5% to 73.5%**, strict fuzzy from **4.4% to 15.0%**, and
generated instructions from **1,098 to 1,132**. The `ModStation`
unit keeps **9 linked-exact / 3 byte-exact** matches. Windows native
and ARM64 static builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs, at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets are identical to the
preceding full report; there are no losses.

Live Hangar, cargo and sound behavior remains unverified. Redmi
testing remains stopped by user request. Next: inspect another
bounded `ModStation::OnTouchEnd` route and keep runtime-dependent
claims separate from ARM-backed behavior.
