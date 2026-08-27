# AEI Image Parser And Atlas Region ARM Pass

Date: 2026-08-27

## Scope

This package recovered the two parsers below from the Android HD body and
cross-checked their common file-format behavior against the iOS engine dump:

- `AbyssEngine::ImageCreateRegionFromFile` at Android `0x6eeb8`;
- `AbyssEngine::ImageCreateFromFile` at Android `0x6f4d4`;
- iOS Ghidra functions `FUN_00327220` and `FUN_003281a4`.

## Confirmed File Contract

Both functions require the complete 8-byte header `AEimage\0`. The old local
implementation compared the input against a four-asterisk placeholder and
therefore rejected real AEI files.

The common header is:

```text
8 bytes  magic: AEimage\0
u8       type
u16      atlas width
u16      atlas height
u16      region count
8*N      region records: u16 x, y, width, height
```

`ImageCreateFromFile` skips the region table and then reads the payload. The
Android routes are now source-backed for raw RGBA/cubemap, PVRTC 2bpp/4bpp,
ATC explicit alpha, DXT1/3/5, and ETC1. The initial mip flag is `type & 2`;
Android types `0x40` and `0x17` explicitly clear it. Unknown types preserve
the native successful empty-image behavior instead of being guessed.

The iOS body independently confirms the raw, cubemap, PVRTC, ATC and DXT
families. The ETC type family is Android-specific in the inspected builds and
is not claimed as an iOS upload route.

## Atlas Quad

`ImageCreateRegionFromFile` now calls the typed native ABI:

```cpp
MeshCreate(engine, 4, 2, 0x13, &region->mesh)
```

The generated positions are `(0,0)`, `(w,0)`, `(w,h)`, `(0,h)`. Rect origins
are normalized as unsigned values, while the far edge uses the native signed
16-bit interpretation of the stored offset plus unsigned size. Both Android
and iOS confirm the index order:

```text
0, 2, 1, 0, 3, 2
```

The previous implementation wrote zero for the final packed index word and
produced a degenerate second triangle. `Image2D` is now protected by ARM-only
size/offset assertions for its recovered `0x14` layout.

Error handling intentionally follows the Android body. Open/header/read/index
errors return `-1`, invalid arguments return `-4`, and mesh allocation failure
returns `-2`. A rect-table failure after mesh creation releases the mesh. The
early Android error paths do not all close the file handle; this native quirk
is preserved for source and control-flow fidelity rather than silently
rewritten.

## Verification

Native UCRT64:

```text
cmake --build cmake-build-ucrt --target gof2 -- -k 0
result: libgof2.a linked successfully
```

ARM corpus:

```text
compiled 201 / 204 translation units
```

The three failures are the existing `SolarSystem *` versus integer errors in
unrelated translation units.

Focused normalized ARM comparison:

| Function | Before | After |
| --- | ---: | ---: |
| `ImageCreateRegionFromFile` | `66.7%`, 214/221 instructions | `81.0%`, 216/221 instructions |
| `ImageCreateFromFile` | `32.5%`, 268/317 instructions | `34.0%`, 295/317 instructions |

Neither function is linked- or byte-exact. The region function is now close in
control flow and generated size. The larger payload parser has the confirmed
behavior and nearly the native instruction span, but switch placement, local
lifetimes and stack-slot allocation still differ. An attempted synthetic local
struct matched stack offsets but reduced the score to `31.0%`; it was rejected
and is not present in the source.

## Remaining Work

- Match the payload parser's natural local declaration/lifetime shape without
  artificial stack objects.
- Validate every payload family through a real GLES context, including mip
  chains and cubemap restoration.
- Audit the iOS texture uploader separately before sharing Android ETC policy
  with any iOS-specific port.
