# SD save migration slot count (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `RecordHandler::convertSDVersionSaves` at `0xcffa8`
reads `Globals::recordSlots` when sizing its two record arrays and
two length arrays, while iterating the slots, and when selecting the
last backup record. The reconstruction instead dereferenced a
file-static `g_CSV_count` pointer initialized to `nullptr` and never
assigned. Entering the migration function could therefore crash
before reading a save. All count reads now use the original global.
The existing `Globals::recordSlots` declaration is `void *`; its
small integer value is converted through `std::uintptr_t`, as in the
adjacent backup migration function.

The source-extracted host test was updated to supply
`Globals::recordSlots` and the backup flag directly. Its two-slot
record and preview migration passes at Windows `-O0` and `-O2`,
checking reads, file deletions, output slot mapping, hash calls and
cleanup. This tests the actual count source instead of injecting a
pointer into the removed placeholder.

Focused ARMv7 verification builds 209/209 translation units and
compares 63 `RecordHandler` symbols. `convertSDVersionSaves`
source-shape improves from 46.9% to 88.3%, strict fuzzy from 18.0%
to 57.1%, and generated instructions rise from 60 to 222 against
195 original. The other 62 functions retain their metrics; the unit
keeps 21 linked-exact and one byte-exact comparison. Focused reports:
`_work/record_sd_count_before_focused_20261103.json` and
`_work/record_sd_count_after_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. A real save file,
missing slots, file-operation failure and Android runtime migration
remain unverified. The function still differs in temporary `String`
lifetime and call shape; this is the next source audit.

The full ARM corpus compares 4,323/4,524 original functions in 4,769
pairs, with 201 missing and zero skipped. Mean source-shape rises from
96.70% to 96.71%, and mean strict fuzzy from 86.67% to 86.68%.
Linked-exact stays 2,779 and byte-exact stays 1,108. No comparison
pair or exact match was lost. Full report:
`_work/record_sd_count_full_20261103.json`.
