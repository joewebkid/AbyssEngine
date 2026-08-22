# Hud Init Menu Resource-Chain ARM Audit

Date: 2026-08-22

## Scope

This package closes the resource and render contract around
`Hud::initHudMenu(int, Level *)` at Android address `0x1615c8`. The Android
IDA and Ghidra bodies, the recovered `Hud.c` first pass, the committed
`BuildResourceList` entries and the direct `Hud::drawMenu(int)` consumer were
cross-checked independently.

## Four Menu Modes

All four modes write the resulting Image2D handle to `Hud+0x35c`. The
resource IDs are consecutive, but their semantic order is not numeric mode
order:

| Mode | Confirmed contents | Action masks | Header Image2D | Resource chain |
| ---: | --- | --- | ---: | --- |
| `0` | equipment entry, wingmen, cloak, jump drive, fuel/cargo amount | `0x200`, `0x400`, `0x800`, `0x1000` | `0x4f5` (`1269`) | texture `0x274e`, atlas entry `0xa8` |
| `1` | mounted secondary-equipment rows with item amount | `0x2000`, `0x4000`, `0x8000`, `0x10000` | `0x4f6` (`1270`) | texture `0x274e`, atlas entry `0xa9` |
| `2` | four command rows, GameText `307/308/309/310` or `311` | `0x20000`, `0x40000`, `0x80000`, `0x100000` | `0x4f3` (`1267`) | texture `0x274e`, atlas entry `0xa6` |
| `3` | orbit, station, warp gate, route, programmed station and docking targets | `0x200000` through signed `0x04000000 << index` | `0x4f4` (`1268`) | texture `0x274e`, atlas entry `0xa7` |

The previous C++ used `0x4f4` for both modes 1 and 3. Android assigns decimal
`1270` to mode 1, and Ghidra independently resolves the same value as
`0x4f6`. The mode-1 call now creates `0x4f6`; no heuristic resource remap is
involved.

## iPad Geometry

The source-backed placement branch is retained without platform
approximation:

- mode 3 reads persisted `GameSettings+0x54` (`steerAnchorX`);
- modes 0, 1 and 2 read `GameSettings+0x58` (`fireAnchorX`) and subtract
  `112.5f` for iPad HD, `160.0f` for iPad Large, or `80.0f` otherwise;
- negative results clamp to zero before integer conversion;
- mode 3 shifts X by `Layout+0x28 - Hud+0x3c4`;
- iPad vertical packing is `(4 - count) * (Layout+0x1dc + Layout+0x30)`, with
  one additional `Layout+0x30` subtraction for mode 3;
- phone mode compacts orbit menus with five or more entries upward by one
  `Layout+0x30` row gap.

The final loops export at most ten translated button positions to
`Globals::sub_menu_buttons_x/y`.

## Draw Consumer

`Hud::drawMenu(int)` reads `Hud+0x35c` directly. It draws the selected header
at:

`x = menuOriginX + Hud+0x3d4 + Hud+0x3dc / 2`

`y = menuOriginY + menuOriginYBase + Hud+0x3cc / 2 - Layout+0x22c`

The surrounding top/middle/bottom frame remains `0x4cf/0x4d1/0x4d0`.
Mode 0 additionally draws the confirmed cargo/fuel gauge from Image2D
`0x537/0x538` and the `"X <amount>"` String.

## ARM Verification

- `Hud::initHudMenu`: `35.0%`, `1245/1188` target/base instructions.
- `Hud::drawMenu`: `84.3%`, `225/221` target/base instructions.
- Changing the mode-1 literal fixes runtime behavior but does not alter the
  normalized instruction-shape score.

The remaining `initHudMenu` mismatch is dominated by whole-frame local
placement: Android dynamically aligns the stack and shares the 16-byte
command table slot with String temporaries. Isolated `alignas(16)`, plain
`int[4]`, and native vector experiments all reduced similarity and were not
retained. The next attempt must treat the command table and full equipment
String cleanup graph as one lifetime package.

## Validation Boundary

This pass proves mode routing, resources, action masks, geometry and the draw
consumer. It does not claim byte identity for either function. The resource
table is evidence for Image2D selection only; the original proprietary atlas
payload is not stored in this repository.
