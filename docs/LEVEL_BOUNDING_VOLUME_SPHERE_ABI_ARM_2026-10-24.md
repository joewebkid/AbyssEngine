# Level collision-volume scaling and BoundingSphere ABI (2026-10-24)

Primary source: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Level::getBoundingVolume(int,AEGeometry*)` at `0xc3e48` loads station
collision records below ID 2000 and static records otherwise. For box
records it scales a three-float size vector by `1.2f` through
`Vector::operator*=(float)` before doubling each component for
`BoundingAAB`; for spheres it scales the radius vector by `0.6f`
and multiplies by `-1.0f` when negative. The prior C++ collapsed box
scaling to `2.4f * value` and sphere scaling to a scalar expression.
The recovered vector operations preserve the original calculation and
rounding order. Unknown shape records advance one word and leave their
zero-initialized output slot untouched. The loop bound is signed, as in
the ARM comparison. The adjacent `Globals::getWreckCollision` implementation
independently corroborates the vector/record pattern, with its own factors.

The same ARM function allocates **0x48 bytes** for `BoundingSphere` at
`0xc3f2e`. Its constructor at `0x151d28` writes `radius` at object offset
**0x38**, whereas the previous reconstruction placed it at `+0x20`
and allocated a smaller object. Two opaque regions now preserve the
confirmed gap and full allocation size without assigning guessed
semantics. ARM-only layout assertions enforce the radius offset and size.
The `Level::createAsteroids` manual allocation now uses
`sizeof(BoundingSphere)`, which is still 0x48 on ARM and avoids
under-allocation on the x64 native host.

Focused NDK r18b ARM `-Oz` comparison for `getBoundingVolume` moves from
**76.2% source-shape / 28.4% strict fuzzy** to **94.2% / 59.6%**;
the function remains nonexact. Both `BoundingSphere` constructor aliases
become **100% / 100% linked-exact** after the layout correction, but
neither is byte-exact. Native `gof2` and all 208 ARM translation units
build. The 93 verifier tests pass (one skip). The full corpus compares
**4,323/4,524** original functions across **4,736** pairs, with mean
**94.14% source-shape / 83.48% strict fuzzy**, **2,562 linked-exact**
and **1,082 byte-exact** pairs. Relative to the preceding sentry-gun
baseline, there are two linked-exact gains, no linked- or byte-exact
losses, and **201** missing originals remain.

The original-asset station chain passes at host `-O0` and `-O2` after
the layout change, constructing 44 station collision volumes including
21 spheres and checking representative sphere center/radius and collision
queries. The same gate also executes the extracted production
`Level::getBoundingVolume` body on the original `collision.bin` and
`static_collisions.bin`: IDs 1002 and 2000 yield respectively **12/12**
and **26/26** non-null shapes at both host optimization levels. Independent
checks cover the first sphere's remapped center and scaled absolute radius,
the first box's remapped center and scaled half-extents, and balanced file
opens/closes. The host file/render boundaries and the remaining stage-zero
chain are unchanged. Unknown collision record types and a live Android
scene remain unverified. The next runtime priority is still the
first-friend Pan ship-insertion gate when ADB is available.
