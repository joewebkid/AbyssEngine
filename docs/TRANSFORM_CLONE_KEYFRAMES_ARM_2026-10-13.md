# Transform clone keyframe-list ownership (2026-10-13)

Android ARMv7 `Transform::Transform(Transform *)` at `0x74918`
calls `ArraySet<KeyFrame *>` to copy the source keyframe-pointer list
after cloning child meshes and before cloning child transforms. The
local constructor used `Array::operator=`, which copies the source
array's capacity as well as its count. Android `ArraySet` instead
sets destination capacity to the copied count (or one when empty).
The constructor now calls the existing `ArraySet` overload. It still
shares the keyframe objects, marks the clone `keyFramesShared`, and
creates new child mesh and transform objects.

An extracted-production-body host smoke executes both `Transform`
constructors and its destructor with one child transform, two child
meshes, and owned keyframes. The source deliberately has spare
keyframe-array capacity. It verifies that the clone has an independent
pointer array sized to the copied count, shares the pointed-to frames,
clones meshes and the child transform, and does not delete the owner's
frames on clone destruction. The fixture stubs `Mesh::Mesh` and
manually deletes child objects; it does not prove the production
`MeshRelease` / `TransformRelease` lifetime path or real assets.

Both clone-constructor variants stay **91.5% source-shape**, while
strict fuzzy rises from **57.6%** to **58.1%**. Neither constructor
is linked- or byte-exact. Native `gof2`, **206/206** ARM translation
units, 37 verifier tests (one skipped), and the clone and adjacent
release host smokes pass.

The full corpus has **4,720** comparisons, **4,323/4,524** original
functions covered, **201** missing, **92.6014%** average source-shape
(was 92.5982%), **80.9370%** average strict fuzzy (was 80.9287%),
**2,383 linked-exact** (was 2,381), **1,008 byte-exact** (unchanged),
and 123 extra symbols. Neither previous exact comparison-row set
loses a member. The two new linked-exact rows are translation-unit
instances of the already-exact `ArraySet<KeyFrame *>` overloads, not
exact constructor matches.

Evidence: Android decompiler output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
focused baseline
`A:/Galaxy on Fire new version/_work/transform_clone_arrayset_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/transform_clone_arrayset_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/transform_clone_arrayset_full_20261013.json`.
Next audit the production recursive release path's child-transform
ownership and the actual `Mesh` clone, including ARM32 runtime when
assets are available.
