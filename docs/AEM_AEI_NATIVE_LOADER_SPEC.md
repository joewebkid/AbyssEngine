# AEM/AEI Native Loader Spec Notes

Date: 2026-06-30

This note captures the current source-backed understanding of the native
GOF2 HD mesh/texture loader. It is meant as a decompilation reference, not as
an asset conversion policy by itself.

Primary native anchors in the Android HD binary:

| Function | Address | Role |
| --- | ---: | --- |
| `AbyssEngine::MeshCreate` | `0x6c4d4` | Allocates runtime `Mesh` buffers from counts and `vertexFormat`. |
| `AbyssEngine::MeshReadData` | `0x6c608` | Main AEM stream parser. |
| `AbyssEngine::MeshCreateFromFile` | `0x6d074` | Opens AEM, detects magic/version and dispatches mesh reads. |
| `AbyssEngine::Mesh::ReadEnhancedDataFromFile` | `0x6bbec` | Reads enhanced bounds and animation groups. |
| `AbyssEngine::ImageCreateRegionFromFile` | `0x6eeb8` | Reads AEI atlas rects and creates an `Image2D` quad. |
| `AbyssEngine::ImageCreateFromFile` | `0x6f4d4` | Reads AEI pixel payloads. |
| `AbyssEngine::TextureCreateFromFileIntern` | `0x6f7f4` | Uploads raw/compressed image data to GL. |
| `PaintCanvas::Image2DCreate` | `0x79c78` | Resolves texture resource id plus AEI region index. |
| `PaintCanvas::MeshCreate` | `0x79d5c` | Resolves mesh resource, material id and calls `MeshCreateFromFile`. |
| `PaintCanvas::MaterialCreate` | `0x79e4c` | Resolves texture ids and material/blend state. |

## Resource Chain

`ResourceMesh` and `ResourceTexture` do not parse file formats. They store
resource payload data used by `PaintCanvas`.

Resource record fields inferred from `PaintCanvas::FindResource` users:

- offset `+0`: `uint16_t resourceId`
- offset `+8`: cached runtime handle, with `-1` meaning unloaded
- offset `+12`: type-specific payload pointer

Known resource type tags:

- `2`: `ResourceTexture`
- `4`: `ResourceMesh`
- `6`: `ResourceMaterial`

Mesh load chain:

1. `PaintCanvas::MeshCreate(resourceId, out, cloneFlag)`
2. find `ResourceMesh`
3. create material from the material id stored in `ResourceMesh`
4. call `MeshCreateFromFile(path, outMesh, material)`
5. optionally convert to VBO and cache the mesh handle

Image2D load chain:

1. `PaintCanvas::Image2DCreate(image2dResourceId, out)`
2. payload low16 is texture resource id
3. payload high16 is AEI region index
4. ensure texture is loaded
5. call `ImageCreateRegionFromFile(texturePath, regionIndex, image2d)`

## AEM Header And Mesh Stream

Observed GOF2 HD files use `V4AEMesh`.

Sample V4 header:

```text
00  9 bytes  "V4AEMesh\0"
09  u8       vertexFormat
0A  u16      top-level mesh count
0C  ...      MeshReadData stream
```

Native version masks:

| Magic family | Mask |
| --- | ---: |
| `AEMesh` | `0x04` |
| `V2AEMesh` | `0x01` |
| `V3AEMesh` | `0x02` |
| `V4AEMesh` | `0x08` |
| `V5AEMesh` | `0x10` |

`vertexFormat` bits:

| Bit | Meaning |
| ---: | --- |
| `0x01` | positions/base mesh present |
| `0x02` | UV stream present |
| `0x04` | normal stream present |
| `0x08` | color stream present |
| `0x10` | index stream present |

V4 mesh stream shape:

```text
f32 pivot[3]                 # when versionMask & 0x1A
u16 indexCount               # when vertexFormat & 0x10
u16 indices[indexCount]
u16 vertexCount
f32 positions[vertexCount][3]
f32 uvs[vertexCount][2]      # when vertexFormat & 0x02
f32 normals[vertexCount][3]  # when vertexFormat & 0x04
f32 colors[vertexCount][4]   # when vertexFormat & 0x08
enhanced data                # when versionMask & 0x1A
u16 childMeshCount
child meshes...
```

