# PlayerEgo asteroid-docking target (2026-10-14)

## Evidence and recovery

Android 2.0.16 `PlayerEgo::dockToAsteroid(KIPlayer*, Radar*)` is at
ELF `0x9f4a0` in `analysis/gof2_libgof2hdaa_full_ida.c` (lines
85170–85209). On entry the ARM body checks the first argument,
stores it at `PlayerEgo+0x1bc`, and passes it to
`PlayerAsteroid::getScaling`; the second argument is used to unlock
the radar on teardown. The independent Ghidra export at
`analysis/gof2_gidra_engine_full_source.c` (lines 64961–64989)
shows the same branch and store, though its recovered parameter
types are misleading. MGame call sites supply a locked asteroid
and a separate radar.

The previous C++ instead checked `radar`, saved that pointer as the
asteroid target and called `getScaling` on it. The corrected method
uses `kip` for all three entry operations. This is a behavior and
ABI repair, not a cosmetic shape change.

The host smoke extracts the production method unchanged and uses
different target and radar objects. It checks a null target does
not start docking, entry stores the asteroid and computes
`1.25 * 2500 = 3125`, and teardown re-enables rotation/camera,
resets gun delay, destroys the mining game, unlocks radar and
clears state. It does not run the whole game or its ARM32 binary.
Run: `python tools/run_player_ego_dock_asteroid_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe`.

Focused ARM source-shape is **90.8% → 94.7%** and strict fuzzy
**53.7% → 72.1%**; linked-exact and byte-exact remain false.
Local focused reports are `_work/dock_asteroid_before_20261014.json`
and `_work/dock_asteroid_after_20261014.json`.

Native `gof2`, all 206 ARM translation units, the new smoke, the
`stopMining` smoke and 37 verifier tests pass (one skipped). The
full ARM corpus remains **4,323/4,524** original functions covered
across 4,718 comparisons (201 missing, zero wrong-type). Its
averages remain **93.10%** source-shape and **81.51%** strict fuzzy
after rounding. All **2,403 linked-exact** and **1,008 byte-exact**
pairs are retained: pair-set comparison found zero additions or
losses. The compared set and 123-symbol extra count are unchanged.
The local full report is `_work/dock_asteroid_full_20261014.json`.

## Remaining risk and next priority

The smoke substitutes the Player, Radar, camera and MiningGame
services; full docking/mining gameplay and ARM32 runtime remain
unverified. `PlayerEgo::update` has a path that calls
`dockToAsteroid(nullptr, nullptr)` while docking, whereas teardown
calls `Radar::unlockAsteroid`. The follow-up
[update docking audit](PLAYER_EGO_UPDATE_DOCKING_ARM_2026-10-14.md)
confirms the ARM call passes the radar and corrects this route.
Reachability and safety when the asteroid target itself is null
remain unverified. Do not infer safety from the shape metric alone.
