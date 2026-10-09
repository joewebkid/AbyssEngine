# Status standing pointer contract (2026-10-09)

The Android 2.0.16 ARM `Status::getStanding()` at `0x000ac720` is just
`ldr r0, [r0, #20]; bx lr`: it returns the value of the `Standing *` field
at `Status + 0x14`. The former C++ `int` return cast that pointer to 32 bits.
On a 64-bit native host, callers casting that integer back to `Standing *`
could dereference a truncated address.

`Status::getStanding()` now returns `Standing *`, and every direct source
caller preserves that pointer. The stored field was already `Standing *`;
its ARM32 offset is unchanged. The existing compile-time pointer-contract
smoke now checks both the field and getter types.

Validation: native `gof2` builds, all 206 ARM translation units compile,
the pointer-contract smoke compiles, 36 verifier tests run (one optional
skip), and four scope tests pass. Focused ARM comparison keeps the getter
**linked-exact and byte-exact** at 2/2 instructions. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_standing_pointer_20261009.json`:
**4,323/4,524** original functions across **4,689** rows, **91.87%**
source-shape, **79.93%** strict fuzzy, **2,313 linked-exact** and
**997 byte-exact**. There are 201 absent originals, zero wrong-type misses
and zero skipped units. Compared by `(unit, symbol)` with the preceding
wanted-system report, comparison rows and both exact-pair sets have zero
losses and zero gains.

This repairs the native pointer-width contract, not the many other 32-bit
fields in `Status`. Whole-game runtime and all standing-related gameplay
paths remain untested.
