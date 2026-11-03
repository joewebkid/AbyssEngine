# Station DLC-window open route

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

At `0xd88dc–0xd8998`, `ModStation::OnTouchEnd` calls the layout's
touch-end handler with x and y. On a nonzero result, it reuses the
window at `this+0x50` or allocates `MenuTouchWindow(2)`, then calls
`Globals::status->checkForLevelUp()` and sets the DLC-open byte at
`this+0x62`. It iterates that window's `buttons` array at window+4.
For indices 0–9, it calls each button's `getPosition()` twice,
converts the float coordinates to integers and stores them in
`Globals::sub_menu_buttons_x/y`. The loop advances over later entries
without writing outside those ten-element arrays.

The previous reconstruction used provisional constructor and level-up
wrappers, read the station's button row instead of the DLC window's
buttons, and wrote only five positions to a private station cache.
The new branch restores the window and global-state ownership shown
by the original instructions.

Focused ARM verification compiles **209/209** translation units.
For the whole 1,925-instruction `OnTouchEnd`, source-shape improves
from **69.1% to 70.2%** and generated instructions rise from 1,011
to 1,039. Strict fuzzy falls from **21.9% to 18.0%**; the function
remains neither linked-exact nor byte-exact. The `ModStation` unit
retains **9 linked-exact / 3 byte-exact** matches.

A host smoke compiles the production branch at `-O0` and `-O2`.
It checks accepted and rejected layout touches, construction and
reuse of the DLC window, level-up calls, the ten-entry bound and
two position reads per stored button. Windows native and ARM64
static builds pass. The surrounding station button dispatch and
live device behavior remain unverified. Redmi testing remains
stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs, at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets are identical to the
preceding full report; there are no losses.

Next: inspect the main station button dispatch immediately before
this layout route, including the `TouchButton::OnTouchEnd` arguments
and selected-button state.
