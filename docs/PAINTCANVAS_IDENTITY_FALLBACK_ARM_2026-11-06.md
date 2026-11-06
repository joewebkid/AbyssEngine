# PaintCanvas identity fallback calls (2026-11-06)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `AEMath::MatrixIdentity(Matrix&)` for an invalid transform index
in `PaintCanvas::TransformGetLocal` at `0x7b3f6`. The corresponding
camera-index fallback in `PaintCanvas::CameraGetLocal` calls the same
function at `0x7b7d2`. Both use `identityMatrix` at offset `0xf8`.

The ARM32 source already had the right instruction shape but called an
undefined free `MatrixIdentity(void*, void*)` placeholder. Both paths
now call the typed `AEMath` function, matching the original relocation
and the existing ARM64 source form. The unused placeholder declaration
was removed.

ARMv7 compiles **211/211** translation units. Both affected functions
remain **100%** source-shape, **100%** strict fuzzy and linked-exact.
The raw unmatched-symbol inventory across the 211 ARM32 objects falls
**234 -> 233**, removing only `_Z14MatrixIdentityPvS_`. Windows native
and ARM64 static builds pass, as does strict ARM64 shared linking.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.81%** mean source-shape, **86.84%** strict fuzzy,
**2,791 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. Exact-match membership is unchanged. Device
behavior for invalid indices and full ARM32 shared linking remain
unverified.
