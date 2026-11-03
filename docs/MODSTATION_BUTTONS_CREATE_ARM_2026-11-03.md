# Station button creation

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA pseudocode was cross-checked with NDK ARM objdump for the
`ModStation::OnInitialize` state-80 block at `0xd5a68–0xd5d06`.
The decompiler output is an aid to interpreting the original
instructions, not original C++ source.

The state-80 route initializes station layout boundaries, creates
the launch button with an empty string and kind 12, formats credits
for the kind-11 credits button, then allocates an
`Array<TouchButton*>` of length five at `ModStation+0x8c`. It creates
row buttons with game-text IDs **167, 398, 177, 129, 169**, common
kind 0, flags `0x11/1`, and positions determined by Layout fields
`0x10`, `0x24`, `0x28`, `0x30`, `0x34`, `0x4c`, and `0xcc`. The row width
is `Layout[0xcc] - Layout[0x28] - Layout[0x4c]`; the first Y is
`screen height - Layout[0x10] - Layout[0x24] - 5*Layout[0x30]
- 4*Layout[0x34]`. The route then advances state 80 to 60.

Previously, this entire UI creation segment was a declaration-only
`DlcMenu_build_oiImpl` call. It left the launch, credits and row
pointers without their original allocation and arguments. The source
now creates all seven buttons and the row using typed constructors.
A smoke extracts this production block and checks its output on a
64-bit host at `-O0` and `-O2`: text IDs, coordinates, flags, widths,
row size and the separate launch and credits parameters.

Focused ARM verification compiles **209/209** translation units.
For the full 2,235-instruction `OnInitialize`, source-shape improves
**61.4% → 69.4%**, generated instructions increase **969 → 1,194**,
and strict fuzzy changes **16.8% → 15.0%**. The `ModStation` focused
report has **39** comparisons and **11 linked-exact / 3 byte-exact**;
new symbols include the array constructor and the linked-exact
`ArraySetLength<TouchButton*>`. Windows native and ARM64 static builds
pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,767** pairs, at **96.45% mean source-shape / 86.31% strict fuzzy**,
with **2,759 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. No old pair or exact match was lost. The two new pairs are
the array constructor and `ArraySetLength<TouchButton*>`; the latter
is the new linked-exact match.

Live UI creation and visual layout remain unverified. In particular,
the later state-100 route still has a no-op loop where original ARM
copies button positions into global submenu coordinates. That route
is the next bounded priority. Redmi testing remains stopped by user
request.
