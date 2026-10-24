# AEFile raw PAK fallback (2026-10-24)

The Android 2.0.16 ARM `libgof2hdaa.so` (`sha256:
6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
shows `AEFile::RegisterPakFile` at `0x70658` calling the parser and sorter.
At `0x703bc`, `collectFilesInPakFiles` reads a signed one-byte name length,
the name bytes, 32-bit packed and raw-size fields, then skips payload bytes.
The raw-size sentinel `0xffffffff` selects a packed-size skip at `0x704b8`.
`findPakFile` at `0x7075c` uses the stored offset for the raw entry's
`FileInterface::OpenRead` call. This identifies the synthetic test format;
it does not establish that the shipped APK/OBB contain standalone PAKs.

`tools/run_aefile_pak_smoke.py` creates two small raw entries entirely in a
temporary test directory. At Windows `-O0` and `-O2`, production `AEFile`
registers and sorts them, falls back from missing loose files, reads each
entry through `AEPakFile`, enforces its size limit, reuses handle 1 and
closes all three opened host streams. The host bridge now supports a raw
offset read and rejects an offset beyond EOF; compressed/windowed entries
remain unsupported. The entries are deliberately written in descending CRC
order, so finding both requires sorting before the binary search. The
existing original-asset file-stack gate remains
**179/179** closed lifetimes, native `gof2` builds, and **93** verifier
tests pass (one skip).

Current focused ARM `engine/file/AEFile` is **94.43% source-shape / 80.43%
strict fuzzy** over **61** pairs, with **11 linked-exact** and **1
byte-exact**. Within it, `RegisterPakFile` is **100%/100%**,
`collectFilesInPakFiles` **95.4%/74.1%**, `findPakFile` **73.9%/25.5%**,
and `Open` **82.3%/27.4%**; these four are not exact. No production C++
changed, so the last full ARM corpus remains **4,323/4,524** original
functions over **4,737** pairs, **94.31%/83.73%** mean source-shape/strict
fuzzy, **2,578 linked-exact**, **1,082 byte-exact**, with no exact-set
change.

Next: source-backed testing of compressed PAK entries and Android save-file
I/O. Neither real shipped PAK data nor live Android execution was available
for this gate; the original APK/OBB contain no `.pak` entries.
