# Record migration array cleanup (2026-10-24)

Android 2.0.16 ARM calls `ArrayReleaseArrays<signed char *>` twice at the end
of each migration function: `convertSDVersionSaves` at `0xd0164/0xd016a`
and `changeSaveDirectoryToBackupDirectory` at `0xd0354/0xd035a`. The
recovered C++ instead deleted the elements, resized each array through
`ArrayRemoveAll`, and then deleted the array objects. The replacement uses
the original ownership operation directly, avoiding a needless reallocation
during teardown. The record moves themselves were not changed: even the
unusual `addHash(i + 1)` in `changeSaveDirectoryToBackupDirectory` is present
in the original ARM at `0xd032a` and is retained.

The source-extracted host test runs both complete functions at Windows
`-O0/-O2` with two populated record and preview slots. It checks reads,
deletes, output slot mapping, hash calls, the backup flag and successful
cleanup. Native `gof2` and all **208/208** ARM translation units build;
**93** verifier tests pass (one existing skip).

Focused source-shape / strict fuzzy for `convertSDVersionSaves` moves from
**51.7% / 18.4%** to **46.9% / 18.0%**; for
`changeSaveDirectoryToBackupDirectory` it remains **34.6% / 19.1%**.
Neither is exact. This is a deliberate metric regression to match the
observed original cleanup call and remove the extra resize; the compiler's
surrounding code generation still differs substantially.

The full ARM corpus compares **4,323/4,524** original functions over
**4,737** pairs with zero skips and **201** missing. Mean source-shape /
strict fuzzy is **94.24% / 83.60%**; **2,736** pairs are 100%-fuzzy,
**2,569 linked-exact** and **1,082 byte-exact**. The newly instantiated
`ArrayReleaseArrays<signed char *>` in the `RecordHandler` unit is 100%
source-shape/strict fuzzy and linked-exact. Against the preceding full
report, no linked- or byte-exact symbol pair was lost.

The host uses synthetic records and controlled file methods. It does not
validate the real save format, absent slots, file-operation failures, an
Android runtime, or actual memory-leak freedom under a sanitizer. A complete
save/preview fixture and live load remain the next priorities.
