# ARM64 matrix dispatch in rendering and PlayerEgo (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `PlayerEgo::rotate` (`0xa12fc`) and `setRotation`
(`0xa137c`) obtain a local matrix from `Globals::Canvas` and apply
`AEMath::MatrixSetRotation`. The AArch64 recovery instead dereferenced
uninitialized matrix-provider globals. `PaintCanvas` fallback matrix
access used an opaque sret wrapper despite having the typed
`AEMath::MatrixIdentity` method.

On AArch64, rotation now gets the transform matrix from the typed
canvas and applies the recovered rotation method. `PaintCanvas`
fallbacks call typed `MatrixIdentity`. The turret-shoot path also
uses the existing 60-byte `Matrix` product: its former 48-byte
temporary was read as 60 bytes and followed by an out-of-bounds
`m[15]` store. ARMv7 call forms remain architecture guarded.

Native and AArch64 static builds pass; ARMv7 compiles **209/209**
translation units. Rebuilt ARMv7 `PlayerEgo.o` retains SHA-256
`a16742e307bc3b9c1f28ed1ba54f59ef3b08f78223bc7ed2a6280c47a51715b1`.
Scoped `PaintCanvas` verification retains all **94 linked-exact** and
**23 byte-exact** matches, with zero exact-set losses. The last full
ARM corpus remains **4,323/4,524** originals over **4,745** pairs,
**94.79% source-shape / 84.31% strict fuzzy**, **2,607 linked-exact /
1,083 byte-exact**, zero skips. Strict JNI-initialize AArch64
unresolved names fall **14 → 10** with no new names.

The typed matrix forms were accepted; the undersized turret buffer
and uninitialized provider handles were rejected. Live aiming and
rendering remain unverified. A game `.so`/APK is not yet built.
