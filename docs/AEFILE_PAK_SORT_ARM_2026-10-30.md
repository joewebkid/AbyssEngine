# AEFile PAK-entry sorting shape (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::sortPakFileEntryList()` occupies `0x7035c–0x703b4`.
The ARM function decrements the pass bound before each outer iteration,
including a final zero-length pass. Its inner loop runs while the index
differs from that bound. It swaps adjacent entries when the left CRC
exceeds the right CRC, and obtains the array data pointer again before
writing the second slot. The recovered source now expresses those
observed loop and pointer forms.

The previous positive-bound loop sorted ordinary input in the same
order. This packet improves source reconstruction and retains the
explicit second-slot lookup; it does not claim a new sort algorithm or
a fixed lookup bug. Focused ARM source-shape / strict fuzzy improves
**80.4% / 27.3% → 92.0% / 35.8%**. The function remains nonexact.
Residual differences include the static-global address layout and
register allocation in this build.

The production-code synthetic PAK test passes at `-O0/-O2` with three
records written in descending CRC order. Lookup and reading of all
three records succeed, requiring more than one bubble-sort pass. Native
`gof2` and ARM64 static builds pass; ARMv7 compiles 209/209 units.
Packed PAK entries and live Android archive behavior remain unverified
while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs: mean source-shape **95.01% → 95.02%**, strict fuzzy
**84.70% → 84.70%**, **2,665 linked-exact** and **1,088 byte-exact**.
There are zero skipped and 201 missing. Both complete `(unit, symbol)`
exact sets have no gains or losses relative to the preceding full
report. The verifier reports are stored outside Git in
`D:\gof2-arm64-crypto`.
