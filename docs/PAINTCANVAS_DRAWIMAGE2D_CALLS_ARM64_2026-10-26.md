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
