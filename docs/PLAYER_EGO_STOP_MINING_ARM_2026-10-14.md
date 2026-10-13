# PlayerEgo mining-result handoff (2026-10-14)

## Evidence and recovery

Android 2.0.16 `PlayerEgo::stopMining()` is at ELF `0xa17b4` in
`analysis/gof2_libgof2hdaa_full_ida.c` (lines 86967–87059). The
independent Ghidra export at `analysis/gof2_gidra_engine_full_source.c`
(lines 67246–67343) confirms the cargo/status branches and terminal
cleanup. The previous source only deleted `MiningGame` and cleared its
pointer; it omitted almost the entire original function.

The recovered path reads the asteroid's item ID and MiningGame's ore
amount, halves a failed run's ore on hardcore difficulty, and caps it
to ship free space. A core run grants the corresponding core item first,
updates `Status+0xa4` and its discovery byte, then recaps ore to the
remaining space. Ore is granted only if its count is positive, with
`Status+0xa0` and its discovery byte updated. Item 217 maps to core
218 and skips both discovery arrays, as in the ARM body. Full-cargo
HUD events distinguish a full hold before collection from a hold
filled by collection. Finally the function restores exhaust, clears
the low byte at asteroid `+0x4c`, resets its player's hitpoints,
decrements `Status+0xd8`, deletes MiningGame, undocks and restores
audio.

The production-method host fixture covers six cases: ordinary ore,
core plus ore with a capacity cap, core consuming the last slot,
hardcore failed-run halving, a hold already full, and item 217/218.
It checks cargo, discovery flags, HUD call arguments, status counters
and terminal side effects. Native `gof2`, all 206 ARM translation
units, the mining smokes and 37 verifier tests pass (one skipped).
This is an isolated host fixture with dependent methods doubled, not
a full game runtime test.

Focused ARM `stopMining` source-shape rises from **9.3%** to
**91.0%**, strict fuzzy from **4.4%** to **39.7%**. It is not
linked-exact or byte-exact (180 generated versus 173 target
instructions). The local focused reports are
`_work/stop_mining_before_20261014.json` and
`_work/stop_mining_after_20261014.json`.

The full ARM corpus still covers **4,323/4,524** original functions
across 4,718 comparisons (201 missing, none wrong-type). Average
source-shape rises **93.08% to 93.09%**, strict fuzzy **81.50% to
81.51%**. All **2,403 linked-exact** and **1,008 byte-exact** pairs
are retained with zero losses or gains. The compared-function set and
123-symbol extra set are unchanged. The local full report is
`_work/stop_mining_full_20261014.json`.

## Remaining risk

`Status::hardCoreMode()` still dereferences an unassigned local
`g_hardCoreHolder` pointer even though the Android function reads the
global difficulty float `flt_218214`. Thus this dependency remains a
native runtime blocker; the host fixture substitutes its behavior.
`Hud::catchCargo`, `Ship::addCargo`, docking and FMOD are also doubled
in the fixture. A real ARM32 mining session and marquee ownership are
not yet verified. Next: recover the difficulty-global route, then
exercise the native mining handoff through its real dependencies.
