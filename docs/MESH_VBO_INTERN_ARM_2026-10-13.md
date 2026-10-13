# MeshConvertToVBOIntern ARM source audit (2026-10-13)

Android ARMv7 `AbyssEngine::MeshConvertToVBOIntern(Mesh *)` begins at
`0x6d510`. It rejects a null mesh, unsupported VBOs, an already
uploaded mesh, or zero indices. It snapshots raw vertex-array pointers
before issuing GL calls and reloads the 16-bit vertex count for each
buffer size and byte-accounting step. Depending on format flags, it
uploads positions, UVs and indices, normals, shader tangents and
binormals, and colors. It then unbinds both targets and checks
`glGetError`. On success it optionally frees raw arrays, marks the
mesh uploaded, and adds its byte count to `Engine::vboSize`.

The local code previously cached one vertex count across the GL calls
and loaded positions, UVs, and normals only when used. It now matches
the Android load timing: raw pointers are captured before the first
GL call while the count is reread between calls. This also preserves
the original argument snapshots if a GL hook changes a mesh field.
Focused source-shape rises **95.9% → 100.0%** and strict fuzzy
**83.7% → 100.0%**; the method is still neither linked- nor
byte-exact. The remaining raw-byte differences are not claimed to be
behavioral differences.

An extracted-production-body host smoke with recording GL functions
checks the guards, upload order, targets, sizes, pointers, static-draw
usage, byte accounting, repeat rejection, error return, and both
`KeepRawMeshData` paths. It uses GL fixtures, not a graphics driver.
The Android error path tests `uploaded` before deleting generated
buffers, even though entry rejects uploaded meshes; the smoke records
that behavior rather than silently changing it. Actual GPU resource
lifetime, original assets, and ARM32 runtime remain untested.
Native `gof2`, **206/206** ARM translation units, this GL-fixture
smoke, the adjacent canvas smoke, and 36 verifier tests pass (one
skipped). Full corpus: **4,717** comparisons, **4,323/4,524** covered,
**201** missing. Average source-shape rises **92.5708% → 92.5716%**,
average strict fuzzy **80.8861% → 80.8895%**, and 100%-fuzzy
functions **2,528 → 2,529**. The **2,378 linked-exact** and **1,008
byte-exact** symbol sets have no losses or gains.

Evidence: Android decompiler output in
`analysis/gof2_libgof2hdaa_full_ida.c` and ARM disassembly at
`0x6d510`–`0x6d7e0`; focused baseline
`A:/Galaxy on Fire new version/_work/vbo_intern_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/vbo_intern_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/vbo_intern_full_20261013.json`.
Next priority: the canvas sprite-system resource release wrapper,
which still calls an unresolved release shim.
