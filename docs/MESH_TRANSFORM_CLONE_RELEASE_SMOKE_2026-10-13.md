# Mesh/Transform clone and recursive release audit (2026-10-13)

The host smoke now extracts the unchanged production bodies of
`Mesh::Mesh(Mesh *)`, both `Transform` constructors, `Transform::~Transform`,
`MeshRelease`, `MeshReleaseIntern`, and `TransformRelease`. It no longer
substitutes a fake mesh copy constructor. A minimal fixture supplies the
required types and stubs only the GPU calls and `MeshConvertToVBO` boundary.

The test covers a root mesh, a child-transform mesh, a separately cloned
mesh animation, and a shared keyframe. It checks raw-vertex pointer identity,
bounds and pivot values, all four primary VBO names, the color VBO, the
shared/uploaded flags, the nested animation copy, and the shader-enabled
tangent/binormal path. Releasing the clone recursively clears its mesh slots
without deleting the owner's raw vertices or GPU buffers. Its child
`Transform` object remains allocated and is explicitly deleted by the
fixture after recursive release.

That last behavior matches Android `TransformRelease` at `0x6c314`, which
walks children and meshes but does not delete child `Transform` objects.
Android `Transform::~Transform` at `0x775a8` releases the child-pointer
array, not the pointed-to transforms. This may be an ownership contract or
an original leak; no source change is justified without tracing callers and
real asset lifetimes. Android `Mesh::Mesh(Mesh *)` is at `0x6ba90` and
`MeshRelease`/`MeshReleaseIntern` are at `0x6c362`/`0x6c380` in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`.

Validation: `python tools/run_transform_clone_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` passes. No production source was changed,
so the last full ARM corpus remains the `transform_clone_arrayset_full_20261013.json`
snapshot: 4,323/4,524 original functions covered, 92.6014% average
source-shape, 2,383 linked-exact and 1,008 byte-exact, with no change to
those numbers. This extracted host test does not exercise actual GPU or file
assets, ARM32 runtime, or allocator instrumentation. Next trace the callers
that own and eventually destroy cloned child `Transform` objects, then run
asset-backed lifecycle checks when assets are available.
