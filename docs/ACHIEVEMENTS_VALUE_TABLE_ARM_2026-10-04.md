# Achievement value table recovered from Android ARM (2026-10-04)

Scope: the 45-by-3 signed value table consumed by
`Achievements::getValue(int, int)`. Primary reference: Android 2.0.16
`libgof2hdaa.so`, `getValue` at `0x157018..0x157034` and `.rodata` beginning
at `0x203134`.

The ARM body computes `table + index * 12 + (sub - 1) * 4`, then loads a
signed 32-bit value. Its 135 entries occupy 540 bytes and include `-1`
sentinels, several tier thresholds, and a maximum of 1,000,000. The source
previously declared the same-sized array filled entirely with zero. This
returned incorrect values while the method's instructions still scored
**100% linked-exact**: function matching did not validate the referenced
`.rodata` content.

All 135 source integers were compared in order with the original binary.
The rebuilt ARM object's local `gAchievementValues` symbol is 540 bytes in
`.rodata`; all 540 bytes match the original table. Focused verification
before and after remains 100% source-shape, 100% strict fuzzy and
linked-exact (8/8 instructions). This package changes data, not control
flow or ABI.

The original `getValue` does not check index or sub bounds. This patch does
not introduce a different contract; observed `ModStation` callers request
tiers 2 and 3, while `ChoiceWindow::setMedal` receives the stage stored in
its medal list.

The UCRT64 `gof2` native library builds and all 206 matching ARM translation
units compile. The full `--fail-on-wrong-type` report compares 4,322/4,524
unique originals across 4,675 rows, with 202 absent, zero wrong-signature
and zero skipped. Mean source-shape is 90.07% and strict fuzzy is 77.21%.
Linked-exact and raw-byte-exact counts remain 2,162 and 977, respectively;
the exact `(unit, symbol)` sets have zero losses or gains. Local reports are
`_work/blueprint-isunlocked/achievements_value_before.json`,
`achievements_value_after.json`, and `achievements_value_full.json`.

## Remaining work

This correction does not restore achievement detection. The current
`Achievements::checkForNewMedal(PlayerEgo*)` body is 17 generated instructions
versus 414 in the original, and its separate `gCFN_req` table is still zero.
That body and table are the next behavior-recovery priority. Full game
runtime, the display path in `ChoiceWindow::draw`, and other callers remain
unverified.
