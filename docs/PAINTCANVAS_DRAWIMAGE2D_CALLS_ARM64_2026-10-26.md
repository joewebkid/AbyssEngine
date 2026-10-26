# PaintCanvas DrawImage2D call targets (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `DrawImage2D(unsigned int,int,int)` at `0x78b69` calls
`RestoreImage2D` (`0x78b98`), `SetTexture` (`0x78bac`),
`SetWorldViewMatrix` (`0x78bf8`), `glDisable` (`0x78c00`),
`MeshDraw` (`0x78c10`) and `glEnable` (`0x78c18`). The original
`DrawImage2D(unsigned int,int,int,unsigned char)` at `0x78c49` calls
the same targets at `0x78c80`, `0x78c94`, `0x78d46`, `0x78d4e`,
`0x78d5e` and `0x78d66`. Its integer-to-float conversions are local
instructions, not external function calls. The recovered source now
uses these direct targets and conversions. Both overloads pass the
image pointer to `RestoreImage2D` rather than its restore flag.

Native `gof2`, AArch64 static `gof2` and **209/209** ARMv7 translation
units build. Focused ARM source-shape/strict fuzzy is **99.7%/83.1%**
for the plain overload, unchanged, and **94.4%/63.8%** for the flip
overload, up from **81.4%/52.3%**. Neither overload is linked-exact or
byte-exact. The PaintCanvas unit compares **198** symbols and retains
exactly **92 linked-exact** and **23 byte-exact** symbols, with zero
losses or gains. The previous **4,743**-pair run remains the latest
whole-project corpus; this change received focused and unit checks.

With ARM64 ZIP and crypto dependencies included, the strict
whole-archive unresolved-name inventory falls **499 → 485**, and the
JNI-reachable inventory falls **259 → 245**. Exactly fourteen obsolete
wrapper names disappear, with no new names. Live 2D rendering and the
game shared library remain unverified. Remaining JNI-reachable symbols
include engine code and third-party audio dependencies.

## Remaining overloads

The original `DrawImage2D(unsigned int,int,int,unsigned char,unsigned char)`
starts at `0x78dd0`; the overload with width, height and three flag bytes
starts at `0x78fc0`. The Android ARM call sites resolve to
`RestoreImage2D`, `GetWidth`, `GetHeight`, `SetTexture`,
`SetWorldViewMatrix`, `MeshDraw`, and, in the sized overload,
`glDisable`/`glEnable`. Their integer-to-float conversions are local
instructions. The `SetTexture` call at `0x78f1c` and `0x7900e` passes
`0xffffffff` as its third register argument. The source now calls
those functions directly with the image pointer and confirmed texture
slot.

Native `gof2`, AArch64 static `gof2`, and **209/209** ARMv7 translation
units build. Focused source-shape/strict fuzzy for the five-argument
overload improves **82.0%/36.8% → 88.2%/39.4%**; for the sized
overload it improves **82.9%/30.3% → 96.2%/49.7%**. Neither is exact.
Across all **198** PaintCanvas comparisons, the **92 linked-exact**
and **23 byte-exact** symbol sets retain zero losses. No full corpus
was rerun for this edit.

The strict ARM64 whole-archive inventory falls **485 → 469** and
the JNI-reachable inventory **245 → 229**. Exactly sixteen obsolete
`di3_`/`di4_` wrapper names disappear, with no new names. The game
shared library still fails to link; live 2D output is unverified.