Important correction: the V4 pre-index block is 12 bytes of `f32[3]` pivot
data. Hex-Rays prints this as an odd `&byte_9[3]` expression in places; it is
not a 9 byte field.

Legacy stream branches:

- `AEMesh` mask `0x04`: positions are `i16 x/y/z` converted to float.
- V2/V3-style low masks use scaled integer UV/normal/color streams.
- V4/V5 masks `0x18`: positions, UVs, normals and colors are float streams.
- UV V is flipped only when `Engine::enableShader` is true.

Enhanced data starts with:

```text
f32 boundsCenter[3]
f32 boundsRadius
animation groups...
optional V5 extra animation block
```

`ReadEnhancedDataFromFile` also remaps bounds axes after reading:

```text
stored.y = raw.z
stored.z = -raw.y
```

## AEI Header And Image2D Regions

AEI header:

```text
00  8 bytes  "AEimage\0"
08  u8       image format/type byte
09  u16      width
0B  u16      height
0D  u16      regionCount
0F  regionCount * (u16 x, u16 y, u16 w, u16 h)
... pixel payload for texture loading
```

`ImageCreateRegionFromFile` only needs the header and rect table. It creates a
4-vertex, 2-triangle quad with positions `(0,0)`, `(w,0)`, `(w,h)`, `(0,h)`
and UVs from `x/y/w/h` divided by atlas width/height.

Known AEI type byte upload routes:

| Type byte | Runtime format | Upload route |
| ---: | ---: | --- |
| `0x01`, `0x03` | `3` | raw RGBA |
| `0x81` | `6` | raw RGBA cubemap |
| `0x0D`, `0x0F` | `4` | PVRTC RGBA 2bpp |
| `0x10`, `0x12` | `5` | PVRTC RGBA 4bpp |
| `0x11`, `0x13` | `7` | ATC explicit alpha |
| `0x14`, `0x16`, `0x17`, `0x40`, `0x42` | `11` | ETC1 RGB8 OES |
| `0x20`, `0x22` | `8` | S3TC/DXT1 |
| `0x21`, `0x23` | `9` | S3TC/DXT3 |
| `0x24`, `0x26` | `10` | S3TC/DXT5 |

The mipmap flag is initially `(typeByte & 0x02) != 0`. Type `0x17` is a
special case that forces mipmaps off.

## Importer Implications

An AEM-to-GLB importer must not treat offset `0x18` as a universal vertex
count. In V4 samples, offset `0x18` is the start of `indexCount`.

A native-faithful import needs a sidecar or GLB extras for:

- source resource id and source path
- material resource id from `ResourceMesh`
- `ResourceMaterial` texture ids and blend mode
- pivot
- enhanced bounds
- child mesh tree
- animation groups
- native `vertexFormat`
- stream encodings and offsets

Geometry-only OBJ/GLB exports are useful for inspection, but they cannot
represent the full native loader state without that sidecar.

2026-06-30 importer update:

- `tools/aem_to_glb_fx.py` parses the recovered C++ resource tables in this
  decomp tree (`BuildResourceList.cpp` plus `LowResourceTable.cpp`) and joins
  matching meshes through `ResourceMesh -> ResourceMaterial -> ResourceTexture`.
- Joined material ids, texture ids, texture paths, decoded PNG paths and native
  blend policy are written to both GLB material extras and the adjacent
  `*.native.json` sidecar.
- GLB export now writes `COLOR_0` when the native AEM stream has colors.
- UV export is controlled by an explicit `--uv-policy` instead of a hidden
  hardcoded flip.
- Enhanced translation and scale key groups are exported as glTF animation
  channels when their key shape is known. Rotation groups remain sidecar-only
  until native axis order/units are verified.
- V2/V3 coverage is recorded by
  `analysis/gof2_aem_aei_native_loader_spec/aem_version_validation_report.json`
  in the main workspace: 127 V2 and 2 V3 samples parse with zero failures in
  the current validation pass.
