# Android archive/file read and existence paths (2026-10-24)

Android 2.0.16 ARM `FileInterfaceAndroid::OpenRead` at `0x6dfe0` and
`FileExist` at `0x6e630` are the primary source. Both form ZIP paths using
`assets/` and `47947_GOF2CONTENT_ETC/assets/`, probe the main and patch
archives, and fall back to `appRootDir` with mode `"rb"`. `OpenRead` strips
one leading slash before constructing those paths; its disk fallback uses
the stripped name **without** an `assets/` prefix. It logs the first ZIP
path and arguments to Android `stderr`. ARM passes the caller's windowed
flag and archive offset to the ZIP-file constructor at `0x6e0d8` and
`0x6e0ee`. The recovered source previously used null literals/pointers,
logged the wrong path and arguments, prefixed the disk fallback with
`assets/`, and passed name length instead of offset.

The two `FileInterfaceAndroid` archive pointers now address shared local
slots used by `loadAPKAndZip`, `FileExist` and `OpenRead`. This implements
the ARM-observed shared APK/patch state; the local slot names are a C++
representation, not recovered original symbol names. `OpenRead` uses
short-lived String temporaries matching the observed destructor sequence.
Focused source-shape/strict fuzzy changes are **40.8%/16.0% → 98.3%/94.1%**
for `OpenRead` and **97.2%/91.3% → 98.7%/98.7%** for `FileExist`.
Neither function is linked- or byte-exact.

`tools/run_file_android_readexist_smoke.py` extracts the production
lookup methods, file and ZIP constructors, `Seek`, and `loadAPKAndZip`.
Windows `-O0/-O2` runs verify primary/patch archive names and precedence,
missing-patch behavior, slash stripping, a real two-byte filesystem read
under an isolated temporary root, disabled-reader behavior, and the
42-byte archive seek. ZIP calls are controlled host doubles, not a real
APK read. Native `gof2` builds, **208/208** ARM translation units compile,
and **93** verifier tests pass (one existing skip).

The final full ARM corpus compares **4,323/4,524** original functions
over **4,737** symbol pairs, with **201** missing and zero skips. Mean
source-shape/strict fuzzy is **94.31%/83.71%**, up from
**94.30%/83.70%**. The exact sets remain **2,578 linked-exact** and
**1,082 byte-exact**; pair-by-pair comparison finds zero lost or gained
exact matches.

Real libzip decompression, JNI setup, live Android filesystem behavior and
full-save loading remain unverified. `FileExist`'s original ARM path does
not guard a null `appRootDir` before constructing its disk path; the host
test intentionally does not claim that branch is safe. Next priority:
exercise genuine Android archive/save fixtures and the `AEFile` read
fallback end-to-end before claiming restored game loading.
