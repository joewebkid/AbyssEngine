# GameRecord station-stack and wanted pointer ABI (2026-10-11)

## Evidence and change

The Android ARM `RecordHandler::recordStoreRead` creates a station
array and stores its pointer at `GameRecord + 0x5c`. It subsequently
reads that array while loading extended station-ship mods, and
`GameRecord::load` passes it to `Status::setStationStack`. The same
reader conditionally creates an `Array<Wanted *>` at `+ 0x1b4` in an
extended record. The original ARM `GameRecord::load` reads word 109
(`0x1b4`) as a pointer and copies its elements to `Status::wanted`.

The previous native reader wrote 64-bit pointers through 32-bit fields,
overlapping adjacent record slots. Both fields are now typed pointers
at the verified ARM offsets, with offset assertions. Aligned native
sidecars retain full-width pointers. Constructor, reader, and load use
the same accessors. The wanted copy now moves `Wanted *` elements
without narrowing them to 32 bits. File order and existing ownership
behavior were not changed.

## Verification

- Native `gof2` static-library build: passed.
- ARM object build: 206/206 translation units, no errors.
- Focused ARM verification: `GameRecord` 5 comparisons at 96.4%
  average source-shape and `RecordHandler` 62 at 88.8%. The affected
  `load` and `recordStoreRead` functions retain their previous
  source-shape and strict-fuzzy scores.
- Verifier tests: 36 run, 35 passed, one skipped.
- Full ARM corpus: 4,713 comparisons, 4,323/4,524 original functions,
  201 missing, 92.24% source-shape, 80.43% strict fuzzy, 2,353
  linked-exact and 1,008 byte-exact. No exact symbol pair was lost;
  there are no skipped or wrong-type symbols.

The full report is at
`A:/Galaxy on Fire new version/_work/record_stack_wanted_full_20261011.json`.
Existing build directories were reused and no new C: scratch copy
was created.

## Limits and next priority

A live save/reload round trip and complete executable link have not
been proven. In particular, the size and initialization of
`Status::wanted` at the point of element copy are not established by
these build and source-shape checks. The record's `+ 0x134` slot is
written as a `Ship *`, but its load destination `Status + 0x8c` is also
used as an integer counter elsewhere. Resolve that conflicting field
interpretation against the original ARM behavior before changing it.
Other raw record pointer slots likewise remain for a later audit.
