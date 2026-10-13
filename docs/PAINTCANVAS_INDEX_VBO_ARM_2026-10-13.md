# PaintCanvas indexed VBO dispatch (2026-10-13)

Android ARMv7 `PaintCanvas::MeshConvertToVBO(unsigned)` at `0x7a292`
checks the index against mesh count, loads the mesh pointer from the
array at canvas offset `+0x28`, and calls `AbyssEngine::MeshConvertToVBO`.
The local wrapper had the same instruction shape but called an
unresolved `paintcanvas_ext_convert_vbo` shim. It now calls the
already implemented typed converter directly.

An extracted-production-body host smoke checks a valid index reaches
the selected mesh and an invalid index makes no call. Native `gof2`,
206/206 ARM translation units, and 36 verifier tests pass (one
skipped). The focused wrapper remains **100.0% source-shape**,
**100.0% strict fuzzy**, and **linked-exact**, but not byte-exact.
Full corpus remains **4,717** comparisons, **4,323/4,524** covered,
**201** missing, **92.5708%** average source-shape, **80.8861%**
average strict fuzzy, **2,378 linked-exact**, and **1,008 byte-exact**.
Neither exact symbol set loses a member; extra-symbol count remains
125. The native object now references the defined typed converter
instead of the unresolved shim.
The smoke stubs the converter: GPU buffer creation, asset data, and
ARM32 runtime are not verified by this package.

Evidence: Android decompiler output in
`analysis/gof2_libgof2hdaa_full_ida.c`; focused baseline
`A:/Galaxy on Fire new version/_work/canvas_index_vbo_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_index_vbo_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_index_vbo_full_20261013.json`.
Next priority: audit `AbyssEngine::MeshConvertToVBOIntern(Mesh *)`,
the converter's GPU-upload body, separately from this dispatch fix.
