# SD save migration temporary paths (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.

In `RecordHandler::convertSDVersionSaves` at `0xcffa8`, original ARM
constructs a temporary `String` from each slot index, concatenates it
with the primary directory, deletes that path, and destroys both the
result and index temporary. It repeats the sequence independently for
the backup directory. The reconstruction reused two strings across
both deletes and called `Set(long long)` on the index. It now expresses
the two original full-expression lifetimes with `String(i)`.

The original `String::Set(long long)` at `0x72a0c` sends zero to the
empty-string overload. Consequently, slot `0` appends an empty suffix
to either directory, whereas slot `1` appends `"1"`. The host test now
checks the resulting deletion paths for its synthetic `save/` and
`preview/` directories, along with reads, slot mapping, hashes and
cleanup. Both Windows `-O0` and `-O2` runs pass. This preserves the
observed original behavior; the effect on real save directories has
not been tested.

Focused ARMv7 verification builds 209/209 translation units and
compares 63 `RecordHandler` symbols. `convertSDVersionSaves`
source-shape improves from 88.3% to 94.2%, strict fuzzy from 57.1%
to 73.7%, generated instructions move from 222 to 204 against 195
original, and call-count match rises from 79.5% to 89.7%. The other
62 functions retain their metrics; the unit keeps 21 linked-exact and
one byte-exact comparison. Focused reports:
`_work/record_sd_count_after_focused_20261103.json` and
`_work/record_sd_string_lifetime_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. Real save files,
absent slots, file-operation failures and Android migration remain
unverified. Next audit the remaining slot move and cleanup call shape
against ARM, then test a genuine save fixture.

The full ARM corpus compares 4,323/4,524 original functions in 4,769
pairs, with 201 missing and zero skipped. Mean source-shape remains
96.71% after rounding; mean strict fuzzy rises from 86.68% to
86.69%. Linked-exact remains 2,779 and byte-exact 1,108. No
comparison pair or exact match was lost. Full report:
`_work/record_sd_string_lifetime_full_20261103.json`.
