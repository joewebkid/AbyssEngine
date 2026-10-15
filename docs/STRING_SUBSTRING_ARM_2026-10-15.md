# String::SubString ARM recovery — 2026-10-15

## Evidence and behavior

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA instructions at `0x72ed8–0x72f64` take a hidden result pointer in
`r0`, `this` in `r1`, and `start/end` in `r2/r3`. If `end <= start`, the
result is empty. Otherwise the method allocates `end-start+1` UTF-16
elements, copies exactly `end-start` elements from `data+start`, appends
zero, constructs the result from that temporary String with `reverse=false`,
and destroys the temporary. It does not clamp `end` to `this->length`.
The copy-constructor instructions at `0x72564–0x7258c` call
`Set(unsigned short const *)`, so a zero embedded in the selected range
truncates the returned String even though the temporary records the full
range length.

The earlier C++ implementation clamped the range and returned its own
buffer directly. It disagreed with the original on embedded zero and
out-of-range requests. The new body follows the observed temporary-copy
lifecycle. Callers must supply in-range boundaries; this recovery does
not make an out-of-range request safe.

## Checks and limits

The existing ABI smoke now checks a normal slice, an empty slice and
the embedded-zero copy behavior. Native `gof2` and the ABI smoke pass,
as do 37 verifier tests (one skip). All 206 ARM translation units compile.
Focused source-shape/strict fuzzy changes from **58.4%/23.4%** to
**99.3%/94.2%** (61 original versus 60 recovered instructions),
without an exact match. The remaining normalized difference is instruction
ordering around the initial bound check and a register copy. The full
corpus compares 4,323/4,524 original functions at 93.55% average
source-shape and 82.59% strict fuzzy, with 201 missing, 123 extras and
zero skipped. The 2,499 linked-exact and 1,074 byte-exact `(unit,
symbol)` sets have zero gains or losses against the preceding full
report. The focused and full JSON reports are saved outside the checkout
in the parent `_work` directory as
`string_substring_variant_focused_20261015.json` and
`string_substring_full_20261015.json`.

The smoke runs reconstructed code with local dependencies. Original
Android/iOS runtime behavior has not been replayed on identical inputs;
in particular, no claim is made for `end > this->length` or invalid
`data` pointers. REA/IDA `Set(unsigned short const *)` at `0x723dc`
also revealed an empty-buffer dependency difference. It was corrected
in the subsequent [Set audit](STRING_SET_EMPTY_ARM_2026-10-15.md).
The next priority is an original-vs-recovery comparison for valid
UTF-16 inputs and object ownership paths.
