# Status hardcore difficulty route (2026-10-14)

## Evidence and recovery

Android 2.0.16 `Status::hardCoreMode()` is at ELF `0xabe40` in
`analysis/gof2_libgof2hdaa_full_ida.c` (lines 98548–98552). It
compares the float at `0x218214` with `1.5`. The original ELF symbol
table places `_ZN7Globals7optionsE` at `0x2181e8` with size 100, so
that float is exactly `Globals::options + 0x2c`. The original
`Globals::Globals()` initializes it to `0.5`; menu start and touch
paths subsequently write the same setting. The local
`GameSettingsRecord` field at that offset was misleadingly named
`contrast`; it is now named `difficulty`.

The previous `Status::hardCoreMode()` dereferenced a never-assigned
`g_hardCoreHolder`. It now reads the confirmed four bytes of
`Globals::options` through `memcpy` and compares them with `1.5f`.
This avoids a null dereference without introducing another state
variable or relying on potentially unaligned float aliasing.
The combined production-method host fixture now extracts both
`Status::hardCoreMode()` and `PlayerEgo::stopMining()`; normal and
hardcore mining-result cases pass using option bytes. Native `gof2`,
all 206 ARM translation units and 37 verifier tests pass (one
skipped).

Focused ARM source-shape rises from **83.6% to 93.7%** and strict
fuzzy from **66.7% to 88.0%** (13 generated versus 12 target
instructions). It is neither linked-exact nor byte-exact. Local
focused reports: `_work/hardcore_before_20261014.json` and
`_work/hardcore_after_20261014.json`.

The full corpus still covers **4,323/4,524** original functions
across 4,718 comparisons (201 missing, zero wrong-type). Average
source-shape rises **93.09% to 93.10%**; strict fuzzy remains
**81.51%**. All **2,403 linked-exact** and **1,008 byte-exact**
pairs are retained with zero losses or gains. The compared-function
set and 123-symbol extra set are unchanged. The local full report is
`_work/hardcore_full_20261014.json`.

## Remaining risk

The fixture substitutes cargo, HUD, docking and FMOD behavior, and
does not load a saved options record through the actual game path.
A complete native/ARM32 mining session and marquee ownership remain
unverified. Next: exercise the handoff with real dependent services
and a persisted difficulty setting.
