# AENormalFile stream release (2026-10-31)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AENormalFile::Release()` occupies `0x70140–0x70160`. With a non-null
`FileInterface*`, the ARM body calls the virtual `Close` method through
vtable offset `+0x44`, reloads the pointer, calls its virtual deleting
destructor, clears the field, and returns one. The null case also clears
the field and returns one.

The previous source deleted the interface without first calling `Close`.
That omitted a real step for implementations whose destructors do not
close themselves. The recovered source calls `Close` before `delete`.
Focused `Release` improves from **80.6% source-shape / 82.8% strict fuzzy**
to **100.0% / 100.0%** and is byte-exact. Its two destructor aliases
improve from **80.4% / 96.3%** to **100.0% / 100.0%** and become
linked-exact. The focused `AENormalFile` unit is 10/10 at 100% in both
metrics, with 10 linked-exact and five byte-exact pairs.

The production-code synthetic PAK test passes at `-O0/-O2`. It now
opens an ordinary file through the host adapter, releases an
`AENormalFile`, and counts one explicit `Close` plus the adapter's
idempotent destructor `Close`. The underlying stream closes only once,
and subsequent destruction does not close it again. Native `gof2` and
ARM64 static builds pass; the ARMv7 comparison build compiles 209/209
translation units. Alternate interface implementations and live Android
ownership remain unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs: mean source-shape **95.03% → 95.05%**, mean strict fuzzy
**84.71% → 84.72%**, **2,673 linked-exact** and **1,091 byte-exact**,
zero skipped and 201 missing. Comparing complete `(unit, symbol)` exact
sets against the previous main revision finds no losses. `Release` and
both destructor aliases add three linked-exact pairs; `Release` adds
one byte-exact pair. Reports and host test outputs are stored outside
Git in `D:\gof2-arm64-crypto`.
