# Mesh release dispatch (2026-10-13)

Android ARMv7 `AbyssEngine::MeshRelease(Engine *, Mesh **)` at `0x6c362`
checks the engine and mesh, calls `TransformRelease` on the animation
slot, then directly calls `MeshReleaseIntern`. The local body instead
called through `g_MeshRelease_freeFn`, a zero-initialized global with
no assignment in this repository. A non-null release would therefore
call a null pointer. The function now calls the existing typed
`MeshReleaseIntern` implementation, and the unused global is removed.

Focused ARM source-shape rises from **72.0%** to **98.3%**, and strict
fuzzy from **31.2%** to **83.3%**. It is not linked- or byte-exact;
the remaining difference is instruction scheduling/register selection,
not evidence for a different release operation. The native `gof2`
build, **206/206** ARM translation units, and 37 verifier tests
(one skipped) pass. An extracted-production-body host smoke checks
null guards, ordinary and uploaded mesh cleanup, seven GL buffer
deletions, shared data retention, animation destruction, slot clearing,
and the sprite-system release caller. The smoke stubs
`TransformRelease` and GL; real GL state, game assets, and ARM32 runtime
remain untested.

The full corpus has **4,717** comparisons, **4,323/4,524** covered,
**201** missing, **92.5772%** average source-shape (was 92.5716%),
**80.9006%** average strict fuzzy (was 80.8895%), **2,378
linked-exact**, and **1,008 byte-exact**. Neither exact symbol set
lost or gained members; extra symbols remain 125.

Evidence: Android decompiler output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
focused baseline
`A:/Galaxy on Fire new version/_work/mesh_release_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/mesh_release_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/mesh_release_full_20261013.json`.
Next audit the release ownership of animated/shared meshes and the
adjacent `TransformRelease` path with real asset lifetimes.
