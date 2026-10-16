# AEGeometry rotation and scaling calls (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The `setRotation(float,float,float)` body at `0xa4784`,
`setScaling(float,float,float)` at `0xa4800`, and `rotate(float,float,float)`
at `0xa4880` each call `PaintCanvas::TransformGetLocal` twice, followed by
`AEMath::MatrixSetRotation(Matrix&,float,float,float,RotationOrder)` and
`AEMath::MatrixSetScaling(Matrix&,float,float,float)` in that order. The
original assembly also shows the expected argument registers, rotation
state updates and stack-buffer return convention for the two `Matrix`
return values.

The recovered bodies previously called declared-only `AEGeomCanvas` and
`_ae_MatrixSet*` transition hooks, narrowing the canvas and returned matrix
pointers to `uint32_t`. The three bodies now call the existing typed
`PaintCanvas` and `AEMath` methods and retain full pointer width. No
rotation or scaling formulas were modified in this package.

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` exercises sixteen production method bodies
with recording host fixtures. Its new cases check both transform lookups,
matrix-object identity, rotation order, incremental rotation, scaling
arguments, state updates and matrix writes. The math methods are fixtures
here; this is a call-contract test, not an independent numerical test of
the production `AEMath` implementations. Native `gof2`, **206/206** ARM
translation units and **40** verifier tests (one existing skip) pass.

Focused AEGeometry verification compares **45** functions. Each changed
method now has **100% source-shape**, **100% strict fuzzy** and
**linked-exact** instructions, not byte-exact. Their prior strict fuzzy /
source-shape scores were `setRotation` **82.8% / 98.2%**, `rotate`
**75.2% / 95.7%**, and `setScaling` **78.7% / 98.2%**. Focused exact counts
rise **23 -> 26 linked-exact**, staying **8 byte-exact**. The direct-call
audit observes **15/15** matching call destinations across the three
newly linked methods, including three stack-check calls.

The full corpus compares **4,323/4,524** original functions, with **201
missing**, **123 extras** and zero skipped. Average source-shape remains
**93.59%**; strict fuzzy rises **82.64% -> 82.65%**. Linked-exact rises
**2,503 -> 2,506**, byte-exact remains **1,074**. Exact `(unit,symbol)`
set comparison shows **zero losses**, three expected linked gains and no
byte gains. Whole-corpus direct-call matches rise **2,372 -> 2,387**;
**53 mismatch candidates** and **647 unresolved** remain unchanged.

Reports: `A:/Galaxy on Fire new version/_work/aegeometry_ctor_lod_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/aegeometry_rotation_after_unit_20261016.json`,
`A:/Galaxy on Fire new version/_work/aegeometry_rotation_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/aegeometry_rotation_full_audit_20261016.json`.

Eight `AEGeomCanvas` references remain in `translate(float,float,float)`,
`updateLod` and `setDirection`; these need separate argument/ownership
recovery. Live Android ARM32 rendering and iOS/PC analogue behavior were
not exercised. In particular, a linked-exact call route does not prove
the entire `PaintCanvas` transform implementation equivalent.
