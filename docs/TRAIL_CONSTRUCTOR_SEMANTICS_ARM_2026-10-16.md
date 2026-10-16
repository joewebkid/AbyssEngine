# Trail constructor behavior (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Both `Trail::Trail(int,int)` constructor aliases start at `0x15bd58`.
The original ARM body zeros the source coordinates, stores the segment
count, sets width to `60`, and calls `PaintCanvas::MeshCreate` with
`2 * segments + 2` vertices, `2 * segments` triangles, type `0x13`,
and material `0x4e77`. Its triangle loop begins at vertex `2` and
emits two triangles for each step below `2 * segments`. It writes UV
pairs for vertices `0` through `2 * segments + 1`; the first coordinate
is `1.0` for types `9`–`11` and `0.0` in the tested default path. The
second coordinate is derived from a literal `0.0` loaded at `0x15beec`,
so it remains zero even though the original computes a division and
multiplication. Finally it allocates two arrays of
`(2 * segments + 2) * 3` integers, creates a transform, attaches the
mesh, and calls `changeType(type)`.

The existing production C++ reproduces these observed steps; it was
not modified. `tools/run_trail_canvas_calls_smoke.py` now extracts the
constructor with the already-tested `changeType`, `render`, `update`,
and `setWidth` bodies. Under both host `-O0` and `-O2`, a recording
canvas checks the `(type=9, segments=2)` case: counts and arguments,
both triangle calls, six UV calls, allocations, transform attachment,
and green color. A `(type=4, segments=0)` case checks the no-triangle
boundary, two zero-UV calls and default color. The test releases both
allocated arrays after each case.

Native `gof2`, 207/207 ARM translation units, and 40 verifier tests
(one existing skip) pass. With unchanged production C++, focused Trail
verification remains **98.76% source-shape / 88.72% strict fuzzy**,
**7 linked-exact / 1 byte-exact** among twelve functions. The prior
full report remains **4,323/4,524** compared, **93.66%/82.73%**
average source-shape/strict fuzzy, **2,509 linked-exact / 1,074
byte-exact**, without exact-set changes. Reports:
`A:/Galaxy on Fire new version/_work/trail_ctor_semantics_focused_20261016.json`
and `A:/Galaxy on Fire new version/_work/trail_canvas_full_20261016.json`.

This is a bounded host call-contract and allocation check, not a live
ARM32/GPU rendering test. Larger segment counts, allocation failure,
and every possible type value were not exercised. The constructor's
remaining machine-code mismatch is not evidence of a behavior error.
