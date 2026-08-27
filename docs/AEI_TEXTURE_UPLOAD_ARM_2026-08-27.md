# AEI Texture Upload And Ownership ARM Pass

Date: 2026-08-27

## Scope

This pass recovered the Android HD texture resource path around:

- `AbyssEngine::TextureCreateFromFile` at `0x6f7cc`;
- `AbyssEngine::TextureCreateFromFileIntern` at `0x6f7f4`;
- `PaintCanvas::TextureCreate` at `0x79a34`;
- its callback-free wrapper at `0x79b00`.

Primary evidence was the Android Hex-Rays body in
`analysis/gof2_libgof2hdaa_full_ida.c`, checked against the existing
`ResourceTexture`, `Image`, `Engine`, and `PaintCanvas` layouts.

## Recovered Contract

`ResourceTexture` payload offset `+0x04` is a `float` creation scale. It must
not be converted from an integer before loading.

`PaintCanvas::TextureCreate` now:

1. invalidates the first two bound-texture cache slots;
2. resolves the texture resource by id;
3. returns its cached handle when present;
4. otherwise calls the native loader with the resource path and float scale;
5. stores and returns the created handle only after a successful load.

The `managed` boolean is passed unchanged by `TextureCreateFromFile` to the
internal loader. In managed mode without a caller-supplied record, the loader
creates an `AELoadedTexture`, appends it to `PaintCanvas+0x10`, and changes the
output from a GL id to the new array index.

The recovered 32-bit `AELoadedTexture` layout is:

| Offset | Field |
| ---: | --- |
| `0x00` | GL texture id |
| `0x04` | `String` source path |
| `0x10` | creation scale |
| `0x14` | cubemap flag |
| `0x15` | valid/restore flag |
| `0x18` | uploaded byte count |

Its size is `0x1c`. The offsets and size are protected by ARM-only static
assertions.

## Upload Branches

The native filtering and upload branches are restored for:

- raw luminance, RGB, and RGBA;
- PVRTC RGBA 2bpp and 4bpp;
- ATC explicit alpha;
- S3TC DXT1, DXT3, and DXT5;
- ETC1 RGB8 OES;
- six-face RGBA cubemaps in native face order.

Mip payload sizes use the format-specific Android formulas rather than one
generic `width * height / 2` approximation. The loader also uses the recovered
`Engine::enableShader`, `Engine::clampTextures`, `Engine::AnisotropyValue`,
`Engine::linearFilterFlag`, `Engine::ImageCount`, and texture-byte accounting
fields. Previous null global-pointer shims in this function were removed.

Managed upload errors preserve the native non-fatal record behavior: the
record receives `glId = -1` and `byteSize = 0`, while unmanaged GL errors set
`Engine::lastGlError`/`lastErrorPath` and return `-4`.

## Verification

Native UCRT64 build:

```text
cmake --build cmake-build-ucrt --target gof2 -- -k 0
result: libgof2.a linked successfully
```

ARM object build:

```text
compiled 201 / 204 translation units
```

The three failures are the pre-existing `SolarSystem *` versus `int` errors;
none is in the files changed by this package.

Focused ARM comparison:

| Function | Before | After |
| --- | ---: | ---: |
| `TextureCreateFromFile` | `43.5%`, 10 instructions | `100.0%`, 13/13 instructions |
| `TextureCreateFromFileIntern` | `12.1%`, 441 instructions | `30.3%`, 678/730 instructions |
| callback-free `PaintCanvas::TextureCreate` | `100.0%` | `100.0%` |
| main `PaintCanvas::TextureCreate` | `69.8%`, semantically wrong | `50.4%`, source-backed |

The lower main-wrapper score is recorded, not hidden: typed direct calls and
correct handle lifetime changed register allocation. It is functionally closer
to the recovered body but still needs a dedicated source-shape pass. Only the
two small wrapper functions are normalized ARM matches; raw bytes are not yet
claimed identical.

## Remaining Work

- Recover the compiler source shape and local lifetime of the main
  `PaintCanvas::TextureCreate` without reintroducing its old zero-handle bug.
- Align the large loader stack frame (`0x44` currently versus native `0x34`)
  and switch/local lifetime while retaining the confirmed format formulas.
- Exercise suspend/reload with a real GLES context and representative raw,
  compressed, mipmapped, and cubemap AEI assets.
- Continue the dedicated source-shape pass recorded in
  `AEI_IMAGE_PARSER_ARM_2026-08-27.md`: the parser bodies are now source-backed
  and the region function is `81.0%`, but neither is byte-exact.
