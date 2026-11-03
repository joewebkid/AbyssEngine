# Station button-row dispatch and transparency

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The `ModStation::OnTouchBegin` row loop at `0xd73f6–0xd7410`,
`OnTouchMove` row loop at `0xd7584–0xd759e`, and
`OnInitialize` button-transparency calls at `0xd602a–0xd6036` and
`0xd619a–0xd61e2` were checked against the original NDK ARM objdump.

The original gets the button row from `ModStation+0x8c`, then the
array data pointer, and sends touch begin and move to exactly five
buttons. The previous C++ read `buttonRow[1]` as a 32-bit `int` and
cast it back to a pointer. That models the ARMv7 layout but reads
the wrong bytes for `Array<TouchButton *>` on a 64-bit host, where
the data pointer follows the four-byte count with padding. Both
input loops now use `Array<TouchButton *>::operator[]`.

The original also calls `TouchButton::setHalfTransparent(true)` on
row indices 0, 2, 3 for campaign missions below 5, indices 2 and 3
for the next campaign range, and index 1 when the station shop should
be shown. The previous code called an unbound wrapper with only a
pointer-sized integer argument. These calls now use the real member
function and pass `true`. The Windows object references
`TouchButton::setHalfTransparent(bool)` and no longer references the
wrapper. This fixes call arguments and 64-bit array access; a live
station test has not yet verified the surrounding initialization.

Focused ARM verification compiles **209/209** translation units.
Whole-function source-shape changes are `OnTouchBegin` **83.2% →
83.1%**, `OnTouchMove` **78.2% → 78.1%**, and `OnInitialize`
**61.4% → 61.4%**. Strict fuzzy changes are **89.9% → 85.3%**,
**87.2% → 81.7%**, and **16.9% → 16.8%**, respectively. The small
metric decreases are recorded rather than reverting the corrected
64-bit access and function argument. Windows native and ARM64 static
builds pass; the focused `ModStation` unit keeps **9 linked-exact /
3 byte-exact** matches.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs, at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Pair membership and both exact sets match the preceding
full report exactly; there are no losses.

Next: verify the button-row allocation and destruction paths and
their remaining wrapper calls. Redmi testing remains stopped by user
request, so actual station input and visual transparency are unverified.
