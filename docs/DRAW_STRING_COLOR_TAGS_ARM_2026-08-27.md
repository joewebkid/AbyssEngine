# DrawStringColor And SplitTags ARM Audit

Date: 2026-08-27

## Scope

This package restores the native inline-color string route:

- `AbyssEngine::String::SplitTags(String)`
- `PaintCanvas::DrawStringColor`
- typed handoff to `ImageFontDrawString`
- text-width accumulation and canvas color restoration

The glyph parser and ordinary draw runtime are documented in
`AEI_FONT_ATLAS_PARSER_ARM_2026-08-27.md` and
`IMAGE_FONT_DRAW_RUNTIME_ARM_2026-08-27.md`.

## Native Anchors

Android HD ARMv7:

| Function | Address |
| --- | ---: |
| `String::SplitTags(String)` | `0x73368` |
| `PaintCanvas::DrawStringColor` | `0x78274` |
| `PaintCanvas::GetColor` | `0x783d4` |
| `PaintCanvas::GetTextWidth(String const&)` | `0x7842c` |
| `PaintCanvas::SetColor(unsigned int)` | `0x78460` |

Both Android IDA and Ghidra bodies agree on the control flow and constants.

## Tag Grammar

The previous recovery incorrectly split on `|`. The native code passes tag
name `c` to `String::SplitTags`, which expands it to the prefix:

```text
<c:
```

The value ends at the next `>` character. A source such as:

```text
normal<c:ff8040ff>orange<c:>normal
```

becomes an alternating array:

```text
["normal", "ff8040ff", "orange", "", "normal"]
```

Even indexes are text segments. Odd indexes are color commands. The packed
32-bit value is parsed with `%x` and passed to `PaintCanvas::SetColor` as
`RRGGBBAA`.

An empty command (`<c:>`) restores the color saved at function entry. A
nonempty command that cannot be parsed leaves the zero-initialized value and
therefore selects packed color zero, because the native code ignores the
`sscanf` return value.

## SplitTags Contract

`String::SplitTags` now has its native typed return:

```cpp
Array<String *> *SplitTags(String tag);
```

It returns null when the source, tag, or matching result is empty. For every
matching prefix it appends the preceding text, finds the next `>`, then
appends the raw value. A malformed opening tag without `>` returns the partial
array already created by the function. The caller owns every String, the
array data, and the array object.

## DrawStringColor Contract

For a valid font index, the function:

1. binds the font atlas texture
2. saves the current packed canvas color
3. copies the input String
4. splits it with `SplitTags(String("c"))`
5. alternates between drawing text and applying color commands
6. advances X by `GetTextWidth` after every text segment
7. restores the saved color after an empty command and after the complete loop
8. releases all split String objects and the array

The draw call uses the length-bearing `ImageFontDrawString` overload, so an
embedded segment is not required to be null-terminated for traversal.

The native function has no ordinary `DrawString` fallback when `SplitTags`
returns null. This API is therefore a tagged-string path, not a transparent
replacement for the plain draw overload.

The native path obtains a temporary narrow color string through `GetAEChar`
and does not show a matching release in the inspected body. The recovered C++
keeps that source behavior; a modern Godot parser should avoid the allocation
rather than reproduce its lifetime.

## Removed Shims

`PaintCanvas::DrawStringColor` no longer depends on the opaque
`paintcanvas_ext_dsc_*` family for texture binding, color access, String
construction, splitting, width, font draw, cleanup, or `%x` parsing. The
function now uses the typed engine classes directly.

The old `PCFontView`, `PCSplitArrayView`, and `PCStrPartView` overlays were
removed because their fields are represented by the recovered `ImageFont`,
`Array<String *>`, and `String` types.

## Verification

Native UCRT64:

```text
cmake --build cmake-build-ucrt --target gof2 -- -k 0
libgof2.a linked successfully
```

ARM corpus: `201/204`; the three failures remain the unrelated existing
`SolarSystem *` migration errors.

Focused results:

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `PaintCanvas::DrawStringColor` | `18.5%` | `93.3%` | `136/132` |
| `String::SplitTags` | `18.0%` | `76.3%` | `184/183` |
| `GetTextWidth(String const&)` | `100%` linked-exact | `100%` linked-exact | `17/17` |
| ranged `GetTextWidth` | `100%` linked-exact | `100%` linked-exact | `23/23` |
| `PaintCanvas::GetColor` | `93.9%` | `93.9%` | unchanged |
| `PaintCanvas::SetColor(uint)` | `92.1%` | `92.1%` | unchanged |

`DrawStringColor` now has the exact target stack allocation and differs mainly
in parameter-move ordering plus the terminal literal/unwind representation.
It is source-backed but not linked- or byte-exact. `SplitTags` has a one-
instruction count difference; its remaining delta is register and exception-
cleanup shape. No artificial stack locals or assembly were added.

## Godot Boundary

The Godot bitmap-font path can now implement the native tag contract without
guessing: parse `<c:RRGGBBAA>`, preserve the current color across segments,
restore on `<c:>` and after the line, and measure each visible substring with
the recovered ImageFont advance rules. Godot's BBCode parser may be used as an
implementation detail, but the source syntax and packed alpha order must be
translated explicitly.

The next dense native font package is the line-layout family:
`GetLine`, `GetLineArray`, `DrawTextLines`, text-height accumulation, alignment
and clipping. That package determines wrapping and multi-line placement.
