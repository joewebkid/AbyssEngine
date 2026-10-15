# String signed-int append contract (Android ARM, 2026-10-15)

## Evidence and change

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `AbyssEngine::String::operator+=(int const&)` at
`0x72928–0x72950`. Its Thumb instructions load the signed 32-bit value,
write it as the low word of a stack-local 64-bit value, create the high word
with `ASRS #31`, and call `operator+=(long long const&)` at `0x72940`.
The stack-canary epilogue is compiler instrumentation, not source logic.
The REA/IDA call relationship is corroborated by the call instruction;
REA does not provide complete body ranges or direct callers in this profile.

The previous C++ instead constructed a temporary `String` and invoked
`operator+=(String const&)` directly. It now sign-extends to a local
`long long` and delegates to the observed overload. The host ABI/lifetime
smoke checks the `INT_MIN` append path, including sign extension.

## Verification and limits

Focused source-shape/strict fuzzy changes **53.0%/14.3% →
100%/100%**. The symbol gains linked-exact equality but not byte-exact
equality. Native `gof2`, the ABI/lifetime smoke, the standalone String
reverse check, **206/206** ARM translation units and **37** verifier tests
(one skipped) pass. Focused reports are in the shared workspace `_work/`
directory as `string_append_int_{before,after}_20261015.json`.

This confirms the ARM call contract and matching linked instructions;
it does not prove live original behavior on all values. In particular,
the downstream `operator+=(long long const&)` still differs from the
original's `realloc`/copy path. That allocator/ownership distinction is
outside this patch and must not be hidden by this exact wrapper match.

## Full-corpus gate

The fresh shared-workspace report `_work/string_append_int_full_20261015.json`
compares **4,323/4,524** unique original functions across 4,720
comparisons, with **201 missing**, **123 extras**, zero skipped and zero
wrong-type. Average source-shape/strict fuzzy is **93.59%/82.63%**.
Compared with `_work/string_set_i64_full_20261015.json`, the linked-exact
`(unit, symbol)` set gains only this int-append symbol and loses none:
**2,499 → 2,500**. The byte-exact set is unchanged at **1,074** with
zero losses.

Next priority: resolve the ownership, append length and terminator behavior
of `operator+=(long long const&)` with instruction evidence and controlled
boundary tests before attempting to reproduce its original `realloc` path.
