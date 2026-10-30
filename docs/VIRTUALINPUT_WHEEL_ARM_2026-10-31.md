# Virtual input wheel recovery (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`LowerMouseWheel` starts at `0x6a6a0`; `MouseWheel(float, float)` starts
at `0x6a7ac`. The original `LowerMouseWheel` decays the
three exported globals `Globals::mouse_wheel`, `mouse_wheelX`, and
`mouse_wheelY` independently by **0.95**, truncating toward zero and
clamping a crossed sign back to zero. The old source used three
anonymous local accumulators and **0.78**, so the game-visible globals
never changed.

Original `MouseWheel` compares the first float against ±4 and calls
`ApplicationManager::GetCurrentApplicationModule` through
`Globals::appManager` for either exceeded threshold. In module 2 it
sets `Globals::keyboard_swipe` to +1 or -1. It then computes
`int((delta + residual) + float(wheelStack))` into the exported global
`wheelStack` (ELF symbol `0x217988`). The previous source used a
separate private zoom request, the manager singleton, and a different
float addition order. `simulateTouch` also references `wheelStack` in
the original at `0x6ac02–0x6ac42`; the recovered source now shares
the same accumulator between the input and touch paths.

Before the change, focused `MouseWheel` was **15.4% source-shape /
9.1% strict fuzzy**, and `LowerMouseWheel` was **45.8% / 18.8%**.
Both now reach **100.0% / 100.0%**; `MouseWheel` becomes linked-exact
but remains non-byte-exact. The focused 20-symbol unit rises from
**74.6% / 59.9%** to **81.6% / 68.5%** and from eight to nine
linked-exact matches, with two byte-exact matches unchanged.
Native Windows and ARM64 static builds pass; ARMv7 compiles all 209
translation units. Live input behavior remains unverified while device
testing is deferred. The separate private `g_zoomRequest` reset in
`simulateTouch` remains unexplained and is not claimed as recovered.

The full ARM corpus compares **4,323/4,524** originals over **4,749**
pairs. Mean source-shape rises **95.05% → 95.08%**; strict fuzzy rises
**84.72% → 84.76%**. There are **2,674 linked-exact**, **1,091
byte-exact**, zero skipped, and 201 missing. Complete `(unit, symbol)`
exact-set comparison finds one new linked-exact match (`MouseWheel`)
and no losses. Verifier outputs are stored outside Git under
`D:\gof2-arm64-crypto`.
