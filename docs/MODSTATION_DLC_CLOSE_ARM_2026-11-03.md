# Station DLC-window close route

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

At `0xd860c–0xd868c`, `ModStation::OnTouchEnd` tests the DLC-open
flag at `this+0x62`, passes the original x/y/touch arguments to
`MenuTouchWindow::OnTouchEnd`, and returns immediately if the window
stays open. On a nonzero result it clears the flag, iterates the
`TouchButton` row at `this+0x8c`, and updates
`Globals::sub_menu_buttons_x/y` for indices 0–9. For each stored
button it calls `getPosition()` twice and converts each float
coordinate to an integer. The loop still advances over any buttons
beyond index 9 without writing past the ten-element global arrays.

The previous reconstruction called a provisional window wrapper and
row accessor, used `getX/getY` placeholders, and wrote at most five
coordinates into the station's private cache. The reconstructed
branch now uses the real window method and button row, preserving the
original global coordinate state and call order.

Focused ARM verification compiles **209/209** translation units.
For the whole 1,925-instruction `OnTouchEnd`, source-shape improves
from **67.1% to 69.1%** and strict fuzzy from **15.8% to 21.9%**;
generated instructions rise from 965 to 1,011. The `ModStation`
unit retains **9 linked-exact / 3 byte-exact** matches.

A host smoke compiles the production branch at `-O0` and `-O2`.
It checks touch argument forwarding, rejected and accepted closes,
the ten-entry bound, integer conversion, and the two position reads
per stored button. Windows native and ARM64 static builds pass.
The surrounding station button flows and live device behavior remain
unverified. Redmi testing remains stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs, with **96.45% mean source-shape / 86.32% strict fuzzy**,
**2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets are identical to the
preceding full report; there are no losses.

Next: restore the station's DLC-open button-position initialization
in `OnTouchEnd` using the same confirmed global arrays and row layout.
