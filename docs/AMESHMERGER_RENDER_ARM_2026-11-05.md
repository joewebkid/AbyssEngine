# AMeshMerger render layout and call (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `AMeshMerger::render()` at `0x1811e4`. Its five instructions load
the canvas from `this + 20` and the transform ID from `this + 28`,
set the third argument to null, then tail-branch through the veneer
at `0x19bd98` to the `PaintCanvas::DrawTransform(unsigned int,
Matrix const*)` PLT entry at `0x6306c`. The adjacent destructor
accesses the mesh array at `this + 8`.

The reconstruction used an undefined `AMeshMerger_drawMeshes` helper
and placed the canvas and transform ID at offsets 24 and 32. The
helper call is now the typed `PaintCanvas::DrawTransform` call. The
extra four-byte pad after the mesh array is removed, restoring the
original field offsets. This changes the ARM32 class layout, not just
the comparison metric.

Focused ARMv7 verification compiles **210/210** translation units.
`AMeshMerger::render()` rises from **92.0% to 100%** source-shape and
**60.0% to 100%** strict fuzzy, gaining linked-exact. Across 210 ARM32
objects the raw unmatched-symbol inventory falls **250 -> 249**:
only `_Z22AMeshMerger_drawMeshesPvjj` disappears, with no new names.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. Compared with the preceding full report, no
linked-exact or byte-exact pair is lost; this function is the only
linked-exact gain.

Windows native and ARM64 static builds pass, as does strict ARM64
shared linking. Device rendering and full ARM32 shared linking remain
unverified.
