# String float sign (Android ARM, 2026-10-15)

## Confirmed source evidence

The original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `String::Set(float)` at `0x72cb0–0x72dbe`. When the sign flag returned
by the digit helper is nonzero, its instruction at `0x72d6e` loads the
literal `"-"`; the branch constructs that prefix and concatenates it before
setting the destination. The recovered method instead prepended `"E"`.
It now prepends `"-"`. The public signature and `String` layout are unchanged.

## Validation and limits

The native ABI smoke supplies controlled helper output (digits `15`,
exponent `1`, sign for `+/-1.5`) and checks that the production `Set(float)`
forms `1.5` and `-1.5`. Native `gof2` and the ABI smoke pass, **206/206**
ARM translation units compile, and **37** verifier tests run with one skip.

Focused source-shape/strict fuzzy for `Set(float)` remains
**42.3%/11.7%** before and after; it is neither linked-exact nor byte-exact.
Full ARM corpus compares **4,323/4,524** original functions, with **201
missing**, **123 extras**, zero skipped and **93.56%/82.60%** average
source-shape/strict fuzzy. The **2,499 linked-exact** and **1,074 byte-exact**
`(unit, symbol)` sets have zero losses or gains against the preceding
`string_append_full_20261015.json` report. The local focused and full JSON
reports are `string_set_float_sign_{before,after,full}_20261015.json` in
the shared workspace `_work` directory.

This is **not** a complete float conversion recovery. The original calls
`AbyssEngine::computeFloatString(float,int,int*,int*,int)` at `0x72cd6` and
receives a UTF-16 buffer. The current `String.cpp` calls a different,
undefined `String_computeFloatString(float,int,int*,int*)`; native `nm`
confirms that unresolved symbol in `libgof2.a`. The existing project
`AbyssEngine::computeFloatString` definition has the original mangled
parameter list but returns `void`, so it cannot yet supply the pointer.
The smoke's controlled helper proves only this sign-formatting path, not
the real helper's digit generation, edge cases, live ARM32 behavior, or
linkability of a complete game executable.

Next priority: recover the original helper's return value and input/precision
contract, connect `Set(float)` to it, then test zero, negative, fractional,
large and exceptional inputs on both native and ARM paths before claiming
float conversion complete.
