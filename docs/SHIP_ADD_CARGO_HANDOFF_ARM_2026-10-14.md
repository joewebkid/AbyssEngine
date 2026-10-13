# Ship cargo-add handoff (2026-10-14)

## Evidence and recovery

Android 2.0.16 `Ship::addCargo(Item*)` is at ELF `0x174faa` and
`Ship::addCargo(Array<Item*>*)` at `0x174fe8` in
`analysis/gof2_libgof2hdaa_full_ida.c` (lines 225623–225638).
The single-item overload constructs a one-element array and calls
the array overload; that overload combines the new item with existing
cargo before calling `setCargo`. The Ghidra export independently
shows the one-element construction and indirect terminal call at
`analysis/gof2_gidra_engine_full_source.c` lines 184365–184394,
though it does not resolve that indirect target.

The prior source incorrectly called `setCargo` directly from the
single-item overload, replacing all existing cargo. This mattered
immediately to `PlayerEgo::stopMining`: awarding a core and then ore
discarded the core. It also affected other single-item cargo callers.
The overload now calls `addCargo(Array*)`, retaining and combining
existing items.

The production-method host smoke runs both `Ship::addCargo`
overloads, `Ship::setCargo`, `Ship::getFreeSpace`,
`Item::combineItems` and `Item::getAmount` with the real `Array`
template. It confirms the core remains after ore is added, a repeat
ore item merges by ID, and load/free-space counters update. The
combined `PlayerEgo::stopMining` smoke and 37 verifier tests also
pass (one skipped); native `gof2` and all 206 ARM translation units
build.

Focused ARM source-shape and strict fuzzy both remain **100.0%**
before and after this semantic fix. The function is still neither
linked-exact nor byte-exact. This is a concrete example of a wrong
call target passing a shape metric. Local focused reports:
`_work/ship_add_cargo_before_20261014.json` and
`_work/ship_add_cargo_after_20261014.json`.

The full ARM corpus remains **4,323/4,524** original functions
covered across 4,718 comparisons (201 missing, zero wrong-type).
Average source-shape remains **93.10%** and strict fuzzy **81.51%**.
All **2,403 linked-exact** and **1,008 byte-exact** pairs are
retained with zero losses or gains. The compared-function set and
123-symbol extra set are unchanged. The local report is
`_work/ship_add_cargo_full_20261014.json`.

## Remaining risk

The host fixture extracts production methods but stubs
`Ship::refreshValue` and uses fixture item objects; it does not run
the entire game or prove cargo allocation/lifetime behavior. The
separate `PlayerEgo` fixture still doubles HUD, docking and audio.
Next: validate those result-handoff effects with real dependent
services; full ARM32 runtime and marquee lifetime remain unverified.
