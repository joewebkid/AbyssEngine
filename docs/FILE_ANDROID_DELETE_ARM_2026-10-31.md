# Android FileDelete argument recovery (2026-10-31)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`FileInterfaceAndroid::FileDelete(String)` occupies `0x6e764–0x6e78c`.
At `0x6e776` it constructs a local `String` from the incoming string
argument with `reverse=false`, destroys that local at `0x6e77a`, then
returns zero. No file-removal call appears in this original body.

The previous source interpreted `name.data`, a pointer to UTF-16
characters, as a pointer to a `String` object. Constructing a copy from
that fabricated object could read invalid fields. The recovered source
now copies the actual `name` object. This is a behavior and ABI fix;
the observed Android method still reports failure and does not remove
a file.

Focused ARM source-shape / strict fuzzy improves **98.1% / 97.6% →
100.0% / 100.0%**, and the function becomes linked-exact. ARMv7
compiles 209/209 units; native `gof2` and ARM64 static builds pass.
The source-extracted Android file test passes at `-O0/-O2`: it calls
`FileDelete` with a nonempty path, confirms a zero result and confirms
the existing file remains. This host gate does not execute JNI, ZIP or
Android filesystem services. Live device behavior remains unverified
while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs: **95.02%** mean source-shape, **84.70%** strict fuzzy,
**2,666 linked-exact** and **1,088 byte-exact**, zero skipped and 201
missing. Neither complete `(unit, symbol)` exact set loses a match;
`FileDelete` adds one linked-exact pair. The verifier reports are
stored outside Git in `D:\gof2-arm64-crypto`.
