# Main-menu release and suspend globals (2026-11-05)

The original Android ARMv7 `ModMainMenu::OnRelease` at `0x175aec`
loads `Globals::Canvas` (`GOT 0x2101f4`), `Globals::globals`
(`0x210218`), `Globals::layout` (`0x210224`),
`Globals::imageFactory` (`0x21021c`), and `Globals::sound`
(`0x210168`). The relocation targets are identified in the original
ELF (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The release order is canvas resources, font, optional layout/image
factory reload and tip initialization, then optional sound-event
cleanup. `OnSuspend` at `0x17582c` loads
`Globals::recordHandler` (`GOT 0x210420`) and saves options if present.

The recovered source used unwritten pointer holders for the layout,
image factory, sound and record handler. Both functions now load the
observed global objects directly. Focused ARMv7 verification compiled
**209/209** translation units. `OnRelease` rises from **97.8%** to
**100%** source-shape, `OnSuspend` from **95.9%** to **100%**; both
are now linked-exact. Windows native and ARM64 static builds pass.
Device execution of this revision remains unverified.

Strict ARM64 shared linking passes with zero unresolved symbols and
53 JNI exports. Full ARMv7 verification compares **4,323/4,524**
original functions in **4,770** pairs: mean source-shape **96.79%**,
strict fuzzy **86.78%**, **2,784 linked-exact** (up from 2,782),
**1,108 byte-exact**, 201 missing, zero skipped, and zero wrong-type
matches. No comparison pair or exact-match member was lost.
