# Cinematic touch movement in the menu (2026-11-05)

The original Android ARMv7 `MenuTouchWindow::OnTouchMove(int, int,
void*)` begins at `0x12a33c` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
Its state `0xb` branch begins at `0x12a720`. It checks
`Globals::iPad` and `Globals::iPadAssetsWithLowerRes`, moves the two
touch buttons, then returns to the common update path for a null
touch ID.

For a non-null ID, `0x12a75a..0x12a76a` compares it with the slots
at `+0x08` and `+0x0c`. The first match takes the steer path at
`0x12a9e8`; the second takes the fire path at `0x12a76e`; unmatched
IDs skip both. The steer path calls `PaintCanvas::GetImage2DWidth`
for images at `+0x1c`, `+0x24`, and `+0x20`, calls
`Globals::setCoordsSteer` (PLT `0x664bc`), then stores the new anchor
at `+0x90`. The fire path queries the image at `+0x78`, calls
`Globals::setCoordsFire` (PLT `0x664c8`), then stores the anchor at
`+0x94`. Both calls receive the prior `GameSettings` anchor plus the
current coordinate minus the previous touch coordinate. The
`Globals::globals`, `Globals::Canvas`, and `Globals::options` GOT
entries are `0x210218`, `0x2101f4`, and `0x21016c`.

The reconstructed ARM32 branch called an undefined
`_mtw_steerFromTouchId` helper. The prior ARM64 branch updated both
control coordinate groups regardless of which ID moved. The shared
recovered branch now calls only the matching coordinate setter and
removes that unresolved ARM32 reference. Device behavior remains
unverified.

Focused ARMv7 verification compiles **209/209** translation units.
Focused comparison raises source-shape **87.7% ->
91.6%** and strict fuzzy **25.7% -> 32.8%**. The generated function
has 735 instructions against 706 in the original and is not exact.
The undefined `_mtw_steerFromTouchId` reference is absent from the
rebuilt ARMv7 `MenuTouchWindow.o`. Windows native and ARM64 static
builds pass, as does strict ARM64 shared linking. Other ARM32 helper
references still need a link-completeness audit.

Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.79%**, **2,785 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. No comparison pair or
exact-match member was lost. The same ARM32 object still has seven
other undefined `_mtw_` helper references; this patch does not prove
a complete ARM32 link.
