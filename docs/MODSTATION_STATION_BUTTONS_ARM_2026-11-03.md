# Station main-button touch dispatch

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

In `ModStation::OnTouchEnd` the original instructions show:

| ARM address | Original operation |
| --- | --- |
| `0xd8716–0xd8740` | Call `TouchButton::OnTouchEnd(x,y)` on `this+0x90` (`buttonLaunch`); accepted touch tail-calls `leaveStation()`. |
| `0xd8866–0xd88b2` | Call the button at `this+0x94` (`buttonCredits`); on acceptance set `Globals::options[0x4e]`, save options, initialize or reuse Hangar and show its credit-purchase window. |
| `0xd88b6–0xd88da` | Clear `selectedButton`, scan five buttons from the `Array` at `this+0x8c`, pass both x and y, store the accepted index and dispatch key `0x10000`. |
| `0xd88dc–0xd89ec` | Pass x/y to `Globals::layout`, use text ID `0x284` for help and pass x/y to the news ticker. |

The original vtable `_ZTV10ModStation` at `0x20d9e4` has
`OnInitialize` at `+0x10`, `OnRelease` at `+0x14`, and `OnKeyPress`
at `+0x18` (slot `+0x10` from the address point). The newly compiled
ARM object has these same relative entries and the remaining module
methods in the original order. At `0xd92c6–0xd92f2`, original
`OnKeyPress` reads the key from registers R2/R3: the first 64-bit
argument. The source previously named the second argument as the key.

The reconstruction had substituted unrelated object fields for the
launch and credit buttons, passed only x, read the `Array` header as
button pointers, used an unbound module callback, and read an unbound
layout pointer. It now uses the actual object fields and methods,
the original `0x10000` key dispatch, the correct argument order,
and the original layout and help-text ownership.

Focused ARM verification compiles **209/209** translation units.
For the whole 1,925-instruction `OnTouchEnd`, source-shape improves
from **70.2% to 72.2%** while strict fuzzy falls from **18.0% to
4.4%**; generated instructions rise from 1,039 to 1,089. For
`OnKeyPress`, source-shape changes **93.1% → 92.0%** and strict fuzzy
**48.7% → 47.3%** as its ABI argument is corrected. The `ModStation`
unit retains **9 linked-exact / 3 byte-exact** matches.

Production-branch host smokes at `-O0` and `-O2` cover launch,
credit purchase entry, five-button ordering, x/y forwarding, key
dispatch, constructor reuse and DLC-open layout behavior. Windows
native and ARM64 static builds pass. Live station and purchase
behavior remain unverified; Redmi testing remains stopped by user
request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs, at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets are identical to the
preceding full report; there are no losses.

Next: inspect the Hangar and SpaceLounge close paths in the same
function, especially sound state and submenu button coordinates.
