# PlayerEgo local-matrix rotation calls (2026-11-06)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
loads `Globals::Canvas` through GOT relocation `0x2101f4` in both
`PlayerEgo::setRotation` and `PlayerEgo::rotate`. It calls
`PaintCanvas::TransformGetLocal(unsigned int)` at `0xa13a8` and
`0xa134a`, respectively, then calls
`AEMath::MatrixSetRotation(Matrix &, float, float, float)` at
`0xa13bc` and `0xa135e`. The latter returns a `Matrix` by value and
also updates the referenced local matrix.

The reconstructed ARM32 paths instead read two uninitialized local
pointer placeholders, called undefined `TransformGetLocal` and
`MatrixSetRotation` helpers, and could dereference a null pointer.
Both methods now use the confirmed `Globals::Canvas` and typed
matrix functions. The same source form is used on Windows and ARM64.

Focused ARMv7 verification compiles **211/211** translation units.
`setRotation` rises from **88.4% to 100%** source-shape and from
**54.0% to 100%** strict fuzzy. `rotate` rises from **94.7% to 100%**
source-shape and from **88.9% to 100%** strict fuzzy. Both gain
linked-exact. The raw unmatched-symbol inventory over 211 ARM32
objects falls **236 -> 234**, removing exactly the two helper names
with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.81%** mean source-shape, **86.84%** strict fuzzy,
**2,791 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No previous exact-match pair is lost; these two
functions are the linked-exact gains. Windows native and ARM64 static
builds pass, as does strict ARM64 shared linking. Device rotation
behavior and full ARM32 shared linking remain unverified.
