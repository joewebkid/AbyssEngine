# ImageFont Draw Runtime ARM Audit

Date: 2026-08-27

## Scope

This pass restores and verifies the runtime side of the AEI font pipeline:

- both `ImageFontGetWidth` overloads
- both `ImageFontDrawString` overloads
- spacing, Y-offset and height accessors
- `PaintCanvas::FontCreate`
- the PaintCanvas draw wrappers and world-view matrix route

The parser and glyph-mesh ownership are covered separately in
`AEI_FONT_ATLAS_PARSER_ARM_2026-08-27.md`.

## Native Anchors

Primary Android HD ARMv7 addresses:

| Function | Address |
| --- | ---: |
| `ImageFontDrawString(font, text, len, x, y, canvas, engine, rtl)` | `0x71bc0` |
| `ImageFontDrawString(font, text, x, y, canvas, engine, rtl)` | `0x71b80` |
| `ImageFontGetWidth(font, text, count)` | `0x7213e` |
| `ImageFontGetHeight` | `0x721ba` |
| `ImageFontGetYOffset` | `0x721da` |
| `ImageFontGetWidth(font, text, start, len)` | `0x721e6` |
| `ImageFontSetSpacing` / `ImageFontGetSpacing` | `0x72266` / `0x7226c` |
| `ImageFontSetYOffset` | `0x72278` |
| `PaintCanvas::SetWorldViewMatrix` | `0x78810` |
| `PaintCanvas::FontCreate` | `0x79b14` |

The iOS HD binary independently exposes the common draw body at
`FUN_00326304`, width at `FUN_00326d94`, height at `FUN_00326e98`, substring
width at `FUN_00326f0c`, and the Y-offset family around
`FUN_00327014/FUN_00327038`.

## Advance And Width

For every UTF-16 code unit, the runtime linearly searches the font code table.
If a glyph exists, its width is read from the second vertex X coordinate in
the glyph mesh. The ordinary advance is:

```text
glyphWidth + font.spacing
```

There is one confirmed special case: code point `0x20` with a glyph width of
`11` subtracts two pixels from the advance. Missing glyphs add no width.

The substring overload keeps the text cursor as a 16-bit value. This source
shape is required for the recovered function to match the Android ARM body.

## Baseline, Clipping And Direction

The draw baseline is:

```text
y + font.yOffset - 2
```

The full string is rejected when its measured right edge is left of the
screen. Vertical clipping uses `ImageFontGetHeight`, `font.yOffset`, and the
engine display height. Individual glyphs are drawn only when their horizontal
span intersects the display width.

The default text traversal is reverse: `len - 1` to zero. It changes to
forward order when either `PaintCanvas+0x1c` is nonzero or the draw call's
direction flag is true. In the shader path, language `9` also forces forward
order for a string classified by `GameText::isNonArabicString`.

This is traversal order, not a recovered kerning table. No pair-kerning lookup
exists in the inspected native bodies.

## Shader Draw Path

When `Engine::shaderModeFlag` is enabled, visible glyphs are appended to
`PaintCanvas::quad2dMesh`:

- four translated positions are copied per glyph
- eight UV words are copied from the glyph mesh
- current engine RGBA is copied to all four vertices
- six indices are enabled per glyph
- the batch flushes at 100 glyphs and again for the final remainder
- each flush uses an identity world-view matrix and `MeshDraw`

The batch cursor is `PaintCanvas+0x0c`. This explains why the native draw body
does not submit one mesh per character on the shader route.

## Fixed-Function Draw Path

The non-shader path keeps a local 15-float affine matrix. For every visible
glyph it writes X translation and the recovered baseline translation, calls
`PaintCanvas::SetWorldViewMatrix`, then submits the glyph mesh. The previous
recovery incorrectly reused the canvas matrix and therefore lost per-glyph
translation.

`PaintCanvas::SetWorldViewMatrix` now directly forwards the supplied matrix to
`Engine::SetWorldViewMatrix`. The focused verifier remains linked-exact for
this wrapper.

## FontCreate Ownership

`PaintCanvas::FontCreate` follows the native resource chain:

1. find `ResourceFont`
2. read the texture resource id and font-set index from its payload
3. find or create the referenced texture
4. reuse an already loaded font handle when present
5. call `ImageCreateFontFromFile`
6. attach the texture handle to `ImageFont+0x08`
7. append the font with the real `ArrayAdd<ImageFont *>` route
8. update resource/output handles and default-font selection

Replacing the local array shim with the native template route reduced the ARM
base from 109 to 86 instructions against an 89-instruction target.

## Verification

Native UCRT64 build:

```text
cmake --build cmake-build-ucrt --target gof2 -- -k 0
libgof2.a linked successfully
```

ARM corpus build: `201/204`. The three failures are the existing unrelated
`SolarSystem *` migration errors.

Focused final results:

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| full `ImageFontDrawString` | `9.9%` | `17.1%` | `474/394` |
| null-terminated draw wrapper | `50.0%` | `50.0%` | `22/22` |
| width by count | `52.2%` | `97.8%` | `45/45` |
| substring width | `46.2%` | `100%` byte-exact | `47/47` |
| `PaintCanvas::FontCreate` | `37.4%` | `50.3%` | `89/86` |
| `PaintCanvas::SetWorldViewMatrix` | `100%` linked-exact | `100%` linked-exact | `2/2` |

`ImageFontSetSpacing` and `ImageFontSetYOffset` also remain byte-exact.

## Remaining Boundary

The full draw body is source-backed from Android and iOS, but is not byte- or
linked-exact. The remaining differences are compiler-visible stack layout,
VFP save sets, copy lowering and local lifetime. No artificial stack scratch
or inline assembly was added.

The color-tag follow-up is complete in
`DRAW_STRING_COLOR_TAGS_ARM_2026-08-27.md`. `PaintCanvas::DrawStringColor` now
restores native `<c:RRGGBBAA>` parsing and color lifetime and reaches `93.3%`
at `136/132`; `String::SplitTags` reaches `76.3%` at `184/183`.

Pixel-exact Godot text still needs these runtime rules to be applied by its
bitmap-font renderer rather than approximated with ordinary dynamic-font
metrics. The next native boundary is multi-line wrapping and alignment through
`GetLine`, `GetLineArray`, and `DrawTextLines`.
