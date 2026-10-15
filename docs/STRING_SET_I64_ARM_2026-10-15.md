# String 64-bit integer conversion (Android ARM, 2026-10-15)

## Evidence and recovery

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `AbyssEngine::String::Set(long long)` at `0x72a0c–0x72ad4`.
The Thumb instructions free the old buffer, take the empty-string path for
zero, allocate a UTF-16 buffer directly for nonzero input, prepend `'-'` for
negative input, negate its 64-bit magnitude with `RSBS`/`SBC`, extract decimal
digits with unsigned division by ten, terminate the buffer, and reverse the
digits in place. This is instruction-backed behavior, not a conclusion from
pseudocode alone.

The old C++ implementation negated a signed `long long` with `-v`, which is
undefined for `INT64_MIN`, and built a temporary buffer only to copy it again
through `Set`. The recovered method now follows the direct-buffer path and
uses unsigned two's-complement negation for the full signed range. Native
ABI/lifetime checks cover negative, zero, `INT64_MIN`, `INT64_MAX`, replacing
an existing value, and appending through `operator+=(long long)`.

## Deliberate safety divergence

The Android function requests **40 bytes**, enough for 19 decimal digits and
a terminator, but not for a minus sign, 19 digits, and a terminator. Static
instruction analysis therefore indicates a two-byte out-of-bounds terminator
write on `INT64_MIN` and other negative 19-digit inputs; live original ARM32
behavior was not observed. Our source allocates **21 UTF-16 units** for every
nonzero value. This intentional two-byte allocation difference prevents the
overflow and precludes an exact-match claim for this implementation.

## Verification and limits

Focused ARM `Set(long long)` source-shape/strict fuzzy changes
**65.1%/18.9% → 89.4%/21.9%**, without linked-exact or byte-exact equality.
The native `gof2` and ABI/lifetime smoke, standalone String reverse check,
**206/206** ARM translation units and **37** verifier tests (one skipped)
pass. Focused reports are in the shared workspace `_work/` directory as
`string_set_i64_{before,after}_20261015.json`.

The source-shape improvement supports a closer control-flow reconstruction;
it does not prove full semantic equivalence. In particular, live ARM32
execution and allocator-dependent effects of the original 40-byte request
remain unverified.

## Full-corpus gate

The fresh shared-workspace report `_work/string_set_i64_full_20261015.json`
compares **4,323/4,524** unique original functions across 4,720
comparisons, with **201 missing**, **123 extras**, zero skipped and zero
wrong-type. Average source-shape/strict fuzzy is **93.58%/82.61%**.
Compared with `_work/float_set_object_full_20261015.json`, the
`(unit, symbol)` exact sets have **zero losses or gains**: **2,499
linked-exact** and **1,074 byte-exact**.

Next: inspect the adjacent integer-constructor and append ARM shapes, and
seek a controlled ARMv7 runtime check for negative 19-digit inputs if an
equivalent original function is runnable. Do not infer Android 2.0.16
runtime behavior from a different iOS release.
