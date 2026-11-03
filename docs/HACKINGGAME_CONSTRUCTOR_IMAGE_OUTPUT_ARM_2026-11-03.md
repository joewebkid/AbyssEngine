# HackingGame image output slots (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `HackingGame` constructor at `0x14ecc8` passes the addresses
of the object's image-handle slots directly to `PaintCanvas::Image2DCreate`.
Each of the six loop iterations fills one slot for image ID `0x1f4a + i`
and one for `0x1f50 + i`, starting from the image array at object offset
`+0x4c`. The reconstruction passed a single stack temporary for both
calls, then copied each returned handle into the object. The constructor
now passes each object slot as the output reference. The handle array uses
`unsigned int`, matching the method's output type and the other handle
fields, without changing the 32-bit object layout.

Focused ARMv7 verification compiles 209/209 translation units. Both
constructor symbols (`C1` and `C2`) improve from 78.6% source-shape and
43.2% strict fuzzy to **100% in both metrics** and become linked-exact.
The `HackingGame` unit now has 13 linked-exact and 10 byte-exact
comparisons. `render2D` remains 79.4% source-shape, 14.8% strict fuzzy
and nonexact. Focused report:
`_work/hacking_ctor_direct_output_focused_20261103.json`.

This verifies the constructor's ARM code shape; it does not validate
the hacking screen on a live device. Redmi testing remains stopped.

Windows native and ARM64 static `gof2` builds pass. The full ARM
corpus compares 4,323/4,524 original functions in 4,769 pairs, with
201 missing and zero skipped. Mean source-shape rises from 96.64% to
96.65%; mean strict fuzzy rises from 86.58% to 86.60%. Linked-exact
grows from 2,776 to 2,778; byte-exact stays 1,107. Both newly exact
symbols are the constructor variants. No comparison pair or exact
match was lost. Full report:
`_work/hacking_ctor_direct_output_full_20261103.json`.

Next priority: inspect the remaining draw-call and resource-sizing
sequence in `render2D` against the original ARM, then validate the
screen on a permitted device.
