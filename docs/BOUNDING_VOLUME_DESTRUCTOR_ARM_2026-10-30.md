# BoundingVolume child-release path from Android ARM (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BoundingVolume::~BoundingVolume` starts at `0x11f718`. If its child
array is present, the original calls
`ArrayReleaseClasses<BoundingVolume*>` at `0x11f72c`, then the array
destructor and `operator delete` at `0x11f734`–`0x11f738`, and finally
clears the child pointer. The PLT entries resolve to those functions
through the original relocations at `0x212bd0` and `0x212bd4`.

The reconstructed destructor previously looped over the array's
logical size, then called `ArrayRemoveAll`, which uses `realloc` on a
buffer allocated with `operator new[]`. That was an allocator mismatch
and differed from the original sequence. It now calls the already
recovered `ArrayReleaseClasses(*children)` and then deletes the array.
The helper visits the array's capacity, deletes non-null pointers,
releases the data buffer, and clears that pointer before the array
destructor runs. Original helper code is visible at `0xdc8bc`–`0xdc90a`.

Focused ARM comparison of both destructor aliases improves from
**70.3% to 91.3%** source-shape and **64.3% to 97.6%** strict fuzzy.
The remaining original instruction is an exception cleanup call after
the normal return; neither alias is exact. The 15-function
BoundingVolume unit retains all prior exact matches and gains a
linked-exact `ArrayReleaseClasses<BoundingVolume*>` instantiation.
ARMv7 compiles 209/209 translation units; native `gof2` and ARM64
static libraries build.

The full corpus compares **4,323/4,524** original functions over
**4,746** pairs: **94.92%** mean source-shape, **84.58%** strict fuzzy,
**2,662 linked-exact**, **1,088 byte-exact**, zero skipped and 201
missing. The pair count rises by one because this unit now instantiates
`ArrayReleaseClasses<BoundingVolume*>`. Against the preceding full
report, the complete `(unit, symbol)` exact sets have no losses and
gain that one linked-exact instantiation.

The original helper invokes the non-virtual base destructor when it
deletes a `BoundingVolume*`; the ARM64 compiler warns about this for
derived dynamic types. The same static call is present in the original
ARM helper, and changing the class's virtual ABI is outside this
bounded recovery. Runtime destruction of nested derived volumes has
not been verified on a device. Focused and full-corpus reports are
stored outside Git in `D:\gof2-arm64-crypto`.
