# Level skybox asset-loader gate (2026-10-19)

For an ordinary orbit with system index 4 and texture index 2,
`Level::createSpace` requests background mesh/texture resources
`0x45bb/0x2767` and main mesh/texture resources `0x458a/0x2753`.
`BuildResourceList.cpp` and `LowResourceTable.cpp` map them to
`skybox_stars.aem`, `skybox_stars_001.aei`, `skybox_002.aem`, and
`skybox_002.aei` in the Android OBB. The four local original files are
identified by SHA-256, respectively:

- `aa1b03cd8dfe0a4204581567c198ee71fc30a535f54c8dc1e9374a8717219d45`
- `b229550d982badab49beccf7b9a9b3611bd0d0cf61f4ae041ce12755b7d9bc73`
- `2f274dd9eaba051038a1e1379b49e36d7c9cc99f1f11a5c3aa3a258d6317d0d8`
- `2244222571cf9f19a7d5ed16a42451be004f4734f744a76a25f28f32d402c3db`

The production `MeshCreateFromFile`/`MeshReadData` bodies now pass an
asset-backed host smoke for both meshes. The production
`ImageCreateFromFile`/`ImageRelease` bodies pass `-O0/-O2` host smokes for
both textures, including successful decode and a truncated-data failure.
The file service is the fixture boundary: it supplies the unchanged
original bytes to production parsers. The texture files each contain two
trailing bytes after the portion consumed by `ImageCreateFromFile`; the
truncation test cuts the consumed payload, not that trailer.

The canvas host gate runs production `PaintCanvas::AddResource`,
`FindResource` and `MeshCreate` for both mesh IDs, and production
`AddResource`, `FindResource` and both `TextureCreate` overloads for the
corresponding texture IDs. Each path
reaches the relevant production file parser, stores a resource handle,
and reuses that handle on a second request without reopening the file.
Fixture-owned resource records carry the exact IDs and paths from the
tables; `BuildResourceList`/`loadLowTexturesAndMaterials` themselves are
not executed. Mesh material creation and VBO upload are bounded services.
The texture upload service is a GPU boundary double which calls the real
`ImageCreateFromFile`/`ImageRelease`, then issues a test texture ID.
Therefore this is a canvas-to-parser handoff, not a GL upload or rendered
skybox test.

An in-process host gate now also runs the production-source-extracted
skybox prelude of `Level::createSpace` through those canvas methods and
loaders for the ordinary system-index-4/texture-index-2 case. It loads
the four files above at `-O0` and `-O2`, obtains handles `0/100/1/101`,
and observes four opens/four closes. Calling the prelude again performs
no additional file open. This is stronger than independent selector and
parser tests, but the source extraction stops before the rest of
`createSpace`; `Status`, resource records, material creation, VBO and GPU
services remain bounded host fixtures. Focused `createSpace` source-shape/
fuzzy is still 86.1%/46.2%, nonexact.

Android ARM `PaintCanvas::AddResource` at `0x77f84`–`0x77f8c` forwards
the resource and `this+0x134` through a veneer to
`ArrayAdd<Resource*>@plt` at `0x5fa30` (the defined specialization is at
`0x77f90`). The recovered `AddResource` body was already linked-exact,
but its synthetic `paintcanvas_ext_add_resource` target had no native
implementation. That helper now performs the same `ArrayAdd`; it has no
remaining undefined reference in the native `PaintCanvas` object. The
asset-backed tests call the production `AddResource` body rather than
adding records directly. Focused ARM remains 100% source-shape/fuzzy and
linked-exact (not byte-exact).
The unchanged production functions have these focused values in
`level_skybox_asset_full_20261019.json`: `FindResource` 100% source-shape/
100% fuzzy (byte-exact), resource `MeshCreate` 98.6%/86.7% (nonexact),
callback `TextureCreate` 95.5%/51.7% (nonexact), and its convenience
overload 100%/100% (linked-exact, not byte-exact). The canvas handoff
first changed tests only; the later helper implementation changes the ARM
symbol inventory as reported below.

Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
allocates 20 bytes for `ImageCreateFromFile` at `0x6f4fe`–`0x6f500`.
The native x64 `Image` is larger because it holds a pointer. Replacing the
fixed `0x14` allocation with `sizeof(Image)` preserves the ARM allocation
and prevents a host-side overwrite. The host smoke records and checks the
actual requested allocation size.

Native `gof2` and 208/208 ARM translation units build. All 40 verifier
tests pass (one existing skip). Focused `ImageCreateFromFile` remains
97.3% source-shape and 58.8% strict fuzzy, nonexact. The current full ARM
corpus compares 4,323/4,524 original functions, with 201 absent,
93.87% mean source-shape, 83.06% mean fuzzy, 2,529 linked-exact and
1,076 byte-exact. Relative to
`level_skybox_asset_full_20261019.json`, no exact pair was lost and
`ArrayAdd<Resource*>` gained one linked-exact pair. One additional
synthetic helper raises the extra-symbol count to 125; it is not an
original game function. Current full report:
`A:/Galaxy on Fire new version/_work/canvas_add_resource_full_20261019.json`.

This gate proves that the exact four ordinary-orbit skybox files can be
selected by the `Level` skybox prelude, looked up by the recovered canvas
methods and parsed by the recovered mesh and image loaders in one host
process. It does **not** yet prove production resource-table construction,
material creation, GL texture upload, the complete `createSpace` tail,
`Level::init`, or `renderBG`. The next integration step is production table
setup and the remainder of `createSpace`, identifying the first failing
service boundary without claiming a complete native flight scene.
