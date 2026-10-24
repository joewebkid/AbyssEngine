# BoundingSphere vector operations (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x151da0`, `BoundingSphere::outerCollide` calls `Vector::operator+`
on the center and extents vectors before subtracting the queried point
and comparing the squared distance with the squared radius. At `0x151e10`,
`getCollisionNormal` makes the same vector addition before subtraction
and normalization. At `0x151e74`, `projectCollisionOnSurface` makes that
addition both before the distance check and in the inside-radius branch;
the latter scales the delta through the `float * Vector` overload. The
existing reconstruction had component-wise center sums and `Vector * float`
in the projection. The new expressions restore those observed operations
without changing the intended finite-input geometry.

Focused NDK r18b ARM `-Oz` source-shape/strict fuzzy changes are:

| Function | Before | After |
| --- | ---: | ---: |
| `outerCollide` | 81.8% / 57.1% | 88.5% / 44.7% |
| `getCollisionNormal` | 73.1% / 37.3% | 83.8% / 36.8% |
| `projectCollisionOnSurface` | 76.4% / 31.9% | 88.7% / 43.6% |

The unweighted family mean moves from **77.1% to 87.0% source-shape**;
strict fuzzy moves from **42.1% to 41.7%**. None of these three functions
becomes exact. The strict-fuzzy decrease is consistent with the remaining
representation difference: the original passes vectors embedded in
`BoundingVolume` directly to the addition call, while the reconstructed
class still exposes six scalar fields and constructs temporary vectors.
Changing the base-class field representation would affect many callers and
has not been justified by this focused result alone.

The full corpus compares **4,323/4,524** originals across **4,736**
pairs, with mean **94.15% source-shape / 83.48% strict fuzzy**,
**2,562 linked-exact**, **1,082 byte-exact**, **2,728 100%-fuzzy**,
**201 missing** and no skipped functions. Pairwise comparison with the
preceding collision-volume baseline finds **zero** linked- or byte-exact
losses or gains. Native `gof2` and all 208 ARM translation units build;
93 verifier tests pass (one skip).

The original-asset host station gate runs at `-O0` and `-O2` and retains
its 44-volume station collision checks and direct Level parser checks for
12 station and 26 static shapes. A shifted sphere with nonzero base center
and extents additionally checks an inside point, the radius boundary,
projection, and normal through the production `BoundingSphere` methods.
This is host execution, not a live Android collision test.
The first-friend Pan ship-insertion gate remains the next device-runtime
priority; ADB currently lists no device.
