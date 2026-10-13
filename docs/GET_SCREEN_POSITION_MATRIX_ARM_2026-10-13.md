# PaintCanvas::GetScreenPosition(const Matrix &, Vector &) ARM audit (2026-10-13)

The original Android ARMv7 function at `0x7bfb4` returns a boolean
visibility result. It copies the matrix translation into the output
vector, checks the selected camera and horizontal projection divisor,
projects screen coordinates, then checks camera depth, nonnegative Y,
X below screen width, and Y below screen height. It does not reject
negative X or test the vertical divisor for zero. The local C++
previously declared the method `void`, discarded the final visibility
result, added both of those checks, and routed integer-to-float
conversions and vector assignment through unresolved helper shims.

The declaration and body now return the source-backed boolean,
preserve the original check order (including unordered/NaN behavior),
and use the existing `Vector` assignment and ordinary numeric casts.
The other `GetScreenPosition` overloads were not changed. No in-tree
caller uses this overload; the corrected return type preserves its
C++ mangled symbol and changes only the caller-visible value.

Focused ARM source-shape improves **64.8% → 71.5%**; strict fuzzy
declines **30.7% → 27.5%**. It is neither linked- nor byte-exact.
Native `gof2`, all **206/206** ARM translation units, and 36 verifier
tests pass (one skipped). Full corpus: **4,714** comparisons,
**4,323/4,524** original functions covered, **201** missing.
Average source-shape rises **92.5558% → 92.5573%**, while strict
fuzzy changes **80.8509% → 80.8502%**. The **2,378 linked-exact**
and **1,008 byte-exact** symbol sets are unchanged, with no losses.

Evidence: `analysis/gof2_libgof2hdaa_full_ida.c` at `0x7bfb4`,
baseline `A:/Galaxy on Fire new version/_work/material_create_full_20261013.json`,
focused `A:/Galaxy on Fire new version/_work/getscreen_matrix_focused_20261013.json`,
and full `A:/Galaxy on Fire new version/_work/getscreen_matrix_full_20261013.json`.
No original-device or real-camera runtime test was performed, and
the other matrix/world-vector overload at `0x7bd58` still needs a
separate return-contract and control-flow audit.
