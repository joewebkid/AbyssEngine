# BoundingVolume::update ARM recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BoundingVolume::update(float, float, float)` starts at `0x11f7e4`.
Its loop at `0x11f812`–`0x11f838` checks `children`, visits each child
through virtual slot `+4`, then reloads `children` and its count after
every call. The old range-based loop captured the end of the array up
front. If a child update changes the child array, these forms can visit
different elements.

At `0x11f83a`–`0x11f84c`, the original builds a three-float Vector on
the stack and calls `AEMath::Vector::operator=` for the center fields
starting at object offset `+8`. The PLT target resolves to that operator
through the original relocation at `0x2107ac`. The old reconstruction
wrote three float fields directly. It now uses the Vector assignment,
preserving the original call and layout while retaining the existing
field names and offsets.

Focused ARM source-shape improves from **42.9% to 100.0%** and strict
fuzzy from **28.9% to 93.9%**, with 49 instructions on each side. The
remaining difference is register-move scheduling before the loop; the
function is not linked-exact or byte-exact. The 14-function
BoundingVolume unit retains all six linked-exact and three byte-exact
matches without a loss. ARMv7 compiles 209/209 translation units;
native `gof2` and ARM64 static libraries build.

The full corpus compares **4,323/4,524** original functions over
**4,745** pairs: **94.90%** mean source-shape, **84.54%** strict fuzzy,
**2,659 linked-exact**, **1,087 byte-exact**, zero skipped and 201
missing. Against the preceding full report, the complete linked-exact
and byte-exact `(unit, symbol)` sets have no gains or losses.

Behavior when child updates mutate the array has not been exercised
against a live original game. The ARM instruction sequence establishes
the loop's reload behavior, but runtime effects remain unverified.
Focused and full-corpus reports are stored outside Git in
`D:\gof2-arm64-crypto`.
