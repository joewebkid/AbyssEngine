# Previous-ship and Status offset audit (2026-10-11)

## ARM evidence and corrected interpretation

Android ARM `GameRecord::load` stores record word 77 (`0x134`) at
`Status + 140` (`0x8c`). The record reader constructs a `Ship` in that
slot and later calls `setMods` through it, so this is a ship pointer.
The ARM `Status::departStation` increments word 34 (`0x88`), not word
35 (`0x8c`). Two original menu-start paths store the preset value 6666
at `Status + 132` (`0x84`). These are three distinct fields. The
previous C++ reconstruction had routed the reroute counter and menu
preset to `0x8c`, conflating them with the ship pointer.

`GameRecord + 0x134` and `Status + 0x8c` now hold typed `Ship *` on
32-bit ARM; aligned sidecars hold full-width native pointers. The
reader, record load, constructors, and reset path use those pointers.
The station-departure counter uses `field_88`, and both menu presets
use `field_84`. ARM offset and native alignment assertions guard the
layout. No save-file order or ownership change was made.

## Verification

- Native `gof2` static-library build passed; ARM object build compiled
  206/206 translation units with no errors.
- Focused ARM verification passed for `GameRecord` (5 comparisons),
  `RecordHandler` (62), `Status` (139), and `MenuTouchWindow` (48).
  `Status::departStation` source-shape changed 83.8% to 84.0%; menu
  `startSupernova` 91.3% to 91.4%, `startValkyrie` 89.0% to 89.1%.
  No focused exact pair was lost. An added compile-time assertion for
  `Status + 0x84` was rebuilt and focused-verified afterward; it does
  not generate executable code.
- Verifier tests: 36 run, 35 passed, one skipped.
- Full ARM corpus: 4,713 comparisons, 4,323/4,524 original functions,
  201 missing, 92.24% average source-shape, 80.43% strict fuzzy,
  2,353 linked-exact and 1,008 byte-exact. Comparison with the prior
  corpus found zero linked-exact or byte-exact symbol-pair losses,
  zero skipped units, and zero wrong-type symbols.

The full report is stored at
`A:/Galaxy on Fire new version/_work/record_previous_ship_full_20261011.json`.
Existing C: build directories were reused.

## Limits and next priority

The native library has not been linked as a full executable, and no
live save/reload or new-game route was exercised. Initialization of
the reroute counter in every new-game path and the lifetime of the
transferred ship remain unverified. Continue auditing remaining raw
record pointers and test a save/load route once executable linking is
possible; do not infer runtime correctness from source-shape alone.
