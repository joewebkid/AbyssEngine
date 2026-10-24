# RecordHandler whole-buffer write recovery (2026-10-24)

The Android 2.0.16 ARM library calls
`AEFile::Write(unsigned int, void *, unsigned int)` from
`RecordHandler::writeByteArrayAsRecord` at `0xcdd06–0xcdd0a` and from
`writeByteArrayAsOptionsFile` at `0xcde12–0xcde16`. The register arguments
are the byte count, buffer and open-file handle. Both recovered C++ methods
instead used `((AEFile *)(n))->Write(buf, fd)`: overload resolution converted
the non-null buffer pointer to `bool` and selected the one-byte writer.
This truncated rewritten records and options, including appended hashes.

Both sites now call the three-argument raw writer. The source-extracted host
gate passes Windows `-O0/-O2`, checking that all seven distinct bytes reach
the controlled file boundary for options, the primary record and the backup
record. The previous call would have left only one byte. Native `gof2`,
**208/208** ARM units and **93** verifier tests (one skip) pass.

Focused source-shape / strict fuzzy changes are **77.4% / 50.4% → 79.7% /
59.9%** for `writeByteArrayAsRecord` and **90.9% / 68.8% → 92.6% /
63.0%** for `writeByteArrayAsOptionsFile`; neither is exact. The full ARM
corpus still compares **4,323/4,524** originals over **4,736** pairs with
zero skips and **201** missing. Rounded mean source-shape / strict fuzzy
stays **94.23% / 83.60%**, with **2,735** 100%-fuzzy,
**2,568 linked-exact** and **1,082 byte-exact**. Before/after exact-symbol
sets have no lost matches.

This gate verifies the byte-write contract, not successful file opening,
hash computation/validation, other record fields, or live Android save
compatibility. Next: cover `addHash`/`checkHash` and the full record-file
envelope with a controlled file interface.
