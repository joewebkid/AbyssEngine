# PaintCanvas triangle-count path (2026-10-13)

Android ARMv7 functions at `0x7a5cc`, `0x7a5f2`, and `0x7b690`
count a mesh's own `indexCount / 3`, then recursively count animation
meshes and child transforms. A null mesh or transform contributes zero;
the indexed transform wrapper returns zero for an out-of-range index.
These contracts agree with the already recovered
`TransformGetTriCount(Transform *)` body.

Previously, `MeshGetTriCount(Mesh *)` and the indexed transform wrapper
called an unresolved transform-count shim. The mesh path now calls the
typed recursive method directly. The indexed wrapper keeps its external
call shape, with an out-of-line compatibility function forwarding to
the typed method: changing that wrapper to a same-translation-unit call
preserved normalized instructions but lost its pre-existing
linked-exact match. The compatibility function resolves the runtime
dependency while retaining that exact match.

An extracted-production-body host smoke tests null inputs, a plain
mesh, nested meshes and child transforms, and valid/invalid indices.
It does not test malformed cyclic transform graphs, original assets,
or ARM32 runtime execution. Focused family source-shape stays **98.2%**
and strict fuzzy **92.2%**; its three members remain **94.7%/82.4%**,
**100%/94.1%**, and **100%/100%** respectively (source/fuzzy). The
indexed wrapper remains linked-exact, and none is byte-exact.
Native `gof2`, 206/206 ARM translation units, the host smoke, and 36
verifier tests pass (one skipped). Full corpus has **4,717**
comparisons, **4,323/4,524** covered, **201** missing, **92.5708%**
average source-shape, **80.8823%** average strict fuzzy, **2,378**
linked-exact and **1,008** byte-exact; neither exact symbol set loses
a member. The implemented compatibility entry adds one extra symbol
not present in the original (124 versus 123), rather than an unresolved
runtime reference.

Evidence: Android decompiler output in
`analysis/gof2_libgof2hdaa_full_ida.c`; focused baseline
`A:/Galaxy on Fire new version/_work/canvas_tri_count_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_tri_count_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_tri_count_full_20261013.json`.
Next priority: the neighboring mesh/transform shader-animation value
path, which still calls unresolved shims.
