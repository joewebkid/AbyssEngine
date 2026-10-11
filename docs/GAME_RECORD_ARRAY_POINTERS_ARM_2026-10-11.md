# GameRecord array pointer ABI audit (2026-10-11)

## Evidence and change

The Android ARM `GameRecord` is 0x1c1 bytes. `RecordHandler::recordStoreRead`
places arrays at offsets `0x160`, `0x168`, `0x164`, `0x170`, `0x16c`,
`0x174`, `0x180`, `0x184`, `0x178`, and (in the extended record) `0x17c`
as it reads the file. The four integer arrays' file order is deliberately
different from increasing struct-offset order. `GameRecord::load` reads
these slots as pointers and transfers data into `Status` or the void
station. The reader creates `Array<bool>` for the byte-sized flag arrays,
`Array<int>` for the four integer arrays, and item/ship pointer arrays
for `0x180`/`0x184`.

Previously, the fields were raw `uint32_t` on both targets. The native
reader wrote 64-bit pointers through those 32-bit slots and in doing so
overwrote neighboring fields. The slots now hold typed pointers at the
same ARM offsets, with `static_assert` checks. On native 64-bit builds,
the original 32-bit layout slots remain, while aligned sidecars hold
the full pointers. Constructor, reader, and load all use those pointers.
The file format, array counts, read order, and ownership behavior were
not changed.

## Verification

- Native `gof2` static-library build: passed.
- ARM object build: 206/206 translation units, no errors.
- Focused ARM verification: `GameRecord` 5 comparisons, 96.4% average
  source-shape; `RecordHandler` 62 comparisons, 88.8% average. The
  affected `GameRecord` constructor/load functions retain their prior
  source-shapes; `recordStoreRead` changes from 91.4% to 91.5%.
- Verifier unit tests: 36 run, 35 passed, one skipped.
- Full ARM corpus: 4,713 comparisons; 4,323/4,524 original functions
  covered, 201 missing, no skipped or wrong-type symbols. Average
  source-shape 92.24%, strict fuzzy 80.43%, 2,353 linked-exact and
  1,008 byte-exact. Comparing exact symbol pairs with the previous
  corpus found zero losses.

The full report is stored outside the repository at
`A:/Galaxy on Fire new version/_work/record_arrays_full_20261011.json`
to avoid filling C:. Existing build directories were reused.

## Limits and next work

This closes the pointer-width defect only for these ten slots. A live
save/reload round trip and complete executable link are not yet proven.
Other raw pointer fields in the same record, notably the station stack
at `0x5c`, a ship at `0x134`, and the wanted array at `0x1b4`, still
need an ARM-backed native ABI audit. The current source also has casts
between `Array<bool>` and `Array<uint8_t>` in load; they preserve the
existing byte-level behavior but have not been validated by runtime
save/reload testing.
