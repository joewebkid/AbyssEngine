# AEGeometry construction and LOD call contracts (Android ARM)

The primary reference is `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This package follows the preceding AEGeometry canvas-call audit and changes
only five production method bodies (seven mangled C1/C2/function symbols).

Original ARM observations:

- `AEGeometry::setMatrix` at `0xa4624` passes the canvas and current
  transform to `PaintCanvas::TransformSetLocal(unsigned int, Matrix const&)`.
- `setLodChildMeshes` at `0xa4a98` calls `PaintCanvas::TransformCreate`,
  `TransformAddMesh`, `TransformAddChild`, `TransformRemoveChild`, then
  `TransformAddChild` for each LOD child.
- `setLodChildTransform` at `0xa4b3c` calls
  `PaintCanvas::TransformAddChild(unsigned int, unsigned int)`.
- The canvas-only constructor at `0xa4468` calls
  `PaintCanvas::TransformCreate(unsigned int&)` for `baseTransform`.
- The mesh constructor at `0xa4398` creates `baseTransform`, creates and
  attaches the mesh, then calls `PaintCanvas::TransformGetLocal` with the
  created `baseTransform` in `r1` at `0xa443a`. The previous recovered code
  passed literal `0` and truncated the returned pointer to `uint32_t`.

Eight declared-only `AEGeomCanvas` call routes are now calls to the existing
`PaintCanvas` methods. The mesh constructor reads its reference matrix from
the newly created transform, not index zero. Its host-side pointer remains a
pointer instead of being narrowed to 32 bits. The production LOD loops,
allocation and other transform mathematics were not otherwise changed.

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` compiles 13 production bodies in a recording
host fixture. New cases check both constructors, `setMatrix`, both LOD-child
methods and, in particular, a created transform index of `8` rather than `0`.
The native `gof2` target, **206/206** Android ARM translation units and **40**
verifier tests (one existing skip) pass.

Focused `AEGeometry` verification compares **45** functions: **23
linked-exact**, **8 byte-exact**. The mesh constructor C1/C2 strict fuzzy
score changes **45.6% -> 52.7%**, with source-shape **68.8% -> 68.9%**.
The other changed methods' scores are unchanged, including linked-exact
`setMatrix`. No exact match is claimed for either constructor or the LOD
methods. Full comparison remains **4,323/4,524** unique original functions,
**201 missing**, **123 extras**, zero skipped, **93.59% source-shape** and
**82.64% strict fuzzy** average, **2,503 linked-exact** and **1,074
byte-exact**. Exact `(unit,symbol)` pair sets have zero losses and gains
against the preceding report. The full direct-call audit remains **2,372
matches / 53 mismatch candidates / 647 unresolved**; its scope is direct
calls in linked-exact functions, so it does not establish the nonexact
constructors' targets. Those were checked against the original ARM call sites
and the compiled object's relocations separately.

Reports: `A:/Galaxy on Fire new version/_work/aegeometry_canvas_calls_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/aegeometry_ctor_lod_after_unit_20261016.json`,
`A:/Galaxy on Fire new version/_work/aegeometry_ctor_lod_full_20261016.json`,
and `A:/Galaxy on Fire new version/_work/aegeometry_ctor_lod_full_audit_20261016.json`.

Fourteen `AEGeomCanvas` references remain, notably in rotation, scaling,
translation, direction and `updateLod`. Several still narrow host pointers to
32 bits, and some cast transform IDs as matrix pointers. The next package
should recover a small related group from original argument registers and
matrix semantics before replacing those calls. The host fixture is not a
live Android ARM32/GL runtime test. iOS/PC analogues were not checked for this
package, nor were the deeper `PaintCanvas` transform implementations proved
behaviorally equivalent.
