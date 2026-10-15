# String float object composition (Android ARM, 2026-10-15)

## Evidence and change

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
uses local `String` objects in `String::Set(float)` beginning at `0x72cb0`.
After its digit-helper call at `0x72cd6`, it initializes an accumulator,
uses `"0."` and repeated `"0"` appends for nonpositive exponents
(`0x72dc2–0x72dfc`), appends a temporary UTF-16 digit `String`
(`0x72cf4–0x72d08`), inserts `"."` with two `SubString` calls and
`operator+` (`0x72d10–0x72d68`) for positive exponents, and prefixes
`"-"` through `operator+` when the sign flag is set (`0x72d6a–0x72d94`).
It then deletes the helper buffer, copies the accumulator into the
destination and destroys the accumulator.

The previous local method assembled the output in a separate manually
allocated UTF-16 array. It now follows the observed temporary-object,
append, substring, assignment and destruction sequence. The isolated ABI
smoke exercises the production `Set(float)` body and uses the same
four-line `operator+` implementation as the production engine file. It
checks positive, negative, zero, fractional and small-fraction results,
plus the `String::operator+=(float)` caller path.

## Verification and limits

Native `gof2` and the ABI/lifetime smoke pass; 206/206 ARM translation
units and 37 verifier tests (one skipped) pass. Focused `Set(float)`
source-shape/strict fuzzy improves **43.4%/12.3% → 96.6%/65.6%**;
it remains neither linked-exact nor byte-exact. Focused reports are in
the shared workspace `_work/` directory as
`float_set_{after,object_after}_20261015.json`.

The result confirms closer ARM control flow and tested host formatting,
not complete semantic equivalence. The helper's exceptional float values
and rounding boundaries, live ARM32 execution, and the remaining codegen
differences of `Set(float)` are not yet verified.

## Full-corpus gate

The fresh shared-workspace report `_work/float_set_object_full_20261015.json`
compares **4,323/4,524** unique original functions across 4,720
comparisons, with **201 missing**, **123 extras**, zero skipped and zero
wrong-type. Average source-shape/strict fuzzy is **93.57%/82.61%**.
Against `_work/float_helper_full_20261015.json`, both `(unit, symbol)`
exact sets are unchanged with zero losses or gains: **2,499 linked-exact**
and **1,074 byte-exact**. The corpus gate does not prove live behavior.
