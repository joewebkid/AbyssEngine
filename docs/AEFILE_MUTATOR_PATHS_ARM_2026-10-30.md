# AEFile mutator path temporaries (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::FileDelete(String const&)` occupies `0x711b8–0x7120c`;
`AEFile::SetSaveDirectory(String)` occupies `0x71288–0x712d4`.

Both ARM bodies construct `String(path, false)` directly in the
by-value argument slot for the corresponding virtual `FileInterface`
method, then destroy it immediately after the call. The recovered
source had created a default string, called `Set()` and incurred an
additional argument copy. The two calls now use direct temporary
construction. The null-interface behavior and return contract remain
unchanged.

Focused ARM source-shape / strict fuzzy improves **75.4% / 60.0% →
98.7% / 94.5%** for `FileDelete` and **74.0% / 56.5% → 98.6% / 94.0%**
for `SetSaveDirectory`. Neither is exact. ARMv7 compiles 209/209 units;
native `gof2` and ARM64 static builds pass. The existing production-code
PAK host test passes at `-O0/-O2`; it does not exercise destructive
delete or save-directory effects. Those effects and live Android
behavior remain unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,748** pairs. Mean source-shape rises **95.00% → 95.01%** and strict
fuzzy **84.67% → 84.69%**; **2,664 linked-exact** and **1,088
byte-exact** remain unchanged. There are zero skipped and 201 missing.
Both complete `(unit, symbol)` exact sets have no losses or gains
relative to the preceding full report. The verifier reports are stored
outside Git in `D:\gof2-arm64-crypto`.
