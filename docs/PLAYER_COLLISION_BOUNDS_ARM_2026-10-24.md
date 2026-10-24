# Asteroid and far-static collision bounds (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PlayerAsteroid::collide(float,float,float)` at `0xe3104` first checks
hitpoints, calls `AEGeometry::getPosition()` three times (one coordinate
from each result), then reads the `Player` radius field directly. Each
axis must satisfy the open interval `-radius < delta < radius`.
`PlayerStaticFar::collide` at `0x11c460` uses the same open interval, but
subtracts the signed integer `PlayerStatic::positionX/Y/Z` fields at
offsets `0x124/0x128/0x12c`, not the float `KIPlayer::posX/Y/Z` fields.

The former C++ used one asteroid position query and a getter for radius;
the far-static method used the float position fields. Both also rejected
points via `delta >= radius || delta <= -radius`, which lets NaN deltas
fall through as inside. The recovered checks explicitly require both
strict inequalities, so NaN is outside as in the ARM branches. The
far-static coordinate correction also matters when the integer and float
position fields diverge. Finite points on the radius boundary remain
outside.

Focused NDK r18b ARM `-Oz` source-shape/strict fuzzy moves:

| Function | Before | After |
| --- | ---: | ---: |
| `PlayerAsteroid::collide` | 67.0% / 55.4% | 94.1% / 86.1% |
| `PlayerStaticFar::collide` | 69.4% / 45.2% | 91.0% / 71.3% |

Both functions remain nonexact. A host smoke extracts the two production
bodies and checks NaN on every axis, the open boundary, far-static integer
position precedence, hitpoint guard, and three asteroid geometry queries
at both `-O0` and `-O2`. The separate original-asset station gate also
executes the production far-static method on a live constructed station
and checks NaN rejection and integer-position precedence at both host
optimization levels. These are bounded host checks, not a live Android
collision scene. `BoundingVolume::update` was also reviewed at `0x11f7e4`:
its existing child-first order is correct, and its low match primarily
reflects the unresolved scalar-field versus vector-member representation;
no speculative base-class ABI change was made.

Native `gof2` and all 208 ARM translation units build; 93 verifier tests
pass (one skip). The full corpus compares **4,323/4,524** originals
across **4,736** pairs, with mean **94.17% source-shape / 83.50% strict
fuzzy**, **2,562 linked-exact**, **1,082 byte-exact**, **2,728
100%-fuzzy**, **201 missing** and zero skipped functions. Pairwise
comparison with the preceding AAB-projection baseline found no linked-
or byte-exact gains or losses. Live Android asteroid and far-static
collisions remain unverified. The next device-runtime priority is the
first-friend Pan ship-insertion gate when ADB is available; the next
source candidate is `PlayerStaticFar::update` after an ARM behavior review.
