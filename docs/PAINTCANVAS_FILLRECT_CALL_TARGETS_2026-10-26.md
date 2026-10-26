# PaintCanvas FillRectangle call targets (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `PaintCanvas::FillRectangle` at `0x79289` calls
`PaintCanvas::SetWorldViewMatrix` at `0x7931a`, `Engine::GlEnable`
at `0x79326` and `0x7933c`, and `MeshDraw` at `0x79330`. The recovered
source now calls those methods directly rather than three unresolved
`paintcanvas_ext_fr_*` names. Its 15-float local matrix view is passed
as `const Matrix&`, matching the called method's signature.

Native `gof2`, AArch64 static `gof2`, and **209/209** ARMv7 translation
units build. Focused ARM comparison stays **100% source-shape / 100%
strict fuzzy** over 69 instructions. It is still **not linked-exact or
byte-exact**; matching normalized instructions do not establish exact
relocated bytes. The full changed PaintCanvas unit compares **198**
symbols and retains exactly **92 linked-exact** and **23 byte-exact**
symbol sets, with zero gains or losses. Other source files are unchanged;
the previous **4,743**-pair corpus remains the latest full run.

With ARM64 ZIP and crypto dependencies included, the strict
whole-archive unresolved-name inventory falls **502 → 499** and the
JNI-reachable inventory **262 → 259**. Exactly the three `fr_` names
disappear, with no new names. Live rectangle rendering and the game
shared library remain unverified.
