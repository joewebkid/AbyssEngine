# PaintCanvas::MeshCloneMaterial ARM recovery (2026-10-13)

The Android ARMv7 `libgof2hdaa.so` function at `0x7a380` checks
`index < meshCount`, allocates a 0x74-byte `Material`, calls the
pointer-taking `Material` constructor with the mesh's material at
offset `0x30`, appends the new pointer through
`ArrayAdd<Material *>`, and writes `materials.count - 1` to the
output. For an invalid mesh index it writes `0xffffffff` and does
not allocate. The local function had unresolved allocation, clone,
and append shims. It now uses `new Material(source)` and the existing
out-of-line `ArrayAdd` specialization. The declaration suppresses a
duplicate local template instantiation, matching the ARM call site.

The production-body host smoke uses real `Material` and `Mesh`
objects with a fixture `PaintCanvas`. It checks invalid-index
behavior, two clones, copied texture/default-ambient fields, distinct
ownership, output indices, Array growth, and cleanup. It does not
test a nonzero `addDataSize`, the full canvas constructor, OpenGL,
original assets, or ARM32 gameplay; the clone constructor's known
`addData` ownership uncertainty remains.

Focused ARM source-shape rises 82.2% → 96.8%, strict fuzzy
74.6% → 87.1%; the function remains non-exact. Native `gof2`,
206/206 ARM units, the clone and Material lifecycle smokes, and
36 verifier tests pass (one skipped).

Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
covered, 201 missing, zero skipped/wrong-type. Average source-shape
92.5438% → 92.5469%; strict fuzzy 80.8337% → 80.8364%.
Linked-exact stays 2,378 and byte-exact stays 1,008, with no exact
symbol-set gains or losses. Focused reports:
`A:/Galaxy on Fire new version/_work/mesh_clone_material_before_20261013.json`
and `A:/Galaxy on Fire new version/_work/mesh_clone_material_after_20261013.json`;
full report:
`A:/Galaxy on Fire new version/_work/mesh_clone_material_full_20261013.json`.

Next priority: audit `PaintCanvas::MaterialCreate`, which still uses
unresolved constructor/append shims for the same material table.
