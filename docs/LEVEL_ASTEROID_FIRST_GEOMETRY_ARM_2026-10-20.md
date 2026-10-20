# First asteroid geometry in the Pan Level stage-one path — 2026-10-20

This checkpoint extends the continuous original-asset Pan gate from the
`createAsteroids` prelude through `Waypoint`, `BoundingSphere`, the first
probability choice, asteroid position, original mesh construction, three LOD
meshes and distances, LOD manager registration, scale and quality selection.
The source is extracted from the production C++ at test time. The gate stops
just before `PlayerAsteroid` allocation; it does not construct the first
asteroid, finish the field, finish stage one, or render a playable frame.

The Android 2.0.16 original `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
allocates `Waypoint` with 0x138 bytes, `BoundingSphere` with 0x48 bytes and
`AEGeometry` with 0xc0 bytes in `Level::createAsteroids` at Thumb 0xaf6f8.
The restored `PlayerAsteroid` layout is 0x170 bytes on ARM. Fixed ARM-sized
`::operator new` calls under-allocated the host versions of `Waypoint`,
`AEGeometry` and `PlayerAsteroid`; allocation now uses `sizeof` for those
three, guarded by 32-bit layout assertions where applicable. The original
0x48 `BoundingSphere` allocation is retained because the reconstructed ARM
class is smaller than the original allocation; an assertion checks it stays
within that allocation, but its full original layout is not yet established.

The gate passes with Windows SwiftShader and on an Android ARMv7 Adreno 618
device at `-O0` and `-O2` against the original Pan asset bundle. It checks
the seeded field center and count, the
Waypoint coordinates, radius-100000 sphere, mesh 0x4204, LOD meshes
0x4205/0x4206/0x4207 and squared distances 60000²/100000²/120000². The
Galaxy probability provider remains a bounded test service producing the
first original-compatible pair `[0x9a, 100]`; full probability calculation
and other branches are not established by this gate. The gate also checks
scale and quality ranges, the position envelope, LOD registration, and
balanced file operations. The first Android attempt was interrupted when the
ADB connection disappeared; both modes passed after reconnection. Native
`gof2`, all 208 Android ARM translation units and 93 verifier tests pass
(one existing skip). Focused `Level::createAsteroids` remains nonexact at
89.4% source-shape and 47.3% strict fuzzy. The focused report SHA-256 is
`e91b303d5aa0dd6fbda5ca44f09206d5e4d2334a49b16a49ee4034f0b0ad2b4a`,
identical to the preceding source tree.

The complete ARM report is also byte-identical to the preceding one:
`asteroids-first-geometry-full.json` has SHA-256
`f73a5ab9b28f719d1b42bb1b3c410d225bc4acfd9acada3a1596500525886d2`.
It compares 4,323/4,524 original functions in 4,735 pairs, with 93.95%
mean source-shape, 83.24% mean strict fuzzy, 2,551 linked-exact and 1,079
byte-exact pairs, zero skipped units and no exact-pair losses or gains.

Next: extend the gate through the first `PlayerAsteroid` construction and
field completion. Confirm the `BoundingSphere` object layout separately
instead of treating its 0x48 allocation as proof of its `sizeof`.
