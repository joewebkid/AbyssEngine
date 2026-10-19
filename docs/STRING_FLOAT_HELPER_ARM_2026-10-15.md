# String float digit helper (Android ARM, 2026-10-15)

## Evidence and recovery

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
defines `AbyssEngine::computeFloatString(float,int,int*,int*,int)` at
`0x72af8–0x72ca4`. At `0x72b12` it allocates 0x42 bytes (33 UTF-16 code
units), reports the exponent and sign through the two pointers, and returns
the allocation in `r0`. The precision is clamped to 0–30; integer digits
are generated backwards, fractional digits forwards, then one extra digit
is rounded. The integer-digit path uses a `double`-precision `+0.03`
correction. The five-argument call from `String::Set(float)` at `0x72cd6`
passes precision 10 and mode 1 on the stack (`movs r0,#1` at `0x72cce`,
`str r0,[sp,#0]` at `0x72cd0`). The earlier mode-0 attribution was incorrect;
the separate direct mode-0 helper smoke below does not prove Set's call argument.

The previous local helper had the same parameter types but returned `void`
and interpreted the value incorrectly. `String::Set(float)` called a
different, undefined four-argument helper. It now calls the original
five-argument symbol, which returns an owned UTF-16 buffer. The shared
implementation body is compiled in the original `AbyssEngine.cpp`
translation unit and in the isolated ABI smoke; this keeps the test on the
production body without linking unrelated engine dependencies. The smoke
checks positive, negative, zero, fractional and small-fraction formatting,
and exercises mode 0 directly. A comparison with the ARM instructions also
corrected the fractional exponent's initial value from 0 to 1.

## Verification and limits

Native `gof2`, the ABI/lifetime smoke, the standalone String reverse check,
206/206 ARM translation units, and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy improves from **70.5%/20.5% to
86.8%/26.0%** for `computeFloatString`, and from **42.3%/11.7% to
43.4%/12.3%** for `String::Set(float)`. Neither function is exact. The
focused reports are in the shared workspace `_work/` directory as
`float_{helper,set}_{before,after}_20261015.json`.

This confirms the static call contract and tested host behavior, not full
float-conversion equivalence. NaN, infinities, out-of-range integer
conversions, all rounding boundaries, and live ARM32 execution remain
unverified. `String::Set(float)` still has substantial source-shape and
lifetime differences.

## Full-corpus gate

The fresh report `_work/float_helper_full_20261015.json` in the shared
workspace compares **4,323/4,524** unique original functions across 4,720
comparisons, with **201 missing**, **123 extras**, zero skipped and zero
wrong-type. Average source-shape/strict fuzzy is **93.56%/82.60%**. Both
`(unit, symbol)` exact sets are unchanged from
`_work/string_set_float_sign_full_20261015.json`: **2,499 linked-exact**,
**1,074 byte-exact**, zero losses or gains. This is an ARM code-comparison
gate, not proof of full runtime behavior.
