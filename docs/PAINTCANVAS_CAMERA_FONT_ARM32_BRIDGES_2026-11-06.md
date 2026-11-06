# PaintCanvas camera and font ARM32 call closure (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers three already linked-exact callers that still had
undefined target names in the ARM32 object graph.

`PaintCanvas::CameraSetCurrent` at `0x7b7ec` loads the selected
camera's first three 32-bit fields into `r1`–`r3` and tail-branches
through veneer `0x19b2c8` to the
`PaintCanvas::SetProjectionMatrix3d(float, float, float)` PLT entry
`0x5fd90`. Those words are the raw bits of the camera's float FOV,
near plane, and far plane. The existing ARM32 caller used an undefined
raw-word target to retain its exact instruction form. Its new
ARM32-only bridge reconstructs those floats bit-for-bit and calls
`SetProjectionMatrix3d`. The compiled bridge is itself a direct tail
branch to that method; no conversion instructions change the values.

`PaintCanvas::FontSetYOffset` at `0x79c48` tail-branches through
`0x19b218` to `ImageFontSetYOffset` PLT entry `0x5fbec`.
`FontGetYOffset` at `0x79c60` tail-branches through `0x19b228` to
`ImageFontGetYOffset` PLT entry `0x5fbf8`. Their ARM32-only bridges
call the typed functions. The compiled bridges inline the same
signed-short field access as those functions. The 64-bit path already
used typed calls and remains unchanged.

Focused ARMv7 verification compiles **211/211** translation units.
All three callers remain **100% source-shape**, **100% strict fuzzy**,
and linked-exact. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. The raw 211-object ARM32
unmatched-symbol inventory falls **195 -> 192** with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape **96.83%**, strict fuzzy
**86.91%**, **2,798 linked-exact**, **1,113 byte-exact**, 201 missing,
zero skipped, and zero wrong-type. Both complete exact-match
membership sets are unchanged. Extra symbols rise **149 -> 152**,
exactly the three named ARM32 bridges. They are implementation
scaffolding for call closure, not claims that the original binary
contained these names.

Live camera switching and font offsets on ARM32, as well as full
ARM32 shared linking, remain unverified. Next priority: inspect the
remaining gameplay-specific ARM32 placeholders and verify camera
behavior on a device when available without interrupting another
active device test.
