# Station activation audio and button positions

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
The `ModStation::OnInitialize` station-activation route at
`0xd5e78–0xd5f36` was checked against the original NDK ARM objdump
and REA/IDA pseudocode. Decompiled text was used as an interpretation
aid; the ARM calls, arguments and branches are the evidence.

The original plays sound `0x7a`, calls `enableReverb(0)` and
`setDownPitch(false)`, plays and fades the pending music mode when it
is not `-1`, then resets that global mode to `-1`. It marks the
station active and sets state 100. If `buttonRow` is non-null, it
iterates the whole array, writes integer X/Y positions for indices
0–9 into the global submenu arrays, and stores the full row size in
`Globals::sub_menu_button_count`. It calls `getPosition()` separately
for X and Y. A null row leaves the global count unchanged.

The previous code used a null-initialized sound/music placeholder and
an empty loop. The ARMv7 compiler emitted an undefined-instruction
trap after those calls, so the coordinate loop was absent from the
object even after its first reconstruction. Replacing the placeholder
with `Globals::sound`, `Globals::globals` and
`Globals::switch_to_target_setting` made the route reachable in the
compiled object. The production-route smoke at `-O0` and `-O2`
checks sound IDs and call order, pending/no-pending music, state,
12-button input with the ten-coordinate limit, two position reads
per included button, and a null row.

Focused ARM verification compiles **209/209** translation units.
For the full 2,235-instruction `OnInitialize`, source-shape improves
**69.4% → 71.5%** and generated instructions rise **1,194 → 1,264**;
strict fuzzy changes **15.0% → 14.7%**. The object now contains the
two `TouchButton::getPosition()` call sites in the activation loop.
Windows native and ARM64 static builds pass. The focused `ModStation`
unit remains **11 linked-exact / 3 byte-exact**.

The full ARM corpus compares **4,323/4,524** original functions in
**4,767** pairs, at **96.45% mean source-shape / 86.31% strict fuzzy**,
with **2,759 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets are unchanged from the
preceding full report.

Live audio and station input remain unverified. The separate
`state == 100` sound-resource initialization path still uses an
unbound sound pointer and is the next bounded priority. Redmi testing
remains stopped by user request.
