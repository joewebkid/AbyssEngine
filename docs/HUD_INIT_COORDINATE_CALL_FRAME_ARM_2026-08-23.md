# Hud init coordinate/call-frame ARM pass (2026-08-23)

## Evidence

- Android 2.0.16 `libgof2hdaa.so`, `_ZN3Hud4initEv` at `0x1604e4`.
- IDA first-pass `Hud.c` and the independent Ghidra Android body.
- Focused ARM object comparison after every accepted source-shape change.

## Accepted reconstruction

- Replaced the local image-width/image-height lambdas with always-inlined direct
  `PaintCanvas` calls. The lambdas had survived as separate ARM functions and
  inserted non-native `bl` instructions throughout the coordinate block.
- Restored fresh `Globals::w` and `Globals::h` reads where the Android body
  reloads the globals, while retaining the two proven local snapshots used by
  the hacking-control and quick-menu branches.
- Re-expressed the `Layout+0x12c..0x14b` transfer as the four ordered 64-bit
  values visible in IDA. It still copies exactly 32 bytes into
  `Hud+0x4d4..0x4f3`.
- Restored the iPad call-frame lifetime: read the steer anchor, query the three
  image widths, call `setCoordsSteer`, copy both steering-center halfwords,
  then read the fire anchor and a fresh `Globals*` before `setCoordsFire`.
  Persisted steer/fire anchors are read again after the calls.
- Restored the source order of the cloak/boost ready bytes and the exact
  hacking-panel half-width recurrence.
- Kept the confirmed raw-bit fire-power comparison. `Ship::getFirePower()` is
  byte-exact as a two-instruction load from `Ship+0x48`, while Android IDA uses
  `COERCE_FLOAT` at the Hud call site. Changing the shared Ship field type is
  therefore deferred to a separate ABI audit.

## ARM result

| Checkpoint | Match | Target/base instructions | Target/base size |
| --- | ---: | ---: | ---: |
| Before this pass | 64.9% | 1077 / 1021 | not re-recorded |
| Remove non-native metric lambdas | 73.2% | 1077 / 1043 | `0xd88 / 0xd2c` |
| Restore global-read and steer-copy order | 76.5% | not retained | not retained |
| Restore four-word layout copy | 78.1% | 1077 / 1071 | not retained |
| Restore iPad call-frame lifetimes | 81.1% | 1077 / 1068 | `0xd88 / 0xd74` |

The ARM stack reservation is now exact at 188 bytes. The complete 48-function
Hud set averages 89.5%, with 26 linked-exact and 19 byte-exact functions.
The corpus remains 201/204 because of the same three unrelated `SolarSystem *`
typing failures.

## Remaining boundary

- `Hud::init` is source-backed but not byte-exact. Remaining differences are
  concentrated in compiler register allocation, the NEON load/store element
  spelling for the 32-byte copy, literal-pool placement, and scheduling in the
  final queue/String cleanup tail.
- No artificial stack scratch, volatile register forcing, inline assembly or
  padding was added. Further work should start from the shared `Ship+0x48`
  type audit or from a compiler/toolchain provenance check, not from local
  instruction padding.

## Build verification

- UCRT64 `libgof2.a`: successful.
- Focused `_ZN3Hud4initEv`: 81.1%.
- Full Hud snapshot: 48 functions, 89.5% average.
