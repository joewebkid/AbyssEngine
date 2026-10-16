# AEGeometry LOD selection and transform transfer (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `AEGeometry::updateLod(Vector const&, float)` spans
`0xa4d80–0xa4f82`. Its ARM instructions, rather than decompiler
variable names, establish these roles:

- Copy the current transform's 60-byte matrix. For position, read
  `altTransform` when it is not `0xffffffff`, otherwise the current
  transform. Store `MatrixGetPosition(...) - camPos` in `cameraDelta`.
- Compare the squared distance with `lastVisibleDistSq`, setting
  `currentLod = -1` on the invisible path before any transform lookup.
- Read the active transform's `currentTime` at offset `0x110`. The
  detail factor is `0.5` through screen factor `0.33`, `0.75` through
  `0.66`, and `1.0` above that. The second comparison uses the
  original ARM `HI` condition: `NaN` takes the not-`<= 0.33` path.
- When a different LOD handle is selected, write the saved matrix to
  that *target* handle, update `transform`, transfer the saved time to
  the selected and base transforms, update `currentLod` and
  `referenceMatrix`, and notify the optional merger.
- When no LOD threshold selects a child, write the saved matrix to
  `baseTransform`, restore that handle and LOD zero, and notify the
  optional merger. This path does not reset animation time.

The previous body used four declared-only `AEGeomCanvas` calls and a
declared-only `Transform_GetTransform` hook, narrowed pointers to 32
bits, read the wrong position transform, reversed the stored camera
delta, used `0.03125`/`0.0625` thresholds, passed transform IDs as
matrix pointers, and reset animation time to zero. The recovered body
now uses the existing `PaintCanvas` and `Transform` APIs with typed
pointers and the ARM-observed matrix/time destinations. The unrelated
LOD allocation and merger implementations were not changed.

`python tools/run_aegeometry_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now extracts twenty production bodies
into a host fixture. New cases check alternative and fallback position
handles, signed camera delta, exact `0.33`/`0.66` boundaries, `NaN`,
unchanged and changed LOD handles, animation-time transfer, base
fallback, visibility culling, matrix-copy destination and merger
notification. The fixture's matrix accessors were aligned with the
actual 15-float matrix layout. It records canvas and transform calls;
it is not a live ARM32 animation or rendering test.

Native `gof2`, **206/206** ARM translation units, the host fixture and
**40** verifier tests (one existing skip) pass. Focused AEGeometry
verification compares **45** functions. `updateLod` strict fuzzy /
source-shape rises from **82.8% / 96.4%** to **97.6% / 100%**.
Both original and reconstructed functions have **164** instructions,
but `updateLod` is not linked- or byte-exact: remaining differences
are stack-slot allocation and the scheduling of an independent index
decrement. The 19 compiled direct-call relocations have the
original-named destinations in the Android disassembly. The standard
call-target audit does not cover this nonexact function.

The full corpus compares **4,323/4,524** unique original functions,
with **201 missing**, **123 extras** and zero skipped. Average
source-shape remains **93.60%**; strict fuzzy rises **82.68% ->
82.69%**. Linked-exact remains **2,507** and byte-exact **1,074**.
Exact `(unit,symbol)` set comparison against the preceding full report
shows zero losses or gains. The linked-function direct-call audit is
unchanged at **2,392 matches / 53 mismatch candidates / 647
unresolved**. Reports: `A:/Galaxy on Fire new version/_work/aegeometry_direction_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/aegeometry_lod_after_unit_20261016.json`,
`A:/Galaxy on Fire new version/_work/aegeometry_lod_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/aegeometry_lod_full_audit_20261016.json`.

No `AEGeomCanvas` call remains in `AEGeometry.cpp`; the header and any
other consumers were not removed. Live ARM32 behavior, frame-to-frame
animation continuity, GL output, and iOS/PC analogues remain
unverified. The next priority is an executable transform/LOD runtime
scenario and then the remaining missing or semantically weak symbols,
not a claim of project completion from this local source-shape result.
