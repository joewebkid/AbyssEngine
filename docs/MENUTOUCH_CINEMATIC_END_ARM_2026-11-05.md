# Cinematic touch release in the menu (2026-11-05)

The original Android ARMv7 `MenuTouchWindow::OnTouchEnd(int, int,
void*)` begins at `0x12aabc` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
At entry it clears `pendingActivate` at `+0x01` and `dragging` at
`+0x224`, then checks the layout choice-window path before dispatching
on `menuState` at `+0x16c`. It does not inspect the incoming touch ID
at entry.

The jump-table entry for state `0xb` leads to `0x12b74a`. That path
checks `Globals::iPad` (GOT `0x21032c`) and
`Globals::iPadAssetsWithLowerRes` (GOT `0x2103f4`) before the touch-ID
logic. At `0x12b768`, a null ID skips both comparisons; a non-null
ID is compared with fields `+0x08` and `+0x0c`, clearing either
matching slot. The halfword at `+0x98` is then cleared regardless of
the ID. These offsets correspond to `cinematicTouchIdA`,
`cinematicTouchIdB`, and `cinematicTouchState` in the reconstructed
class.

The previous source cleared the IDs and touch state at function entry
when `menuState == 0xb`, even if a choice window took the early return
or the iPad layout checks failed. The recovered source performs the
clear inside the cinematic branch after those guards, matching the
observed order and scope. Device behavior remains unverified.

Focused ARMv7 verification compiles **209/209** translation units.
Strict fuzzy rises **13.5% -> 14.2%**; source-shape changes
**88.4% -> 86.9%** because moving the block changes register and stack
lifetimes across this large switch. The function remains non-exact.
The change is retained for the confirmed branch behavior rather than
the source-shape metric. Windows native and ARM64 static builds pass,
as does strict ARM64 shared linking.

Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.79%**, **2,785 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. No comparison pair or
exact-match member was lost.
