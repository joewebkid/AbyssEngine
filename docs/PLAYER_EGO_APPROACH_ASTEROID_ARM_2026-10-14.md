# PlayerEgo asteroid mining phase (2026-10-14)

## Evidence and recovery

Android 2.0.16 `PlayerEgo::approachAsteroid` begins at ELF
`0x9f570`. Raw ARM at `0x9f590–0x9f59a` loads the target from
`PlayerEgo+0x1bc` and calls `KIPlayer::isDying`; at
`0x9f726–0x9f738` it reloads the same target for dying/dead
checks after `MiningGame::update`. The earlier C++ tested the ego
object instead. The target is normally installed with the docking
flag by `dockToAsteroid`; a source-level search found no other
independent write of that flag in PlayerEgo. That does not prove
null-target runtime safety under all external transitions.

The original ELF places `Globals::hints` at `0x21824c`; the byte
reset at `0x218283` is `hints[0x37]`. `Globals::status` is at
`0x2181b0`; the ARM writes its pointee at `+0x124`, the existing
`Status::field_124`. `Globals::sound` is at `0x2182bc`; ARM plays
event 1 and pauses the event index at `PlayerEgo+0x1c`.
The earlier source dereferenced four unassigned holders instead.

ARM preserves the incoming `Hud*` in `r5` at `0x9f582`, passes it
to `MiningGame` and uses it for event 8. The prior C++ cast an
unrelated integer argument to `Hud*`. The original also stores zero
at `(*(asteroid+4)+0x40)`: the pointer at `KIPlayer+4` is `Player*`,
and `Player+0x40` is `radius`. The previous source wrote at
`asteroid+0x44` instead. These routes are now direct in C++.

The host smoke extracts the unchanged production method. Its
doubles cover a dying target before approach, a live target on the
approach branch, mining-game construction and HUD ownership,
correct hint/radius/sound effects, target death during a mining
update, and a lost-game event/status reset. The stage-zero helper
is stubbed only in the smoke; it is not a production implementation.
Run: `python tools/run_player_ego_approach_asteroid_smoke.py
--cxx C:/msys64/ucrt64/bin/g++.exe`.

Focused ARM source-shape is **23.8% → 39.6%** and strict fuzzy
**13.5% → 21.6%**; linked-exact and byte-exact remain false.
Local focused reports are
`_work/approach_asteroid_guard_before_20261014.json` and
`_work/approach_asteroid_guard_after_20261014.json`.

Native `gof2` static-library build, all 206 ARM translation units,
the new smoke, docking and stopMining smokes and 37 verifier tests
pass (one skipped). The full ARM corpus remains **4,323/4,524**
original functions covered across 4,718 comparisons (201 missing,
zero wrong-type), **93.10%** average source-shape, **81.52%**
strict fuzzy, **2,403 linked-exact**, **1,008 byte-exact** and
123 extras. Exact-pair and compared-function sets are unchanged.
The local report is `_work/approach_asteroid_guard_full_20261014.json`.

## Remaining risk and next priority

The production archive still has an undefined reference to
`PE_aa_approachStep(PlayerEgo*, int, void*)`; only its declaration
and call exist. Therefore stage-zero approach is not linked or
runtime-verified, despite the green static-library build. The
smoke also replaces game services and does not execute ARM32.
Next, recover stage-zero navigation/orientation from ARM and
remove the undefined helper. The target-null teardown path remains
unproven in real gameplay; no speculative guard was added.
