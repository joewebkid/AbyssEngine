# Record file I/O wiring — Android ARM (2026-10-11)

The decompiled `RecordHandler.cpp` called 21 locally declared but undefined
`AEFile_Read*` and `AEFile_Write*` helpers. Android ARM calls the existing
`AEFile::Read`/`Write` overloads. Small local forwarding functions now resolve
those calls without rewriting the roughly 600 reader/writer call sites.
Scalar forwarding uses the file format's 1-, 2-, 4-, and 8-byte widths; the
float-bit writer preserves all four raw bytes. String calls retain their
wide/narrow flag. In the preview reader, the misleading `Read_bool` name
covers both wide-string reads (flag 1) and one-byte item flags (flag 0), so
its forwarding function handles both cases.

The native `RecordHandler.cpp` object no longer references any undefined
`AEFile_Read*` or `AEFile_Write*` symbol; all referenced `AEFile::Read` and
`AEFile::Write` overloads are defined in `libgof2.a`. The native static
library builds, all 206 ARM translation units compile, and the verifier
suite runs 36 tests successfully (one skipped). The focused RecordHandler
report has 60 comparisons, 88.4% average source-shape, 67.1% strict fuzzy,
19 linked-exact and one byte-exact. The full ARM corpus compares
4,323/4,524 original functions, with 92.18% average source-shape,
80.37% strict fuzzy, 2,350 linked-exact, 1,008 byte-exact, zero skipped
units, and zero wrong-type symbols. No previously exact pair was lost.
Relative to the prior full report, source-shape and exact counts are stable;
strict fuzzy changes from 80.38% to 80.37%.

This establishes symbol wiring, not a working save/reload round trip.
`GameRecord` still represents some ARM `String` fields as raw 32-bit slots;
casting those slots to `String` can be unsafe on a 64-bit native build.
The final game executable has not been linked or exercised. The next bounded
package should recover the preview record's true field types and ownership,
then exercise a native serialization round trip before claiming runtime
correctness. Other unresolved dependencies may remain elsewhere.
