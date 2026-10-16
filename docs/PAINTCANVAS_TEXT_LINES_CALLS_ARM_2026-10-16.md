# PaintCanvas text-line call contracts — Android ARM, 2026-10-16

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements the two `PaintCanvas::DrawTextLines` alignment overloads
at `0x7c6c0` (center flag) and `0x7c726` (right-align parameter and
flag). REA/IDA Thumb instructions at `0x7c6ea/0x7c708/0x7c710` and
`0x7c750/0x7c76e/0x7c776` respectively call
`PaintCanvas::GetTextWidth(unsigned int, String const&)`,
`DrawString(unsigned int, String const&, int, int, bool)` and
`GetTextHeight(unsigned int)`. Registers and stack stores carry the
array-selected String, font, coordinates and false drawing flag.
The decompiler's guessed return type is not evidence for the C++
signature.

Before this change, both recovered bodies generated linked-exact
instructions but routed these six calls to declared-only
`paintcanvas_ext_dtl_*` symbols. They now invoke the existing methods
with the same arguments; the unused shim declarations were removed.
No alignment condition, offset formula, array traversal or vertical
advance was changed.

`tools/run_paintcanvas_draw_text_lines_smoke.py` extracts the two
production bodies into a small host fixture. It checks centered,
right-aligned and unaligned coordinates, nine-pixel line advances,
callee arguments and an empty array. Its text methods are instrumented
stubs: this does not validate real fonts, glyph drawing or GL output.

Native `gof2`, **206/206** ARM translation units and **40** verifier
tests (one pre-existing skip) pass. Both changed overloads stay
**100% source-shape / 100% strict fuzzy**, **linked-exact**, not
byte-exact. The unchanged four-argument forwarding overload remains
**94.8% source-shape / 53.3% strict fuzzy**. All six corrected
`R_ARM_THM_CALL` destinations match the original named callees.

The full corpus compares **4,323/4,524** unique original functions,
with **201 missing**, **123 extras**, zero skipped,
**93.59%/82.64%** average source-shape/strict fuzzy,
**2,503 linked-exact** and **1,074 byte-exact**. Against the previous
full report, exact `(unit,symbol)` sets have zero losses and gains.
The direct-call audit changes from **2,354 matches / 71 mismatch
candidates** to **2,360 / 65**, with **647 unresolved** unchanged.
Reproducible reports are on A: at
`_work/draw_text_lines_before_20261016.json`,
`_work/draw_text_lines_after_20261016.json`,
`_work/draw_text_lines_full_20261016.json` and
`_work/draw_text_lines_calls_full_20261016.json`.

The callees' full behavior, actual Android ARM32 UI output and
asset-backed text rendering remain unverified. The next bounded
priority is another direct-call candidate with confirmed ABI and
source implementation; similarity alone must not authorize a rewrite.
