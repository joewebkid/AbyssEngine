# AEGeometry direction basis (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `AEGeometry::setDirection(Vector const&, Vector const&)` at
`0xa4688` copies the current 60-byte local matrix through
`PaintCanvas::TransformGetLocal`. It computes a normalized right vector
from `VectorCross(up, dir)`, then a normalized new up vector from
`VectorCross(dir, right)`. It stores the three axes by matrix column:
`right` in `m[0,4,8]`, new up in `m[1,5,9]`, and the original `dir` in
`m[2,6,10]`. It writes the matrix through `TransformSetLocal`, obtains
that local matrix again, and calls `AEMath::MatrixSetScaling` with the
geometry's three scale values.

The recovered body previously used declared-only `AEGeomCanvas`,
`VectorCross` and `_ae_MatrixSetScaling` hooks, narrowed the canvas and
matrix pointers to 32 bits, reversed the second cross-product inputs,
and wrote the basis consecutively across `m[0..8]`. That overwrote
translation elements `m[3]` and `m[7]`. The new body uses existing
typed `PaintCanvas` and `AEMath` calls, restores the cross-product
order and matrix layout, and preserves the translation column. The
now-unused hook declarations were removed.

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` compiles nineteen production AEGeometry
bodies in a recording host fixture. Its new case uses nonunit direction
and up vectors, unequal scaling factors, and nonzero translation to
verify orientation, scale and preservation of translation. A second
call exercises a zero up vector, which is used by some callers, and
checks the math layer's documented zero-normalization fallback.
The geometry fixture implements the vector/math contract locally. The
separate native math-link smoke now checks the production `VectorCross`,
`VectorNormalize` (including its zero fallback) and `MatrixSetScaling`
bodies for these inputs. Neither is a live Android ARM32 or GL test.

Native `gof2`, **206/206** Android ARM translation units, the host
fixture, the existing native math-link smoke and **40** verifier tests
(one existing skip) pass. Focused AEGeometry verification compares
**45** functions. `setDirection` strict fuzzy/source-shape improves
from **45.1%/95.6%** to **85.4%/100%**; it is not linked- or
byte-exact. The remaining instruction differences are stack-slot
offsets for local temporaries; both versions have 89 instructions.
All ten compiled direct-call relocations have the original-named
destinations in the Android disassembly. The standard call-target audit
does not cover this function because it is not linked-exact.

The full corpus compares **4,323/4,524** unique original functions,
with **201 missing**, **123 extras** and zero skipped. Average
source-shape remains **93.60%**; strict fuzzy rises **82.67% ->
82.68%**. Linked-exact remains **2,507** and byte-exact **1,074**.
Exact `(unit,symbol)` pair comparison against the preceding full report
shows zero losses or gains. The linked-function direct-call audit is
unchanged at **2,392 matches / 53 mismatch candidates / 647
unresolved**. Reports: `A:/Galaxy on Fire new version/_work/aegeometry_translate_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/aegeometry_direction_after_unit_20261016.json`,
`A:/Galaxy on Fire new version/_work/aegeometry_direction_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/aegeometry_direction_full_audit_20261016.json`.

Four `AEGeomCanvas` references remain, all in `updateLod`. That
function also has distinct evidence for LOD thresholds, vector
subtraction order and transform ownership; it needs its own semantic
recovery rather than a bulk shim replacement. iOS/PC analogues and
live runtime output were not checked in this package.
