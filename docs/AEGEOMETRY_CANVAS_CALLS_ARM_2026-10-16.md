# AEGeometry canvas call contracts — Android ARM, 2026-10-16

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `PaintCanvas` directly from ten linked-exact `AEGeometry`
methods. REA/IDA Thumb instructions show `getPosition` at `0xa4592`
passing the canvas at object offset `0x2c` and transform index at
`0x0c` to `TransformGetLocal` (`0xa459e`); `addChild` at `0xa453a`
passes its transform and child to `TransformAddChild` (`0xa4548`).
The two LOD setters pass array slots by reference to
`TransformCreate` and then selected handles to
`TransformAddChild`; `getParentPosition` at `0xa4572` selects
`altTransform` unless it is `0xffffffff`, then calls
`TransformGetLocal` (`0xa4584`). The returned matrix is passed to
the appropriate vector helper or copied to `referenceMatrix`.
These are instruction and named-call observations, not a claim that
the decompiler's guessed return signatures are correct.

Before this change, twelve direct call sites in these ten methods
targeted `AEGeomCanvas` methods that are declared but have no source
definitions. The recovered methods now use the already implemented
`PaintCanvas::TransformGetLocal`, `TransformCreate` and
`TransformAddChild` signatures. Transform mathematics, LOD allocation
and child bookkeeping were not changed. An initial direct-call form
for `getParentPosition` moved register loads and lost one
linked-exact pair; a typed local canvas pointer restored the ARM
instruction shape without truncating the pointer on a 64-bit host.

`tools/run_aegeometry_canvas_calls_smoke.py` extracts eight
production method bodies into a host fixture. It checks matrix and
vector values, selected transform indices, both parent-index paths,
reference-matrix copying and two child additions. The two LOD setter
bodies are not run by that fixture; their call targets, as part of
the twelve corrected sites, are checked in the ARM object and
original disassembly instead.
The fixture does not exercise real transforms, rendering or GL.

Native `gof2`, **206/206** ARM translation units, **40** verifier
tests (one pre-existing skip), and the host fixture pass. The ten
changed methods stay **100% source-shape / 100% strict fuzzy** and
linked-exact, not byte-exact. Every one of their twelve corrected
direct-call destinations matches the original named callee. The
full corpus compares **4,323/4,524** unique original functions,
with **201 missing**, **123 extras**, zero skipped,
**93.59%/82.64%** average source-shape/strict fuzzy,
**2,503 linked-exact** and **1,074 byte-exact**. Exact
`(unit,symbol)` sets have zero losses/gains versus the preceding
report. The direct-call audit improves from **2,360 matches / 65
mismatch candidates** to **2,372 / 53**, with **647 unresolved**
unchanged. Reports are on A: at
`_work/draw_text_lines_full_20261016.json` (baseline),
`_work/aegeometry_canvas_calls_after_unit_20261016.json`,
`_work/aegeometry_canvas_calls_full_20261016.json` and
`_work/aegeometry_canvas_calls_full_audit_20261016.json`.

Another **22** `AEGeomCanvas` references remain in other
`AEGeometry` methods, many with raw 32-bit pointer conversions and
unverified argument roles. The called `PaintCanvas` bodies also
retain unrelated transition hooks. No live Android ARM32 transform,
LOD or GL scenario was exercised, and no iOS/PC analogue was checked
for this package. The next priority is to recover the remaining
`AEGeomCanvas` call sites by small function groups with original
argument and ownership evidence, not by bulk symbol replacement.
