# Hud Pause Button ARM Pass

Date: 2026-08-17

## Scope

This package restores the native low-byte view of the Hud touch mask and the
source shape of Android `Hud::drawPauseButton()` at `0x1637a4`.

IDA and Ghidra agree on the complete behavior:

1. set the canvas color to opaque white;
2. test bit 0 of the byte at `Hud+0x284`;
3. select the pressed or idle pause Image2D;
4. draw it at the unsigned 16-bit coordinates stored at `+0x40a/+0x40c`.

## Runtime Contract

`Hud+0x284` remains a 32-bit action mask. Touch routing uses higher bits such
as `0x40`, `0x80` and `0x20000000`, so narrowing the field itself would be
incorrect. `Hud` and `HudArm32Layout` now expose the same storage through:

- `touchFlags`, the complete `uint32_t` mask;
- `touchFlagsLow`, its byte at `+0x284`, used by the pause draw path.

Both views are guarded by ARM-only offset assertions.

## Pause Resources And Geometry

| Role | Native field | Image2D / source |
| --- | --- | --- |
| pressed image | `Hud+0x2f4` | `0x4b9` |
| idle image | `Hud+0x2f8` | `0x4b8` |
| draw X | `Hud+0x40a` | `screenW - imageWidth - Layout+0x194` |
| draw Y | `Hud+0x40c` | `Layout+0x198` |
| hit-test | bit `0x1` | pause center plus `touchHalfExtent` |

The source uses separate pressed and idle return paths. With the recovered
NDK r18b flags this preserves the original `ldrb`, `lsls` and conditional image
loads without inline assembly or volatile code-generation tricks.

The adjacent action cluster remains mapped for later `Hud::draw` work:

| Mask | Action | Pressed / idle Image2D |
| ---: | --- | --- |
| `0x2` | boost | `0x4b3 / 0x4b2` |
| `0x4` | quick menu | `0x4bb / 0x4ba` |
| `0x8` | secondary weapon | `0x4bd / 0x4bc` |
| `0x10` | primary action | `0x4b5 / 0x4b4` |
| `0x40` | dock action | `0x4b1 / 0x4b0` |

This table confirms resource and mask routing only. It does not claim that the
corresponding branches inside the large `Hud::draw` body are byte-matched.

## ARM Result

`Hud::drawPauseButton` improves from `61.9%` to `92.7%` and is linked-exact.
The verifier compares 21 original instructions with 20 local instructions;
the displayed residual is after the tail branch and is limited to literal-pool
decoding/relocation shape. Raw object bytes are not exact.

Across all 48 verified Hud functions:

- average similarity rises from `67.9%` to `68.5%`;
- linked-exact functions rise from 20 to 21;
- raw-byte-exact functions remain 15.

## Validation

- UCRT64 native build links `libgof2.a` successfully.
- ARM corpus builds `201/204` translation units.
- The same three unrelated `SolarSystem *` versus integer system-index errors
  remain.
- `sizeof(Hud) == 0x53c` and both `+0x284` offset checks compile.

