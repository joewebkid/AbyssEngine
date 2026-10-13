# Material default ambient sentinel (2026-10-13)

The Android ARMv7 `libgof2hdaa.so` default `Material` constructor at
`0x7c188` stores `0xc1200000` (−10.0f) at offset `0x68`, the X
component of `ambientColor`. The local constructor had a placeholder
zero instead. `MaterialDraw` reads this component and skips the
light-ambient change/restoration when it equals −10.0f, so the wrong
default selected a different rendering path for a fresh material.
The constructor now writes the source-backed sentinel directly.

The pointer-taking clone constructor at `0x7a3d8` allocates a fresh
`addData` buffer when `addDataSize` is nonzero but does not copy its
contents. Its null-source branch does not initialize `addData` or
`addDataSize`. The destructor at `0x7c8b8` destroys the five Array
members but does not free `addData`. The local implementation reflects
these observed behaviors; this pass does not silently change them or
claim ownership safety. The real `Material` host smoke checks the
default sentinel and fields, Array initialization and two rounds of
queue reuse, and the zero-sized copy/null-source paths. The existing
`MaterialDraw` fixture checks the ambient sentinel branch separately;
the two smokes do not constitute an integrated GL frame.

Focused ARM constructor source-shape rises 97.4% → 99.3%, strict
fuzzy 91.4% → 93.5%; it remains non-exact. The pointer-taking
constructor remains 85.2% source-shape / 68.2% strict fuzzy; the
destructor remains linked-exact. Native `gof2`, 206/206 ARM units,
the lifecycle, SpriteSystemDraw, and MaterialDraw host smokes, and
36 verifier tests pass (one skipped).

Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
covered, 201 missing, zero skipped/wrong-type. Average source-shape
92.5434% → 92.5438%; strict fuzzy 80.8333% → 80.8337%.
Linked-exact stays 2,378 and byte-exact stays 1,008; exact symbol
sets are unchanged. Focused reports:
`A:/Galaxy on Fire new version/_work/material_lifecycle_before_20261013.json`
and `A:/Galaxy on Fire new version/_work/material_lifecycle_after_20261013.json`;
full report:
`A:/Galaxy on Fire new version/_work/material_lifecycle_full_20261013.json`.

Next priority: replace the unresolved clone/allocation/append shims in
`PaintCanvas::MeshCloneMaterial` with its confirmed `Material`
constructor and Array path, then test the actual clone owner. A full
native executable, sanitizer pass, original assets, and ARM32 gameplay
remain untested.
