# AEFile open and archive fallback (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::Open(String&, FileOpenType, uint32_t*)` occupies
`0x708c4–0x70a78`. The three wrapper modes are documented in
[the open-mode ABI note](AEFILE_OPEN_MODE_ABI_ARM_2026-10-24.md).

The original converts the input path with `String::operator unsigned
short*()` before dispatch. For each mode, it constructs a temporary wide
`String` directly as the by-value virtual `OpenWrite`, `OpenAppend`, or
`OpenRead` argument and destroys it after the call. The previous source
read the path storage directly and used a default-constructed temporary
followed by `Set()`. The recovered form follows the observed conversion
and lifetime.

When a loose read misses and the path lacks a leading slash, ARM builds
`"/" + String(path, false)`, assigns it back to the caller's path, then
looks up the archive entry. The source now retains that explicit copy
temporary. On an unrecognized numeric open mode, the original skips
the native open and reaches file-table registration with a null file;
the source follows that unusual path. Normal callers use modes 0, 1,
and 2. For an unnamed handle, the original deletes an occupied slot
zero and clears it before placing the new file. The source preserves
that sequence.

Focused `AEFile::Open` ARM source-shape / strict fuzzy improves
**82.3% / 27.4% → 92.2% / 46.0%**. It remains nonexact. The ARMv7
build compiles 209/209 units, and native and ARM64 static builds pass.
The production synthetic PAK test passes at `-O0/-O2`; it now checks
the caller-visible slash prefix and replacing an unnamed file in slot
zero, including closing the previous stream.

The full ARM corpus compares **4,323/4,524** original functions over
**4,748** pairs: mean source-shape **94.99% → 95.00%**, strict fuzzy
**84.67% → 84.67%**, **2,664 linked-exact** and **1,088 byte-exact**.
There are zero skipped and 201 missing. Compared with the preceding
full report, neither complete `(unit, symbol)` exact set has gains or
losses. Reports and test binaries are stored outside Git in
`D:\gof2-arm64-crypto`.

The original's inline `AENormalFile` initialization still differs from
the recovered out-of-line constructor call. The exact constructor
symbols are preserved; manually writing their fields solely to imitate
the call site was not adopted. The compiler also arranges the mode
branches differently. Actual Android file I/O, packed PAK data, and
invalid-mode use remain unverified while device testing is deferred.
