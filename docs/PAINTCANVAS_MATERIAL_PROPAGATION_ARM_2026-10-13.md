# PaintCanvas recursive material propagation (2026-10-13)

Android ARMv7 `PaintCanvas::MeshChangeMaterialIntern(Transform *,
Material *)` at `0x7a322` visits each mesh and child transform when
both arguments are non-null. Its mesh overload at `0x7a372` writes the
material pointer at mesh offset `+0x30`, then recurses into the mesh's
animation transform at `+0x34` with the *same* material pointer.

The local mesh overload previously called an unresolved two-argument
shim after writing `mesh->material`. That call did not declare or pass
the material argument, so propagation to animated descendants was not
a sound C++ call. The shim now has the three-argument contract and an
out-of-line typed implementation forwarding to the existing transform
overload. Its external-call shape is retained because the mesh
overload was already linked-exact. The source/strict-fuzzy scores stay
**100.0%/100.0%** for the mesh overload and **100.0%/92.3%** for the
transform overload; the mesh overload remains linked-exact, and
neither is byte-exact.

An extracted-production-body host smoke checks material propagation
through a mesh animation, nested meshes, and a child transform. It
also checks null mesh/transform/material guards and direct transform
dispatch. This does not exercise real render materials, malformed
cyclic transform graphs, original assets, or ARM32 runtime.
Native `gof2`, 206/206 ARM translation units, the extended host
smoke, and 36 verifier tests pass (one skipped). Full corpus remains
**4,717** comparisons, **4,323/4,524** covered, **201** missing,
**92.5708%** average source-shape, **80.8861%** average strict fuzzy,
**2,378 linked-exact**, and **1,008 byte-exact**. Neither exact symbol
set loses a member. The compatibility entry adds one extra symbol
(125 versus 124) while resolving the former runtime dependency.

Evidence: Android decompiler output in
`analysis/gof2_libgof2hdaa_full_ida.c`; focused baseline
`A:/Galaxy on Fire new version/_work/canvas_material_propagation_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_material_propagation_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_material_propagation_full_20261013.json`.
Next priority: the indexed `PaintCanvas::MeshConvertToVBO` wrapper,
which still calls an unresolved conversion shim despite having a
linked-exact shape.
