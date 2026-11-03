# Gamma damage rate return type (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `Status::getGammaRayDamagePerSecond(int,int)` at `0xacdc0`
returns `float`. Its table paths return floating-point values; the
Android ARM softfp calling convention passes their bits in `r0`.
The IDA output declares this function as `float` and uses
`COERCE_FLOAT` at callers, although its jump wrapper at `0x623dc`
was inferred as `int`. The recovered declaration and definition
previously used `int` and stored float bit patterns in integer tables.
Six callers then reinterpreted those integers through local unions.

The declaration, definition, tables, and six callers now use `float`
directly. This restores the source type and removes the manual bit
reinterpretation. It should not be read as evidence of a prior runtime
damage bug: the old integer and union route reproduced the expected
bits at those six call sites. The game's gamma behavior remains
unverified at runtime.

Focused ARMv7 verification compiles **209/209** translation units.
The target's source-shape changes **80.1% -> 75.9%** and strict fuzzy
**71.4% -> 71.2%**; generated instructions change from 25 to 28
against 31 original. The clearer floating-point source therefore
reduces this local shape score. An integer-bit-table conversion via
`memcpy` was also tested and gave identical focused scores, so the
direct float tables were retained. The 146-symbol `Status` unit keeps
**103 linked-exact** and **66 byte-exact** matches with no exact-set
loss. Focused report:
`_work/status_gamma_float_final_focused_20261103.json`.

The Windows native and ARM64 static `gof2` builds pass. The existing
source-extracted player engine sound smoke passes with the corrected
float interface. The full ARM corpus compares **4,323/4,524**
original functions in **4,769** pairs, with 201 missing and zero
skipped. Mean source-shape and strict fuzzy remain **96.75% / 86.74%**
after rounding. Linked-exact remains **2,779** and byte-exact
**1,108**. There are no pair or exact-set losses against
`_work/status_campaign_sentinel_full_20261103.json`. The `Level::update`
strict fuzzy score rises from 75.7% to 76.1%; other affected callers
retain their scores. Full report:
`_work/status_gamma_float_abi_full_20261103.json`.

Next priority: inspect a bounded unresolved `Status` behavior with
original ARM evidence and a host check. Gamma damage behavior on an
actual device has not been tested in this package.
