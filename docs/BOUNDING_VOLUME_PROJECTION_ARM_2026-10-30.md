# BoundingVolume projection-vector ARM recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BoundingVolume::getProjectionVector(const Vector&)` spans
`0x11f788`–`0x11f7d2`. At `0x11f798`, the original passes the center
fields at object offset `+8` directly to `Vector::operator-`, with
the incoming vector as the other operand. The subtraction writes to
the return object. At `0x11f7ac`–`0x11f7ba`, the result is normalized
into a temporary Vector and then copied back through
`Vector::operator=`. The original PLT relocations resolve to these
three functions at `0x210800`, `0x210818`, and `0x2107ac`.

The old C++ first copied the center into a temporary Vector and
returned `VectorNormalize(v - center)` directly. That allowed the
compiler to omit the final assignment and changed the call and stack
shape. The recovered source uses the center fields as a Vector view,
then assigns the normalized value to a named result. This follows the
same Vector-field convention already used elsewhere in the project.

Focused ARM source-shape improves from **85.2% to 100.0%** and strict
fuzzy from **66.7% to 100.0%**. The function becomes linked-exact but
is not byte-exact. The 14-function BoundingVolume unit rises from
seven to eight linked-exact, retains four byte-exact, and loses no
exact-symbol match. ARMv7 compiles 209/209 translation units; native
`gof2` and ARM64 static libraries build.

The full corpus compares **4,323/4,524** original functions over
**4,745** pairs: **94.91%** mean source-shape, **84.56%** strict fuzzy,
**2,661 linked-exact**, **1,088 byte-exact**, zero skipped and 201
missing. Against the preceding full report, the complete `(unit,
symbol)` exact sets have no losses; this projection function is the
sole new linked-exact match.

Live projection behavior remains unverified on a device. Focused and
full-corpus reports are stored outside Git in `D:\gof2-arm64-crypto`.
