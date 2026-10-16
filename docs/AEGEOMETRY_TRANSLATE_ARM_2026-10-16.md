# AEGeometry translation axes (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `AEGeometry::translate(float,float,float)` at `0xa492c`
loads the current transform's 60-byte local matrix through
`PaintCanvas::TransformGetLocal`, copies it, calls
`AEMath::MatrixSetTranslation(matrix, matrix.m[3] + x,
matrix.m[7] + y, matrix.m[11] + z)`, then writes the matrix back through
`PaintCanvas::TransformSetLocal`. The argument registers and stack values
at `0xa495e–0xa499c` establish the axis mapping. The previous recovered
body ignored `x`, added `z` to `m[3]`, added `y` to `m[11]` and left
`m[7]` unchanged. It also narrowed the canvas and returned matrix
pointers to 32 bits via a declared-only `AEGeomCanvas` call.

The new body keeps the full-width returned pointer, preserves the
60-byte matrix copy and sends the three correctly updated coordinates
to the existing `MatrixSetTranslation` before storing the result. The
`translate(Vector const&)` overload was not changed; its dispatch to
this body is also exercised.

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now exercises eighteen production bodies.
The new host case starts with unequal coordinates `(10,20,30)`, checks
the float overload produces `(11,22,33)`, then checks the vector
overload produces `(13,26,39)`; it records the matrix helper arguments
and `PaintCanvas` write-back. Native `gof2`, **206/206** ARM translation
units and **40** verifier tests (one existing skip) pass.

Focused `AEGeometry` verification compares **45** functions. The float
overload changes from **59.6% strict fuzzy / 95.8% source-shape**, not
exact, to **100% / 100% linked-exact**, not byte-exact. The unchanged
vector overload also reaches **100% / 100%** fuzzy/source-shape but is
not linked-exact. Focused linked-exact rises **26 -> 27**, byte-exact
stays **8**. The direct-call audit confirms all **5/5** named call
destinations in the newly linked float overload.

The full corpus still compares **4,323/4,524** original functions,
with **201 missing**, **123 extras** and zero skipped. Average
source-shape rises **93.59% -> 93.60%** and strict fuzzy
**82.65% -> 82.67%**. Linked-exact rises **2,506 -> 2,507**;
byte-exact remains **1,074**. Exact `(unit,symbol)` pair comparison
shows zero losses and only the expected linked gain. Whole-corpus
direct-call matches rise **2,387 -> 2,392**; **53 mismatch candidates**
and **647 unresolved** remain unchanged.

Reports: `A:/Galaxy on Fire new version/_work/aegeometry_rotation_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/aegeometry_translate_after_unit_20261016.json`,
`A:/Galaxy on Fire new version/_work/aegeometry_translate_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/aegeometry_translate_full_audit_20261016.json`.

Seven `AEGeomCanvas` references remain in `updateLod` and
`setDirection`. Original `setDirection` at `0xa4688` also shows a
different matrix-basis layout from the current source, so it is the
next small semantic/call-contract package. Runtime ARM32/GL behavior
and iOS/PC analogues were not checked here; the fixture records the
translation helper rather than independently testing its numerical
implementation.
