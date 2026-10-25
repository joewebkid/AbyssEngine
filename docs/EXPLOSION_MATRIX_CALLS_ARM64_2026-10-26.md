# Explosion matrix and vector call recovery (2026-10-26)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `Explosion` functions used here are `start(Vector,Vector)`
at `0xa83b0`, `start(Matrix)` at `0xa853c`, `update(int,TargetFollowCamera*)`
at `0xa8668`, and `render()` at `0xa891c`.

Eight global math declarations in `Explosion.cpp` did not name real
linkable functions. The Android ARM calls instead target existing
`AbyssEngine::AEMath` functions. For example, `start(Vector,Vector)`
calls `AEMath::MatrixSetRotation(Matrix&,float,float,float)` at
`0xa8462`; the input address is `this->rotation` and the return object
is a stack temporary. `start(Matrix)` calls `AEMath::MatrixGetPosition`
at `0xa856c`, `0xa8584`, `0xa85ca`, and `0xa863a`. The camera overload
of `update` calls `MatrixGetPosition` at `0xa8752` and `VectorLength`
at `0xa8764`.

`render` uses `MatrixSetTranslation` at `0xa89a2` or
`MatrixGetPosition`, `MatrixGetUp`, and `MatrixGetLookAt` at
`0xa89b2–0xa89ce`. The returned look-at matrix is assigned to the
camera matrix at `0xa89d6`. `MatrixSetScaling` at `0xa89ec` receives
that camera matrix as a mutable input; the subsequent rotation and
`AEGeometry::setMatrix` also use it. The recovered code now preserves
this data flow and uses the real return structures and mutation
semantics. Direct initialization of returned vectors and matrices
avoids extra assignment calls absent in the original.

For explosion types 8–10, the original call at `0xa89fe` is
`Matrix::operator*=` on the camera matrix and stored rotation. The
recovered source instead called `AEMath::MatrixMultiply`, which
computed a temporary and discarded it, leaving the explosion
unrotated. It now uses `cameraLocal *= this->rotation`; the rebuilt
ARMv7 object relocates directly to `Matrix::operator*=`.

Focused source-shape/strict fuzzy changes are:

| Function | Before | After |
| --- | --- | --- |
| `start(Vector,Vector)` | 91.7% / 59.1% | 93.9% / 59.0% |
| `start(Matrix)` | 93.0% / 46.3% | 93.4% / 45.9% |
| `update(int,TargetFollowCamera*)` | 97.6% / 61.6% | 97.6% / 61.6% |
| `render()` | 92.5% / 54.9% | 94.7% / 62.6% |

The focused unit compares **20** functions, with **6 linked-exact**
and **3 byte-exact**; none of the edited functions is exact. Native,
AArch64 static, and **209/209** ARMv7 translation units build. The
strict AArch64 whole-archive link probe falls from **555** to **548**
unique unresolved names, with no new names. It still fails; no
game `.so` or APK was produced. One of the eight removed placeholder
names, `MatrixGetDir`, remains unresolved at another call site.

The complete ARM corpus compares **4,743** symbol pairs and covers
**4,323/4,524** original functions, with **201** missing and no
skipped comparisons. Mean source-shape is **94.71%**, strict fuzzy
**84.21%**, linked-exact **2,601**, and byte-exact **1,083**. Both
exact-symbol sets are identical to the previous PaintCanvas baseline:
zero gains and zero losses.

Live explosion animation, camera rumble, and audio have not been
verified on Android. The next priority is another original-backed
call-closure package and then a runnable Android host for those paths.
In particular, `BeamGun::update` still uses the unresolved
`MatrixGetDir` placeholder. Five other callers of the no-op
`AEMath::MatrixMultiply` helper also need their original call targets
checked before changing that shared helper.
