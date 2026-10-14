# Save preview level label (2026-10-14)

`MenuTouchWindow::createRecordButtons(bool)` previously treated
`GameRecord+0x20` as a `String` and copied its supposed data into the
preview label. This is an integer: `GameRecord::load()` passes it to
`Status::setLevel`, and `RecordHandler` reads/writes the same word as
an `i32` in full records and preview records. The field is now named
`savedLevel`; its offset and on-disk representation are unchanged.

The Android `libgof2hdaa.so` body at `0x125454` (IDA dump lines
162752–162766) builds this label from GameText ID `0x141` (321),
`: `, and the integer at record offset `+0x20`. Raw Thumb at
`0x125730–0x12576e` independently shows `getText(321)`, the colon
string constructor, a load from `+0x20`, the integer `String`
constructor, and two concatenations. The source now forms that
expression without interpreting the integer as a pointer.

The TouchButton smoke extracts the production label expression and
checks levels 0, 7 and 123, including text key 321. Native `gof2`,
206/206 ARM translation units and verifier tests (37 run, one skipped)
pass. Focused source-shape/strict fuzzy changes from **89.5%/43.6%**
to **89.9%/44.4%**; this function is not exact. The full report
`_work/menutouch_preview_level_full_20261014.json` compares
**4,323/4,524** original functions, average source-shape **93.28%**,
strict fuzzy **82.18%**, **2,479 linked-exact** and **1,065
byte-exact**. Its exact-pair and extra-symbol sets have no losses or
gains against `_work/layout_save_menu_metrics_full_20261014.json`;
123 extras and zero skipped units remain.

The smoke covers only label construction, not a complete save/load UI
flow. Asset-backed display, record lifecycle and ARM32 runtime remain
unverified. Next: trace the adjacent preview rank/row ownership and
exercise a populated save-menu interaction.
