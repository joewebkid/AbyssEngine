# AEI Font Atlas Parser ARM Audit

Date: 2026-08-27

## Scope

This pass restores and verifies:

- `AbyssEngine::ImageCreateFontFromFile`
- `AbyssEngine::ImageFontRelease`
- the AEI font metadata tail and per-glyph mesh geometry

The Android ARM body is the primary source. The iOS body and the extracted
font assets are independent cross-checks.

## Native Sources

| Source | Function | Address |
| --- | --- | ---: |
| Android IDA | `ImageCreateFontFromFile` | `0x6f13c` |
| Android IDA | `ImageFontRelease` | `0x7227e` |
| iOS Ghidra | font loader (`FUN_00327828`) | `0x00327828` |

The Android body proves the complete stream order, allocation sizes, glyph
quad layout and failure paths. The iOS body independently confirms the common
`AEimage\0` header, font-set selection and `x/y/width/height` records.

## Recovered File Layout

The font loader first reads the ordinary AEI header and skips its atlas region
table:

```text
00  char[8]  "AEimage\0"
08  u8       type
09  u16      atlasWidth
0b  u16      atlasHeight
0d  u16      regionCount
0f  regionCount * (u16 x, u16 y, u16 width, u16 height)
... texture pixel payload
... u16      fontSetCount
```

Pixel-payload skipping follows the native type-byte branches:

- raw `0x01` and `0x03`: `4 * atlasWidth * atlasHeight` bytes
- length-prefixed `0x0d, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x20, 0x21`
- length-prefixed `0x24, 0x40, 0x42`
- an unrecognized type has no additional payload skip in this function

Each appended font set is:

```text
u16 glyphCount
u16 codePoints[glyphCount]
struct GlyphRect {
    u16 x;
    u16 y;
    u16 width;
    u16 height;
} glyphRects[glyphCount]
```

An unselected set is skipped as exactly `10 * glyphCount` bytes after its
count. The selected set is converted into one four-vertex `Mesh` per glyph.

## Runtime Layout And Geometry

The recovered ARM32 `ImageFont` is `0x14` bytes:

| Offset | Field |
| ---: | --- |
| `0x00` | `u16 glyphCount` |
| `0x04` | `u16 *codes` |
| `0x08` | unknown/unused pointer-sized field |
| `0x0c` | `Mesh **glyphMeshes` |
| `0x10` | `i16 spacing` |
| `0x12` | `i16 yOffset` |

Each mesh is created with `MeshCreate(engine, 4, 2, 0x13, &mesh)` and receives:

```text
positions: (0,0), (width,0), (width,height), (0,height)
uv:        (x,y), (x+width,y), (x+width,y+height), (x,y+height)
indices:   0,2,1, 0,3,2
```

UV coordinates are divided by `atlasWidth` and `atlasHeight`. The packed
index words are `0x00020000`, `0x00000001`, `0x00020003`.

The previous reconstruction mixed rectangle fields, swapped atlas
denominators, used `offX` for both UV axes and emitted a degenerate second
triangle. Those errors are removed.

`ImageFontRelease` frees the UTF-16 code table, releases every glyph mesh,
frees the mesh-pointer table, deletes the font and clears the caller slot.

## Real Asset Validation

The restored layout was checked with the existing
`tools/extract_gof2_font_atlas.py` validator against the extracted Android/iOS
texture set. Results:

- 21 AEI font/interface atlas variants parsed
- 23,370 glyph metric records parsed
- every glyph rectangle stays inside its atlas
- every file was consumed through the last metadata byte
- all standalone language atlases contain one font set
- each `gof2_interface*.aei` variant contains two sets: 272 and 26 glyphs

Representative dimensions and counts:

| Family | Atlas variants | Glyphs per set |
| --- | --- | ---: |
| Arabic/main | `512x256`, `512x512` | 272 |
| Simplified Chinese | `1024x1024`, `2048x1024`, `2048x2048` | 2184 |
| Traditional Chinese | `1024x1024`, `2048x1024`, `2048x2048` | 2115 |
| Japanese | `1024x1024`, `2048x1024`, `2048x2048` | 1747 |
| Korean | `1024x512`, `1024x1024`, `2048x1024` | 1155 |
| Language select | `128x128`, `256x256` | 19 |
| Interface | `1024x1024`, `2048x2048` | 272 + 26 |

No original assets are added to this repository by this validation.

## Verification

Native UCRT64:

```text
cmake --build cmake-build-ucrt --target gof2 -- -k 0
```

Result: `libgof2.a` links successfully.

ARM object corpus:

```text
GOF2_VERIFY_LOCAL_NDK=1 GOF2_VERIFY_JOBS=4 tools/verify/build_objs.sh cmake-build-match/verify
```

Result: `201/204`; the same three unrelated `SolarSystem *` errors remain.

Focused match results:

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `ImageCreateFontFromFile` | `44.3%` | `57.6%` | `314/314` |
| `ImageFontRelease` | `88.3%` | `88.3%` | `40/37` |

The parser is source-backed and has the exact target instruction count, but it
is not linked- or byte-exact. Remaining differences are chiefly stack-slot,
VFP save-set and register-lifetime shape. No artificial assembly or dummy
stack locals were added to inflate the score.

## Next Boundary

The runtime follow-up is complete in `IMAGE_FONT_DRAW_RUNTIME_ARM_2026-08-27.md`.
It restores advance, spacing, direction, baseline/Y offset, clipping, shader
batching, vertex color and the fixed-function matrix route. The remaining
independent font boundary is `PaintCanvas::DrawStringColor` tag parsing and
temporary color-state lifetime.
