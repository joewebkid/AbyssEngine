# Station window touch-end audio state

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `ModStation::OnTouchEnd(int,int,void*)`, the original ARM shows:

| Closed window | Original action after clearing its flag |
| --- | --- |
| `StatusWindow`, `0xd83ba–0xd83dc` | Set current music event parameter zero; no `resetLight` |
| `StarMap`, `0xd7fb8–0xd8006` | `resetLight`, then set current music event parameter zero |
| `MissionsWindow`, `0xd8462–0xd84b2` | Set current music event parameter zero, then `resetLight` |

The original audio call uses `Globals::sound` as the `FModSound`
object, parameter index zero, the object's `currentMusicEvent`
field as event ID, and value `0.0f`. The reconstruction omitted the
audio reset on two routes, called `resetLight` on the status route,
and used a provisional audio wrapper with a wrong receiver and
arguments on the missions route. These three routes now follow the
original operations and ordering.

Focused ARM verification compiled **209/209** translation units.
For the whole 1,925-instruction `OnTouchEnd` function, source-shape
rises from **66.2% to 67.1%** and strict fuzzy from **13.2% to
15.8%**; the generated instruction count rises from 944 to 965.
The `ModStation` unit keeps **9 linked-exact** and **3 byte-exact**
matches without losses.

A host smoke compiles the unchanged production branch snippets at
`-O0` and `-O2`. It checks rejected close attempts, dispatch
precedence, music-event arguments and the `resetLight` counts and
operation order.
Windows native and ARM64 static builds pass. The snippets do not
exercise the surrounding mission and purchase logic, and live audio
output remains unverified. No device run was made; Redmi testing
remains stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. The rounded global means are unchanged; pair and exact sets
are identical to the preceding full report, with no losses.

Next: examine the DLC-window close route in the same function,
including its `MenuTouchWindow` call and submenu button positions.
