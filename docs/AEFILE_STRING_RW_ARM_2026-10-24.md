# AEFile string I/O source-shape and ownership (2026-10-24)

The Android 2.0.16 ARM `AEFile::Read(String&, unsigned, bool)` at `0x70c44`
branches into wide/narrow handling before testing the length-read result.
The recovered C++ previously tested it first and zero-initialized the length,
adding two instructions absent from the original. Each branch now tests the
result locally; the length is assigned by the raw read before use.

The ARM `AEFile::Write(const String&, unsigned, bool)` at `0x70fe4` calls
`String::GetAEWChar()` for wide text, obtains the length inside each branch,
and reloads it from the String for the payload write. The recovered code
formerly accessed the wide pointer directly and reused a cached length.
Its narrow path also used scalar `operator delete` on the array allocated by
`String::GetAEChar()`; it now uses the ARM-observed `operator delete[]`.

Focused `Read` source-shape / strict fuzzy rises **93.7% / 64.6% →
100% / 100%**; `Write` rises **91.6% / 59.1% → 100% / 100%**. Both are
new linked-exact matches; neither is byte-exact. The source-extracted native
preview gate exercises wide write/read and independently checks the UTF-16
file bytes. It also checks narrow read/write and both wide/narrow failures
on the length and payload reads at `-O0/-O2`; the PendingProduct string
round trip still passes. Native `gof2`, **208/208** ARM translation units
and **93** verifier tests (one existing skip) pass.

The fresh full ARM corpus compares **4,323/4,524** original functions over
**4,737** pairs with zero skips and **201** missing. Mean source-shape /
strict fuzzy is **94.24% / 83.62%**, with **2,738** 100%-fuzzy,
**2,571 linked-exact** and **1,082 byte-exact** pairs. Against the preceding
full report, the two String I/O functions are newly linked-exact and no
linked- or byte-exact symbol pair was lost.

Android ARM `FileInterfaceAndroid::Read` at `0x6e2e4` compares the actual
byte count with the request on ZIP, file and JNI paths. It reports success
only for a complete requested read, so the original uninitialized length is
not consumed after a partial low-level read on that target. Other platforms'
unusual I/O failure behavior, a real original save fixture and live Android
runtime remain unverified. The next priority is a genuine save/preview
fixture and the complete load lifecycle.
