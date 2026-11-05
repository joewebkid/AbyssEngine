# Menu touch remaining ARM32 calls (2026-11-05)

Original Android ARMv7 evidence comes from
`android_2.0.16_libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
In `MenuTouchWindow::OnTouchEnd`, the calls at `0x12abaa`,
`0x12b6a6`, `0x12bfde`, and later sites target
`ApplicationManager::GetApplicationData` at PLT `0x61704` after
loading the application manager pointer. The leaderboard branch at
`0x12c034` calls `Globals::reportLeaderboards` at PLT `0x6528c`
after loading the globals pointer. Neither path guards a null owner
pointer. The ARM32 reconstruction now makes those typed calls
directly; ARM64 retains its existing null guards.

The original `MenuTouchWindow::render3D` at `0x12cfe4` checks menu
state 9 and a non-null missions window, then tail-branches from
`0x12cff4` to the Thumb/ARM veneer at `0x19c408`. That veneer loads a
PC-relative delta and reaches PLT `0x65688`, which is
`MissionsWindow::render3D`. The reconstruction now calls that method
directly on both pointer widths.

ARMv7 compilation passes **209/209** translation units. Focused
`MenuTouchWindow` comparison covers **48** symbol pairs with **21
linked-exact** and **6 byte-exact**. `OnTouchEnd` source-shape rises
**86.9% -> 87.0%** and strict fuzzy **14.2% -> 14.5%**; it remains
non-exact. `render3D` stays **100%** source-shape and linked-exact.
The earlier linked-exact result for `render3D` did not reveal its
undefined helper target, illustrating why the ARM32 undefined-symbol
check is a separate gate. `MenuTouchWindow.o` now has no undefined
`_mtw_` references. This does not establish full ARM32 shared-link
completeness.

Windows native and ARM64 static builds pass, as does strict ARM64
shared linking. Full ARMv7 corpus: **4,323/4,524** original functions
compared in **4,770** pairs; mean source-shape **96.79%**, strict fuzzy
**86.79%**, **2,785 linked-exact**, **1,108 byte-exact**, 201 missing,
and zero skipped or wrong-type functions. Comparison with the preceding
full report found no lost linked-exact or byte-exact members. The
extra-symbol count rises from 148 to 149 because a new internal typed
helper is emitted; it is defined and does not add an undefined ARM32
reference. Device behavior remains unverified.
