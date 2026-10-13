# PaintCanvas sprite-system resource release (2026-10-13)

Android ARMv7 `PaintCanvas::ReleaseSpriteSystemResource(unsigned)` at
`0x7c7f4` guards the index against the sprite-system array count and
dispatches the selected slot to `AbyssEngine::SpriteSystemRelease`.
That function at `0x87064` releases the CPU position and size arrays,
releases the child mesh, deletes the sprite system, and clears the
slot. The local wrapper previously called an unresolved release shim;
it now calls the existing typed release function directly.

The wrapper retains **100.0% source-shape**, **100.0% strict fuzzy**,
and its linked-exact match (not byte-exact). Native `gof2`, **206/206**
ARM translation units, and 36 verifier tests pass (one skipped). An
extracted-production-body host smoke exercises the wrapper and release
body with valid/invalid indices, a null slot, allocated CPU arrays,
an adjacent untouched slot, and a repeated release. `MeshRelease`
is stubbed, so its ownership behavior, actual game assets, and ARM32
runtime remain untested.
Full corpus remains **4,717** comparisons, **4,323/4,524** covered,
**201** missing, **92.5716%** average source-shape, **80.8895%**
average strict fuzzy, **2,378 linked-exact**, and **1,008 byte-exact**.
Neither exact symbol set loses a member; extra-symbol count remains
125. The native canvas object now references the defined typed
`SpriteSystemRelease` instead of the unresolved shim.

Evidence: Android decompiler output in
`analysis/gof2_libgof2hdaa_full_ida.c`; focused baseline
`A:/Galaxy on Fire new version/_work/canvas_sprite_release_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_sprite_release_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_sprite_release_full_20261013.json`.
Next priority: `AbyssEngine::MeshRelease(Engine *, Mesh **)`, used by
this path and currently only 72% source-shape.
