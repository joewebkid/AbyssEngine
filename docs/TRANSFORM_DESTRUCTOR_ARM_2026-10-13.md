# Transform keyframe ownership at destruction (2026-10-13)

Android ARMv7 `AbyssEngine::Transform::~Transform` at `0x775a8`
calls `ArrayReleaseClasses<KeyFrame *>` only when `keyFramesShared`
is false, then runs the ordinary member destructors. The local body
instead deleted only its counted keyframes and called `ArrayRemoveAll`,
which reallocates the backing array immediately before its destructor.
It now uses the existing `ArrayReleaseClasses` helper, which deletes
the owned entries, releases backing storage, and clears the pointer.
The shared case still leaves the keyframe objects to their owner.

Both destructor variants rise from **55.0% source-shape / 43.1%
strict fuzzy** to **100.0% / 100.0%**, and both become linked-exact
(not byte-exact). Native `gof2`, **206/206** ARM translation units,
37 verifier tests (one skipped), and an extracted-production-body
host smoke pass. The smoke uses the production `Array` helper and
checks owned, shared, and empty frame cases. It does not execute a
real game animation, GL work, or ARM32 runtime.

The full corpus now has **4,718** comparisons, **4,323/4,524**
original functions covered, **201** missing, **92.5979%** average
source-shape (was 92.5772%), **80.9287%** average strict fuzzy (was
80.9006%), **2,381 linked-exact** (was 2,378), and **1,008
byte-exact** (unchanged). Neither prior exact row set loses a member.
Of the three new linked-exact comparison rows, two are destructor
variants and one is another translation-unit instance of the already
exact `ArrayReleaseClasses<KeyFrame *>`. Extra symbols fall from 125
to 124.

Evidence: Android decompiler output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
focused baseline
`A:/Galaxy on Fire new version/_work/transform_dtor_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/transform_dtor_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/transform_dtor_full_20261013.json`.
Next audit the `Transform(Transform *)` clone constructor and its
shared-keyframe lifetime; its two variants are currently 90.6%
source-shape and 57.6% strict fuzzy.
