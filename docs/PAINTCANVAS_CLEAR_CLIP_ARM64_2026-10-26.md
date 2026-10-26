# PaintCanvas clear and clip recovery (2026-10-26)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x7ca84`, `PaintCanvas::ClearBuffer` enables depth testing, enables
depth writes and calls `Engine::ClearBuffer` with its color argument.
The latter sets the clear color and clears color plus depth. At
`0x7caa6`, `PaintCanvas::ClearDepth` instead tail-calls `glClear(0x100)`:
it clears only depth. The recovered code previously called
`ClearBuffer(0x100)` from `ClearDepth`, which cleared color as well.

At `0x7cab0`, `EnableClip` enables the scissor test and chooses
`glScissor` arguments from the canvas orientation. The ARM register
flow confirms orientation 0 uses `(y, x, h, w)` and orientation 2
uses `(x, GetHeight() - (y + h), w, h)`. The recovered code previously
swapped these pairs. Orientation 1 and 3 branches retain their
existing transformations. At `0x7cb36`, `DisableClip` tail-calls
`glDisable(0x0c11)`. All these calls now use the confirmed GLES and
engine targets rather than unresolved wrappers.

Native `gof2`, AArch64 static `gof2` and **209/209** ARMv7 translation
units build. The PaintCanvas comparison covers **198** symbols.
`ClearDepth` improves from **100% source-shape / 50% strict fuzzy**
without linked-exact to **100%/100% and linked-exact**. `EnableClip`
improves **88.2%/19.4% → 98.4%/50.0%**. The PaintCanvas unit grows
**92 → 93 linked-exact** while retaining all **23 byte-exact** symbols;
there are no exact-set losses.

The full ARM corpus compares **4,323/4,524** original functions over
**4,743** pairs with **zero skips**. Mean source-shape/strict fuzzy
is **94.72%/84.23%**. It has **2,604 linked-exact** and **1,083
byte-exact** pairs, up from 2,603/1,083 in the preceding full run;
the exact symbol sets have **zero losses**.

The strict AArch64 whole-archive unresolved-name inventory falls
**469 → 461** and the JNI-reachable inventory **229 → 221**. Eight
obsolete `paintcanvas_ext_*` names disappear with no new names.
The game shared library and live clear/clip behavior remain unverified.
Next priority is the still-unrecovered `PaintCanvas::End3d` scene
shutdown path, followed by other JNI-reachable engine and audio calls.
