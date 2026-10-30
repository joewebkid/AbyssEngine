# BoundingSphere vector call and field recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The three reviewed functions are `outerCollide` at `0x151da0`,
`getCollisionNormal` at `0x151e10`, and `projectCollisionOnSurface` at
`0x151e74`. In each, the ARM code passes the center and extent fields
at object offsets `+8` and `+20` directly to Vector addition. The old
C++ copied those fields into temporary Vectors first, adding stack
traffic and changing the source call shape. The reconstructed source
now uses the contiguous fields as Vector views.

`outerCollide` stores the incoming coordinates as a Vector before the
center addition, then subtracts and takes the dot product. Its source
now follows that order. `getCollisionNormal` subtracts the position
from the summed center, normalizes into a temporary, and assigns it to
the return Vector; the prior direct return omitted the assignment
call. `projectCollisionOnSurface` recomputes the center after the
radius check, as in ARM. Its radius condition and zero-vector result
were retained.

Focused ARM source-shape / strict fuzzy changes:

| Function | Before | After |
| --- | --- | --- |
| `outerCollide` | 88.5% / 44.7% | **100.0% / 88.1%** |
| `getCollisionNormal` | 83.8% / 36.8% | **97.8% / 81.1%** |
| `projectCollisionOnSurface` | 88.7% / 43.6% | **99.0% / 65.6%** |

None of the three is linked-exact or byte-exact. The remaining
differences are register and stack-slot selection and instruction
scheduling; no additional arithmetic change is supported by the
evidence. The nine-function BoundingSphere unit retains all six
linked-exact matches and has no exact-symbol loss. ARMv7 compiles
209/209 translation units; native `gof2` and ARM64 static libraries
build.

The full corpus compares **4,323/4,524** original functions over
**4,746** pairs: **94.93%** mean source-shape, **84.60%** strict fuzzy,
**2,662 linked-exact**, **1,088 byte-exact**, zero skipped and 201
missing. Against the preceding full report, the complete `(unit,
symbol)` linked-exact and byte-exact sets have no gains or losses.

Live collision behavior on the phone remains unverified. Focused and
full-corpus reports are stored outside Git in `D:\gof2-arm64-crypto`.
