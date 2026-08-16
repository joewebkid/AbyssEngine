# Hud Post-Fire Progress Recovery

Date: 2026-08-16

## Evidence

This package restores the dense block immediately after the target-context
overlay in the Android HD `Hud::draw` body at `0x163b90`. The corresponding
`Hud::hudEvent`, `Hud::init`, `MGame`, `PlayerEgo`, `Mission`, `LevelScript`,
`Globals::hints`, and resource-loading paths were checked before naming the
fields.

The final pulsing string in this block is the campaign mining tutorial. It is
not a time-extender or boost notification. Android uses GameText `618`, the
campaign mission index `2`, the 64-bit `LevelScript+0x08` time, and byte
`0x21825d`. ELF symbols place `_ZN7Globals5hintsE` at `0x21824c`, proving that
the byte is `Globals::hints[0x11]`.

## State Map

| Android Hud field | Recovered meaning |
| --- | --- |
| `+0x276` | cloak progress active |
| `+0x277` | jump-drive progress active |
| `+0x278` | docking transfer progress active |
| `+0x279` | reverse transfer direction / unloading |
| `+0x27a` | show ordinary mission marker |
| `+0x464` | shared jump/cloak fade timer |
| `+0x468` | docking transfer fade timer |
| `+0x4c4` | mining tutorial pulse timer |

The local class still has a compact host layout. These names identify source
semantics; they do not claim that the current C++ members already occupy the
original 32-bit ARM offsets.

## Image And Layout Map

| Android Hud slot | Resource | Confirmed consumer |
| --- | ---: | --- |
| `+0x378` | `0x053a` | common progress panel |
| `+0x37c` | `0x0539` | symmetric jump/cloak fill |
| `+0x380` | `0x1f40` | docking mission marker |
| `+0x384` | `0x1f5f` | production mission marker |
| `+0x38c` | `0x1f41` | docking transfer fill |

`Layout+0x218` supplies the progress-fill vertical offset. `Layout+0x2c`
supplies the mining tutorial baseline offset. Both remain direct native layout
reads rather than replacement coordinates.

## Recovered Behavior

- Docking progress is visible only while the transfer event is active, the
  player is docked to a docking point, and the ship is alive. It uses GameText
  `3204` for loading and `3205` for unloading.
- The transfer bar uses `transferred / total`, reversed for unloading, and a
  1000 ms alpha fade. Mission type `174` receives the production marker;
  ordinary marked missions use the other marker except type `168`.
- Jump-drive progress has priority over cloak progress when both flags are set.
  Both multiply the source rate by `1.05`, clamp it at `1.0`, use a centered
  symmetric crop, and share a 1000 ms fade timer. Their strings are GameText
  `318` and `317` respectively.
- The mining hint appears only before `Globals::hints[0x11]` is set, outside
  mining and asteroid docking, during campaign mission `2`, after script time
  `12001`. GameText `618` pulses through a 2000 ms triangular alpha cycle.
- `Hud::hudEvent` event pairs `25/26` and `28/29` now start/stop jump and cloak
  progress. Event pairs `0x23..0x2a` now use named docking-transfer fields.
- `Globals::resetHints` now clears the real 59-byte global hint array with the
  native overlapping SIMD-store shape instead of dereferencing a null shim.

## Boundary

This is source-backed behavioral recovery, not a whole-function byte-match
claim. A host-side divide-by-zero guard was retained for malformed transfer
state even though the Android body assumes a positive total. The matching
`MGame` path that displays the completion dialogue and sets hint `0x11` is
still queued. Exact `Hud` tail layout, string temporary placement, register
allocation, and instruction scheduling remain ARM work.

## Validation

- UCRT64 native build: `libgof2.a` links successfully.
- ARM `_ZN3Hud4drawExxP9PlayerEgobjj`: `8.1%` fuzzy match, original `3223`
  instructions, local `1926`; not linked- or raw-byte-equal.
- ARM `_ZN3Hud8hudEventEiP9PlayerEgoi`: `6.0%` fuzzy match, original `1088`
  instructions, local `150`; not linked- or raw-byte-equal.
- ARM `_ZN7Globals10resetHintsEv`: `48.3%` fuzzy match, original `16`
  instructions, local `13`; the previous local trap body is gone, but the
  function is not byte-equal.

