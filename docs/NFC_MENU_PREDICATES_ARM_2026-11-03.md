# Menu-input predicates use the game globals

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The predicates at `0x69e5c–0x6a01e` read exported `Globals::*`
symbols through the GOT. The reconstructed `NFC.cpp` instead declared
seven private static variables with the same conceptual names. No
code wrote those private variables, so virtual-key visibility checks
did not follow the game's live menu and dialogue state.

The original ARM shows these conditions:

| Predicate | Original condition |
| --- | --- |
| `IsDialogVisible` / `IsDialogNotVisible` | dialogue or choice flag nonzero / both zero |
| `IsDialogNotVisible2` | dialogue, choice and menu flags all zero |
| `IsStarMapNotVisible` | star-map byte XOR 1 |
| `IsInGameSubMenuActive` | `subMenuIndex != -1` |
| `IsInGameSubMenuNotActive` | `subMenuIndex == -1`, menu hidden, star map hidden |
| `IsInPrimaryMenu` | menu visible; top-menu, touch-window, dialogue and choice flags zero; star map hidden |

`IsInGameSubMenuNotActive` previously tested `topMenuIndex == 0`
instead of the star-map flag. The predicates now read the actual
`Globals` members and follow these conditions. The recovered return
values and `VirtualKey` predicate slot now use an integer type,
matching the raw ARM returns and zero checks. The original C++ return
type is not encoded in these symbols. `IsSubMenuActive` was given the
same slot-compatible type without changing its existing body.

Focused ARM validation compiled **209/209** translation units.
Six of the seven predicates reached **100% source-shape / 100%
strict fuzzy** and gained linked-exact. `IsInPrimaryMenu` improves
from **37.9% / 9.1%** to **88.0% / 66.7%**. The `VirtualInput` unit
retains its existing exact matches. A native smoke using the actual
predicate functions passes at `-O0` and `-O2`; it exercises dialogue,
choice, menu, submenu and star-map transitions. Windows native and
ARM64 static builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,763** pairs at **96.35% mean source-shape / 86.21% strict fuzzy**,
with **2,754 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Against the preceding full report, these six predicates are
the only linked-exact gains. Pair and byte-exact sets are unchanged;
there are no exact losses.

The smoke injects state into the global variables; it does not prove
that every production path updates them. `IsInPrimaryMenu` still
differs in compiler control-flow shape, and `IsSubMenuActive` in
`VirtualInput` remains about **9.9% source-shape**. The next bounded
priority is that submenu predicate and the state feeding it. No
device run was made; Redmi testing remains stopped by user request.
