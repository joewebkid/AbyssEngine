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

The next host gate runs production `PaintCanvas::FindResource` and
`MeshCreate` for both mesh IDs, and production `FindResource` and both
`TextureCreate` overloads for the corresponding texture IDs. Each path
reaches the relevant production file parser, stores a resource handle,
and reuses that handle on a second request without reopening the file.
Fixture-owned resource records carry the exact IDs and paths from the
tables; `BuildResourceList`/`loadLowTexturesAndMaterials` themselves are
not executed. Mesh material creation and VBO upload are bounded services.
The texture upload service is a GPU boundary double which calls the real
`ImageCreateFromFile`/`ImageRelease`, then issues a test texture ID.
Therefore this is a canvas-to-parser handoff, not a GL upload or rendered
skybox test.

Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
allocates 20 bytes for `ImageCreateFromFile` at `0x6f4fe`–`0x6f500`.
The native x64 `Image` is larger because it holds a pointer. Replacing the
fixed `0x14` allocation with `sizeof(Image)` preserves the ARM allocation
and prevents a host-side overwrite. The host smoke records and checks the
actual requested allocation size.

Native `gof2` and 208/208 ARM translation units build. All 40 verifier
tests pass (one existing skip). Focused `ImageCreateFromFile` remains
97.3% source-shape and 58.8% strict fuzzy, nonexact. The full ARM corpus
compares 4,323/4,524 original functions, with 201 absent, 93.87% mean
source-shape, 83.05% mean fuzzy, 2,528 linked-exact and 1,076 byte-exact.
There are no exact-pair losses against
`fog_parameter_full_20261019.json`; the new full report is
`A:/Galaxy on Fire new version/_work/level_skybox_asset_full_20261019.json`.

This gate proves that the exact four ordinary-orbit skybox files can be
selected by the recovered canvas methods and parsed by the recovered mesh
and image loaders. It does **not** yet prove production resource-table
registration, material creation, GL texture upload, the complete
`Level::init` path, or `renderBG`. The next integration step is to connect
the level selector and these canvas calls under one host run and identify
the first failing service boundary without claiming a complete native
flight scene.
