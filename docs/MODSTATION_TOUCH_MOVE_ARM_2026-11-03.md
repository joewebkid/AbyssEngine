# ModStation touch-move dispatch

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `ModStation::OnTouchMove(int,int,void*)` begins at
`0xd7480`. It accepts only the active touch, records its coordinates,
and routes motion to the first open station window. The main-window
path reads `Globals::layout`, updates the buttons and ticker, tests
the vertical bounds against `Globals::h`, then updates camera scroll.
The original DLC-window call at `0xd7558` forwards the incoming touch
pointer in `R3`.

The reconstruction used two private null-initialized pointers for
layout and screen height. The optimizer consequently reduced this
139-instruction function to ten instructions. It now refers to
`Globals::layout` and `Globals::h`. A reference to the layout slot
preserves the original reload after window calls, which may replace
the active layout. The DLC-window call now forwards `touch` instead
of `nullptr`.

Focused ARM verification compiled **209/209** translation units.
This function rises from **3.6% to 78.2% source-shape** and from
**1.3% to 87.2% strict fuzzy**, with 150 generated instructions
against 139 original. The `ModStation` unit retains its **9
linked-exact** and **3 byte-exact** functions without losses.
The remaining differences are chiefly the compiler's tail-call
choices for early dispatch branches and branch placement; this
function is not exact.

The production function passes a host smoke at `-O0` and `-O2` for
touch identity, coordinate recording, layout/dialogue precedence,
DLC touch-pointer forwarding, and the active-radio early return.
The test does not exercise the main-window button-row path, whose
stored 32-bit pointers are not directly representable in this host
fixture. Windows native and ARM64 static builds pass. No device run
was made; Redmi testing remains stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs at **96.42% mean source-shape / 86.29% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Against the preceding full report, the pair, linked-exact,
and byte-exact sets are identical; there are no losses.

The next bounded priority is `ModStation::OnTouchBegin`, which has
separate private pointer placeholders and needs its original global
bindings checked before changing it.
