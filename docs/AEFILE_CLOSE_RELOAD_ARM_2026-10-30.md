# AEFile close slot reload (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::Close(uint32_t)` occupies `0x70610–0x7064e`. At `0x70630` it
loads the selected file, invokes its virtual deleting destructor if
non-null at `0x7063a`, then reloads the global file array and its data
pointer at `0x7063c–0x70640` before clearing the slot at `0x7064c`.
The null-file path clears the slot through its already loaded data
pointer.

The previous C++ held a reference to the array slot across `delete`.
That reference can become stale if the destructor causes array storage
to move. The source now retains the file pointer for deletion and
fetches the slot again afterward. This follows the original's order;
it does not imply that arbitrary changes to the array size during
destruction are safe.

Focused ARM source-shape / strict fuzzy improves **82.1% / 57.7% →
91.2% / 73.7%**. The function remains nonexact. The remaining visible
instruction difference includes how the build addresses the two
static globals; no unrelated global layout was changed for this
metric. ARMv7 compiles 209/209 units; native `gof2` and ARM64 static
builds pass. The synthetic production-code PAK test passes at
`-O0/-O2` and now checks repeated close on numbered and zero slots.
It does not induce a storage relocation during destruction, and live
Android ownership remains unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,748** pairs: **95.01%** mean source-shape, **84.69%** strict fuzzy,
**2,664 linked-exact** and **1,088 byte-exact**, zero skipped and 201
missing. Both complete `(unit, symbol)` exact sets have no losses or
gains relative to the preceding full report. The corpus means remain
unchanged at displayed precision. The verifier reports are stored
outside Git in `D:\gof2-arm64-crypto`.
