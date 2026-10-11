# Record core pointers and save-writer audit (2026-10-11)

## Evidence and change

`RecordHandler::recordStoreRead` creates two `Mission *` objects at
`GameRecord + 0x54/0x58`, an integer medal buffer at `+ 0x60`, four
`Array<bool> *` values at `+ 0x68/0x6c/0x84/0x8c`, and an
`Array<int> *` at `+ 0x70`. `GameRecord::load` uses these as pointers.
Previously, the native reader wrote 64-bit pointers through 32-bit
slots, overwriting adjacent fields. These eight ARM slots are now typed
and statically checked; native 64-bit builds hold the pointers in
aligned sidecars. Constructor, reader, and load use the same accessors.

The Android ARM save writer reads the reroute counter at `Status + 136`
(`0x88`) and tests/dereferences the previous ship at `Status + 140`
(`0x8c`). The C++ reconstruction instead wrote the address of
`Status::field_90` as the counter and cast `Status::field_94` (a boolean
array) to `Ship *`. Both sources are now corrected. The existing file
field order, value widths, and array element loops were kept.

## Verification

- Native `gof2` static-library build: passed.
- ARM object build: 206/206 translation units, no errors.
- Focused ARM verification: `GameRecord` 5 comparisons at 96.4%
  average source-shape; `RecordHandler` 62 at 88.8%. No affected
  function changed its source-shape or strict-fuzzy score, and no
  focused exact pair was lost.
- Verifier tests: 36 run, 35 passed, one skipped.
- Full ARM corpus: 4,713 comparisons, 4,323/4,524 original functions,
  201 missing, 92.24% average source-shape, 80.43% strict fuzzy,
  2,353 linked-exact and 1,008 byte-exact. Per-symbol comparison with
  the previous corpus found zero exact-pair losses, skipped units, or
  wrong-type symbols.

The full report is at
`A:/Galaxy on Fire new version/_work/record_core_pointers_full_20261011.json`.
Existing build directories were reused; no new C: scratch copy was
created.

## Limits and next priority

The native library has not been linked as a complete executable, and
no live save/reload round trip was exercised. The fact that pointer
types and source-shapes now match does not prove every save-file value
or array capacity is valid at runtime. Next, pursue an executable link
and a controlled save/reload fixture, then audit any remaining record
reader/writer mismatches revealed by that test.
