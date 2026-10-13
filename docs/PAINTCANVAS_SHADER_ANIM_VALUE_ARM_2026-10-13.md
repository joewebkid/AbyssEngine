# PaintCanvas shader-animation value propagation (2026-10-13)

The Android ARMv7 mesh and transform overloads of
`PaintCanvas::MeshChangeShaderAnimValue` begin at `0x7a2a4` and
`0x7a2d2`. A non-null mesh writes the input float at offset `+0x24`
for mode 4, at `+0x20` for mode 2, or bitwise at `+0x1c` for mode 1.
Regardless of mode, it then recurses into its animation transform.
The transform overload visits each mesh and child transform in order,
passing through the same value and mode. Null inputs return.

The local mesh overload previously called an unresolved two-argument
shim after its field write, losing `value` and `mode` for animated
descendants. The transform overload likewise used two unresolved
recursive shims. All three calls now use the existing typed overloads.
This affects the shader-animation calls made by `PlayerEgo` and any
other caller using nested meshes; it does not alter the three field
offsets or the unknown-mode behavior.

An extracted-production-body host smoke checks modes 1, 2, and 4
across a mesh, animation transform, nested mesh, and child transform;
null inputs and an unknown mode are also exercised. It does not
exercise shaders, original assets, malformed cyclic transform graphs,
or ARM32 runtime. Focused source-shape remains **100.0%** for both
functions. Strict fuzzy rises **58.8% → 76.5%** for the mesh overload
and remains **97.1%** for the transform overload; family average rises
**77.9% → 86.8%**. Neither function is linked- or byte-exact.
Native `gof2`, 206/206 ARM translation units, the extended and
adjacent canvas smokes, and 36 verifier tests pass (one skipped).
Full corpus: **4,717** comparisons, **4,323/4,524** covered,
**201** missing. Average source-shape remains **92.5708%**; average
strict fuzzy rises **80.8823% → 80.8861%**. The **2,378 linked-exact**
and **1,008 byte-exact** symbol sets have no losses or gains.

Evidence: Android decompiler output at `0x7a2a4`–`0x7a322` in
`analysis/gof2_libgof2hdaa_full_ida.c`; focused baseline
`A:/Galaxy on Fire new version/_work/canvas_shader_anim_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/canvas_shader_anim_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/canvas_shader_anim_full_20261013.json`.
Next priority: `MeshChangeMaterialIntern(Mesh *, Material *)` and its
recursive transform path, where another shim loses the material
argument.
