# Cinematic touch assignment in the menu (2026-11-05)

The original Android ARMv7 `MenuTouchWindow::OnTouchBegin(int, int,
void*)` begins at `0x129bd4` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The state `0xb` jump-table entry reaches `0x12a086`, where the code
checks `Globals::iPad` and `Globals::iPadAssetsWithLowerRes` before
handling the two cinematic controls. Incoming `r3` is retained as the
touch ID in `r5`; `r1` and `r2` are the two coordinates in `r8` and
`r9`.

At `0x12a0ba..0x12a102`, the original assigns the first ID only when
it is non-null, the first slot is empty, the first coordinate is at
most `0xd1`, the second slot does not hold that ID, and the second
coordinate falls strictly inside the steer-anchor interval. It then
sets state `1` and records the anchor. On a failed first test,
`0x12a288..0x12a314` assigns the second slot only when the ID is
non-null and differs from the first slot, the second slot is empty,
and the coordinates fall strictly inside the fire-anchor bounds.
It then sets state `0x100` and records the second anchor. Otherwise
it sets state `0` without assigning either slot.
The ARM reads of settings at `+0x54` and `+0x58` agree with the
compile-locked `GameSettings::steerAnchorX` and `fireAnchorX` offsets.

The previous source's `else` assigned a null ID to the first slot
and set state `1`; it could also assign the first slot's existing ID
to the second slot. The recovered conditions follow the original
branches and preserve the existing button calls. Device behavior
remains unverified.

Focused ARMv7 verification compiles **209/209** translation units.
Source-shape remains **94.7%** and strict fuzzy **28.9%**; this large
function remains non-exact. The unchanged rounded metrics do not
validate the old conditions: the branch tests above are confirmed by
the original instructions. Windows native and ARM64 static builds
pass, as does strict ARM64 shared linking. The recovered gesture path
has not been exercised on a device.

Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.79%**, **2,785 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. No comparison pair or
exact-match member was lost.

At the time of this audit, the ARM32 object for `MenuTouchWindow`
referenced `_mtw_steerFromTouchId` without a definition. This was a
separate link-completeness gap, not one of the 201 missing original
symbols. The subsequent [cinematic movement recovery](MENUTOUCH_CINEMATIC_MOVE_ARM_2026-11-05.md)
removes that reference.
