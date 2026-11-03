# Record list slot count (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `RecordHandler::readAllRecords` at `0xcbcdc` and
`readAllPreviewRecords` at `0xcbb48` size their result arrays from
`Globals::recordSlots` and iterate that many slots. In the recovered
`Globals::init`, the same global is assigned 6 or 12 depending on
the platform flag. The C++ declaration still uses `void *` as an ABI
placeholder for the ARM integer global; other save code converts its
value through `uintptr_t`.

Both list readers previously dereferenced `g_RH_recordCount`, a
permanently null pointer. They now read the confirmed slot count
directly from `Globals::recordSlots`. This restores list allocation
and iteration without changing the array or record formats.

Focused ARMv7 verification compiles 209/209 translation units and
compares 63 `RecordHandler` symbols. Each changed reader moves from
**65.4% to 98.2%** source-shape and **24.5% to 76.5%** strict fuzzy;
the generated count moves from 15 to 34 instructions against 34 in
the original. The unit mean moves from **93.79% to 94.83%**
source-shape and **74.91% to 76.56%** strict fuzzy. The unit retains
21 linked-exact and one byte-exact comparison, with no exact-set
losses. Focused report: `_work/record_all_slots_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The existing
source-extracted preview round-trip test passes at `-O0` and `-O2`
with a synthetic file boundary. It exercises the preview reader and
`GameRecord` writer, but does not exercise the two list readers or the
live-status writer. Real save lists have not yet been exercised on a
permitted device or with a genuine save fixture. Next test list
population with genuine files and audit remaining save-list paths.

The full ARM corpus compares **4,323/4,524** original functions in
**4,769** pairs, with 201 missing and zero skipped. Mean source-shape
rises from **96.73% to 96.74%**, mean strict fuzzy from **86.72% to
86.74%**. Linked-exact stays **2,779** and byte-exact **1,108**.
Pair and exact sets have no losses against
`_work/record_preview_difficulty_full_20261103.json`. Full report:
`_work/record_all_slots_full_20261103.json`.
