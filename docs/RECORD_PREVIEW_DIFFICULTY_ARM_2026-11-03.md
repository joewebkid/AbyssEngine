# Record preview difficulty source (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `RecordHandler::recordStoreWritePreview(int)` at
`0xcfd78` writes `flt_218214` after the current campaign mission and
level, before the ship index. This global is the game difficulty:
the original `Globals` constructor assigns it `0.5f`, and
`Status::hardCoreMode()` checks it against `1.5f`. In the native
reconstruction, the same value lives at `Globals::options + 0x2c`;
`Globals::Globals()` initializes it to `0.5f`, and the reconstructed
`Status::hardCoreMode()` reads that offset.

The preview writer previously dereferenced a static null pointer to a
fictional rank holder. That would fail as soon as preview writing
reached this field. It now writes the raw float bits of the confirmed
difficulty field. The packed options view names the field and verifies
its offset on ARM. No preview file format or field order changed.
The receiving `GameRecord` field is currently named `rank` in the
reconstruction, but this serialized value is difficulty; the save-list
UI also compares it with difficulty thresholds.

Focused ARMv7 verification compiles 209/209 translation units. For
the affected writer, strict fuzzy rises from **45.6% to 66.1%** and
source-shape changes from **88.2% to 87.2%**. Generated instruction
count grows from 95 to 128 against 111 original instructions; the old
null dereference had allowed the compiler to discard part of the
writer. The 63-symbol unit keeps 21 linked-exact and one byte-exact
match, with no exact-set losses. Focused report:
`_work/record_preview_difficulty_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. A genuine preview
file has not yet been written and read on a permitted device or from
a complete game fixture. Next verify the remaining preview read/write
field correspondence and then exercise a genuine fixture.

The full ARM corpus compares **4,323/4,524** original functions in
**4,769** pairs, with 201 missing and zero skipped. Mean source-shape
remains **96.73%**, mean strict fuzzy **86.72%**, linked-exact **2,779**
and byte-exact **1,108**. Pair and exact sets have no losses against
`_work/record_ctor_assign_full_20261103.json`. Full report:
`_work/record_preview_difficulty_full_20261103.json`.
