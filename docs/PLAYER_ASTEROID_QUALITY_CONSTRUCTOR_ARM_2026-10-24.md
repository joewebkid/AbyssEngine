# PlayerAsteroid quality-string constructor (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0xe2a00`, `getQualityString` selects `C/B/A` for qualities `5/6/7`,
`D` for `4`, and `E` otherwise, then calls
`AbyssEngine::String::String(const char *, bool)` at `0xe2a28` with
`false`. The source already selected those confirmed letters but manually
grew the UTF-16 buffer character by character. Returning `String(text,
false)` restores the observed constructor path and retains the same ASCII
values. The existing constructor's `Set(const char *)` copies those bytes
into the string buffer.

Against the preceding full ARM report, focused source-shape/strict fuzzy
improves **35.7%/18.4% → 90.7%/91.3%**. The generated and original
functions are both 23 instructions; the remaining disassembly difference is
an equivalent unsigned bounds check (`cmp #2; bhi` versus `cmp #3; bcs`).
A branch rewrite was tried but lowered strict fuzzy to 78.3%, so the clearer
source was retained. This function is neither linked-exact nor byte-exact.

Native `gof2`, all **208/208** ARM translation units and **93** verifier
tests pass (one skip). The full corpus compares **4,323/4,524** original
functions across **4,736** pairs, averaging **94.19% source-shape** and
**83.53% strict fuzzy**, with **2,563 linked-exact**, **1,082 byte-exact**,
**2,729 100%-fuzzy**, **201 missing**, zero wrong-type and zero skipped.
Pairwise exact-set comparison with the preceding projection report finds no
linked- or byte-exact gain or loss. Live display of asteroid quality was not
tested; first-friend Pan ship insertion remains the device priority.
