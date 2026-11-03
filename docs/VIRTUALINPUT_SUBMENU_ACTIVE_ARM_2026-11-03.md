# Submenu virtual-key predicate

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `_Z15IsSubMenuActivei` at `0x69f7c–0x69ff6` uses a
`0x58`-byte virtual-key stride and reads the touch-window handle at
offset `0x40`. It rejects handles at or below `0x578` and indices at
or above `Globals::sub_menu_button_count`. A visible menu returns
whether unsigned `Globals::topMenuIndex - 1` exceeds one. With the
menu hidden, an active `Globals::subMenuIndex` returns one; otherwise
it compares `Globals::appManager`'s current module with ID five.

The reconstruction had four private, unwritten pointer variables for
these menu values. It also read an unrelated `ApplicationManager`
field instead of calling `GetCurrentApplicationModule`. The predicate
now reads the live `Globals` fields and uses that method. The
original symbol does not encode the C++ return type; the integer
return stays compatible with the virtual-key predicate slot.

Focused ARM verification compiled **209/209** translation units.
`IsSubMenuActive` improves from **9.9% to 92.8% source-shape** and
from **6.3% to 54.2% strict fuzzy**; its generated instruction count
changes from 2 to 57 against 61 original instructions. The focused
unit retains **9 linked-exact** and **2 byte-exact** functions.
The smoke compiles the unchanged production function at `-O0` and
`-O2` and covers handle bounds, window count, visible-menu index
boundaries, active submenu, and both module outcomes. Native and
ARM64 static builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,763** pairs at **96.37% mean source-shape / 86.22% strict fuzzy**,
with **2,754 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Against the preceding full report, the pair, linked-exact,
and byte-exact sets are identical; no exact matches were lost.

The remaining shape difference is in GOT load order and control flow;
the predicate is not exact. The smoke supplies game state directly,
so the behavior of every producer of these fields remains unverified.
No device run was made. Redmi testing remains stopped by user request.
