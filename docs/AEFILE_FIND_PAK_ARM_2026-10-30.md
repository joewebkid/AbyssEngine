# AEFile archive lookup recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::findPakFile` occupies `0x7075c–0x7089a`. It compares the requested
CRC with sorted `AEPakFileEntry` CRCs. On the lower half, ARM assigns the
midpoint as the new lower bound and stops when the half-width is zero. On
the upper half, it assigns the midpoint as the upper bound. The recovered
source now follows those observed bounds and termination conditions.

For a matching entry, ARM converts its name with `String::operator
unsigned short*()`, then constructs a temporary wide `String` directly
for the virtual `OpenRead` call. A raw entry (`size == 0xffffffff`) passes
`false, 0, 0, offset`; a packed entry passes `true, packedSize, size,
offset`. The temporary is destroyed before the `AEPakFile` allocation.
The source now preserves this branch-specific lifetime and call contract.

The original then stores `packedSize` and `size` at `AEPakFile` offsets
`+8` and `+12`. The previous source passed `offset` as the third
constructor argument, so the object held an archive offset where the
raw-size field belongs. The field and argument are now named `rawSize`,
and `findPakFile` passes `entry->size`. This preserves the 32-bit layout;
the constructor's existing exact ARM shape is unchanged.

Focused ARM source-shape / strict fuzzy for `findPakFile` improves
**73.9% / 25.5% → 84.6% / 39.3%**. The function remains nonexact. A
large part of the residual instruction difference is the original's
inlined `AEPakFile` field initialization versus the out-of-line C++
constructor call; changing the separately exact constructor solely to
alter this call site was rejected. The 61-symbol `AEFile` unit retains
its 11 linked-exact and one byte-exact pairs.

The existing production-code synthetic PAK gate passes at `-O0/-O2`.
It now also opens an entry directly through `findPakFile` and checks
`sizeLimit` and the `rawSize` sentinel, then releases the stream. Native
`gof2` and ARM64 static builds pass; ARMv7 compiles 209/209 units.

The full corpus compares **4,323/4,524** original functions over **4,748**
pairs: **94.99%** mean source-shape, **84.67%** strict fuzzy, **2,664**
linked-exact and **1,088** byte-exact, zero skipped and 201 missing. Against
the preceding full report, the complete `(unit, symbol)` exact sets have
no gains or losses, including both `AEPakFile` constructor aliases. The
reports and host-test binaries are stored outside Git in
`D:\gof2-arm64-crypto`.

The shipped APK/OBB have no standalone `.pak` entries in the local asset
set. Packed-entry decoding and live Android archive lookup therefore
remain unverified. Next offline priority: `AEFile::Open`, which calls this
lookup after a loose-file miss.
