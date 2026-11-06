# Mesh merger ARM EABI call targets (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Its dynamic symbol table imports `__aeabi_uidiv` and
`__aeabi_memcpy4`; their PLT entries are `0x5ec2c` and `0x5f6a0`.

The original `MeshMerger` constructor at `0x1737a0` calls
`__aeabi_uidiv` three times, and `MeshMerger::init` at `0x173ca6`
calls it once. `SimpleMeshMerger` construction at `0x180ecc` calls it
three times. `MeshMerger::update` at `0x173f2c` calls
`__aeabi_memcpy4` four times for vertex attribute buffers.

The reconstruction used undefined C++ placeholder names
`aeabi_uidiv16` and `aeabi_memcpy4`. On ARM32, their declarations now
bind directly to the confirmed EABI symbol names. This retains the
original call sequence, including all three divisions in each
constructor. Windows and ARM64 use ordinary unsigned division and
`memcpy` for the same inputs. The ARM32 object relocations confirm
the original seven `__aeabi_uidiv` and four `__aeabi_memcpy4` calls.

Focused verification compiles **211/211** ARMv7 translation units.
The affected `MeshMerger` and `SimpleMeshMerger` functions retain
their prior source-shape, strict fuzzy, and exact results. Windows
native and ARM64 static builds pass, as does strict ARM64 shared
linking. The raw 211-object ARM32 unmatched-symbol inventory falls
**227 -> 225**: exactly the two placeholder names disappear, with
no new unresolved name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
skipped, zero wrong-type, and 149 extra symbols. The exact-match
membership sets are unchanged.

Live merged-mesh rendering and full ARM32 shared linking remain
unverified. Next priority: trace remaining external `PaintCanvas`
placeholders to original ARM call targets, then close only confirmed
ABI or behavior gaps.
