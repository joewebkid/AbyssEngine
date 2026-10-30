# AEFile release ownership and array cleanup (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::Release()` occupies `0x70254–0x702d8`. At `0x70266` it calls
`ArrayReleaseClasses<AELowLevelFile*>` on the open-file array, then
destroys the array and clears its global pointer. The helper itself is
a previously linked-exact symbol in the ARM corpus. The prior C++
manually deleted the visible files and called `ArrayRemoveAll`, which
changed array storage before deleting the array.

The original then walks the PAK-entry array by index. For a non-null
entry, it destroys the embedded `String` and deletes the allocation;
afterward it reloads the array's data pointer before clearing the
slot. The recovered source now uses an indexed loop with a fresh slot
lookup after `delete`. Both array global pointers are cleared after
their array objects are destroyed.

Focused ARM source-shape / strict fuzzy for `Release` improves
**85.7% / 51.4% → 93.3% / 68.0%**. `Release` remains nonexact. The
production-code synthetic PAK test passes at `-O0/-O2`: it now leaves
one file open for `Release` to close and calls `Release` twice. Native
`gof2` and ARM64 static builds pass; ARMv7 compiles 209/209 units.
The host test does not cover a reentrant PAK-entry destructor or live
Android file ownership; those remain unverified while device testing
is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs: **95.01%** mean source-shape, **84.70%** strict fuzzy,
**2,665 linked-exact** and **1,088 byte-exact**, zero skipped and 201
missing. The complete exact-symbol sets have no losses; the
`ArrayReleaseClasses<AELowLevelFile*>` pair in the `AEFile` unit adds
one linked-exact match. The helper was already linked-exact in its
definition unit. The verifier reports are stored outside Git in
`D:\gof2-arm64-crypto`.
