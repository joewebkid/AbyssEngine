# Transform null-source clone cleanup (2026-10-13)

Android ARMv7 `AbyssEngine::Transform::Transform(Transform *)` at
`0x74918` constructs and destroys a temporary default `Transform`
once when the source pointer is null. The local C++ branch constructed
the temporary, explicitly called its destructor, and then implicitly
called that destructor again at scope exit. This could revisit the
already-released keyframe array. The redundant explicit call is
removed; the non-null clone path is unchanged.

Both constructor variants rise from **90.6% source-shape / 57.6%
strict fuzzy** to **91.5% / 57.7%**. Neither is linked- or byte-exact.
Native `gof2`, **206/206** ARM translation units, 37 verifier tests
(one skipped), and an extracted-branch host smoke pass. The smoke
seeds a temporary with an owned keyframe and confirms destruction
once. It does not prove a fully cloned transform tree or real game
animation lifetime.

The full corpus remains **4,718** comparisons, **4,323/4,524**
covered, **201** missing, **2,381 linked-exact**, **1,008 byte-exact**,
and 124 extra symbols. Average source-shape rises from **92.5979%**
to **92.5982%**; strict fuzzy rises from **80.9287%** to **80.9288%**.
No prior exact comparison row is lost.

Evidence: Android decompiler output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
focused baseline
`A:/Galaxy on Fire new version/_work/transform_clone_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/transform_clone_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/transform_clone_full_20261013.json`.
Next audit the non-null clone path's shared-keyframe and child-mesh
ownership with a complete runtime fixture; the null-source branch
does not initialize `this` as a default transform in the Android
reference, so that behavior must not be inferred.
