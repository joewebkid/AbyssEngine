# PaintCanvas::MaterialCreate ARM recovery (2026-10-13)

The Android ARMv7 `libgof2hdaa.so` direct overload at `0x7c13c`
allocates a 0x74-byte `Material`, calls its default constructor,
writes the first texture and blend/material mode, appends the pointer
through `ArrayAdd<Material *>`, and returns the new index through
the output reference. Its fourth parameter is not used. The local
function previously used unresolved allocation, constructor, and
append shims; it now calls the real constructor and Array template.

The resource overload at `0x79e4c` calls `FindResource`; when the
resource handle is `-1`, it creates a default `Material`, visits up
to eight texture IDs, skips `0xffff`, stops on an absent texture
resource, and calls the three-argument `TextureCreate` when a texture
handle is `-1`. It then copies the material mode, the resource's
32-bit add-data pointer word and size, and the color vector, appends
the material, caches its index in the resource, and writes the output.
An absent material resource leaves the output untouched. These calls
and offsets are visible in the ARM function and agree with the
existing local 32-bit `Material` and resource layouts. The code now
uses those real paths instead of four unresolved material-create
shims and a local raw Array append. The `ArrayAdd<Material *>`
specialization remains out of line in `Array.cpp`.

A host smoke extracts both production method bodies, uses real
`Material` and Array code, and fixtures resource lookup and texture
creation. It checks direct creation, existing and newly created
texture handles, skipped/missing texture IDs, resource-cache reuse,
the absent-resource output, and Array growth. It does not execute the
real texture loader, OpenGL, original assets, or ARM32 gameplay. The
resource's add-data pointer word is tested only at zero; nonzero
ownership remains unresolved.

Focused ARM source-shape rises 78.9% → 100.0%, strict fuzzy
68.1% → 96.2% for the direct overload; the resource overload rises
75.8% → 96.8% source-shape and 19.0% → 59.3% strict fuzzy. Neither
is linked- or byte-exact. Native `gof2`, 206/206 ARM units, the new
and adjacent material smokes, and 36 verifier tests pass (one skipped).

Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
covered, 201 missing, zero skipped/wrong-type. Average source-shape
92.5469% → 92.5558%; strict fuzzy 80.8364% → 80.8509%.
Linked-exact remains 2,378 and byte-exact remains 1,008, with no
exact-symbol gains or losses. Focused reports:
`A:/Galaxy on Fire new version/_work/material_create_before_20261013.json`
and `A:/Galaxy on Fire new version/_work/material_create_after_20261013.json`;
full report:
`A:/Galaxy on Fire new version/_work/material_create_full_20261013.json`.

Next priority: audit the five-argument `TextureCreate` path reached
when a material texture has not yet been created. Its deeper loader
and asset behavior is not established by this package.
