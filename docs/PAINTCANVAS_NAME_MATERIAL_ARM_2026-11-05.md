# PaintCanvas resource-name and mesh-material targets (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `String::Compare(const char *)` at PLT `0x5fa3c` from
`PaintCanvas::GetMeshResourceId(String &, unsigned short)` at
`0x77fdc`. It tail-branches from
`MeshChangeResourceMaterial` at `0x7a53a` and
`MeshChangeMaterial` at `0x7a55e` through veneer `0x19b268` to
`MeshChangeMaterialIntern(Mesh *, Material *)` at PLT `0x5fc64`.

The reconstructed ARM32 methods called undefined
`paintcanvas_ext_strcmp` and `paintcanvas_ext_change_mat` helpers at
these sites. They now use the typed `String::Compare` and
`MeshChangeMaterialIntern` methods. The latter method's unchanged
body moved to `PaintCanvasMeshMaterial.cpp` to keep the two original
external tail-branch relocations. A direct same-file call retained
100% normalized instructions but lost their linked-exact memberships.
The old helper for name comparison remains elsewhere in `PaintCanvas`
and is not claimed resolved by this change.

Focused ARMv7 verification compiles **211/211** translation units.
The two material paths, the name lookup, and the moved method each
retain **100%** source-shape, **100%** strict fuzzy and linked-exact
membership. The raw unmatched-symbol inventory over 211 ARM32
objects falls **247 -> 246**, removing only
`_Z26paintcanvas_ext_change_matPvS_S_` with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match symbol is gained or lost relative
to the preceding report. The moved method changes its `(unit, symbol)`
pair but remains linked-exact by symbol. Windows native and ARM64
static builds pass, as does strict ARM64 shared linking. Device
behavior and full ARM32 shared linking remain unverified.
