# Main-menu touch forwarding and missing-symbol audit (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `ModMainMenu::OnTouchMove(int, int, void*)` at `0x175a98`, the
`0x65118` PLT call resolves to `MenuTouchWindow::OnTouchMove(int, int,
void*)`. The caller changes `r0` to its touch window, leaving the
incoming `r1`, `r2`, and `r3` intact. Thus the original forwards the
touch pointer in `r3`. The previous source explicitly passed null.

The non-logo branch of `ModMainMenu::OnTouchEnd(int, int, void*)` at
`0x175ad0` similarly calls PLT `0x65238`, resolved to
`MenuTouchWindow::OnTouchEnd(int, int, void*)`, without changing `r3`.
The source now forwards the incoming touch pointer in both methods.
`OnTouchBegin` already forwards it and needs no behavioral edit.
The receiver compares this pointer with cinematic touch IDs during
movement and clears matching IDs on touch end, so passing null had
changed observable gesture behavior.

The current full ARM verifier's `missing.txt` has **201** symbols.
All are dependency symbols: **57** C++ ABI/runtime, **23**
OpenSSL/ARM crypto, **47** zlib, and **74** libzip. These categories
account for all 201 names; none is a game-class method. The coverage
gap should therefore be handled by a dependency/toolchain fidelity
pass, not mistaken for 201 missing gameplay methods.
This inventory does not measure unresolved references introduced in
our ARM32 objects: `MenuTouchWindow.o` still references the undefined
`_mtw_steerFromTouchId` helper. ARM32 link completeness therefore
requires a separate audit.

Focused ARMv7 verification compiles **209/209** translation units.
`OnTouchEnd` source-shape rises **98.1% -> 100%** and strict fuzzy
**97.6% -> 100%**; it becomes linked-exact. `OnTouchMove`
source-shape rises **35.0% -> 37.0%** and strict fuzzy **12.5% ->
37.5%**. It still uses a compiler-generated tail call where the
original uses a call and epilogue, so it is not exact. Windows native
and ARM64 static builds pass, as does strict ARM64 shared linking.
Touch behavior has not been verified on a device.

Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.79%**, **2,785 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. There is no lost comparison
pair or exact-match member, and `OnTouchEnd` is the sole new linked-exact
member.
