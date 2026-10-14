# PlayerEgo update docking teardown (2026-10-14)

## Evidence and recovery

Android 2.0.16 `PlayerEgo::update` begins at ELF `0x9ce38`.
At `0x9ce4c` it saves the incoming `Radar*` (`r2`) in `r8`.
The asteroid branch at `0x9dd18–0x9dd44` loads the target at
`PlayerEgo+0x1bc`, checks it for null, then calls
`KIPlayer::isDying` and `KIPlayer::isDead` on that target. At
`0x9dd3e` it moves `r8` into the third call argument (`r2`) before
calling `dockToAsteroid(this, nullptr, radar)`. The next branch
goes directly to the stack-check epilogue at `0x9ef92`, without
running the ordinary post-update path. The docking-point branch
at `0x9dd7a–0x9dd9e` performs the same target-state checks and
also branches directly to the epilogue after teardown.

The IDA export at `analysis/gof2_libgof2hdaa_full_ida.c` lines
84140–84165 supports these checks. The Ghidra export shows the
target-state checks but mislabels the asteroid teardown radar
argument as null; the raw register transfer above resolves that
ambiguity. The prior C++ checked `PlayerEgo::isDead()` instead of
the target's two states, called asteroid teardown with a null
radar, and ran `PE_upd_post` after both teardown paths. Both
branches are now restored to the ARM sequence.

The host smoke extracts the production docking-state fragment
from `PlayerEgo::update` unchanged and compiles it with the
production `dockToAsteroid` method. It checks a live target
continues the approach, dying/dead asteroid targets unlock the
actual radar and clear docking state, and a dead docking-point
target takes its teardown path. This is a bounded branch test,
not execution of the whole `update` method. Run:
`python tools/run_player_ego_dock_asteroid_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe`.

Focused `update` source-shape is **12.2% → 12.7%**, while strict
fuzzy is **4.8% → 4.1%**. The function is neither linked-exact nor
byte-exact; its broader body is still far from the original.
Local focused reports are
`_work/player_update_docking_before_20261014.json` and
`_work/player_update_docking_after_20261014.json`.

Native `gof2`, all 206 ARM translation units, the updated docking
smoke, the `stopMining` smoke and 37 verifier tests pass (one
skipped). The full ARM corpus remains **4,323/4,524** original
functions covered across 4,718 comparisons (201 missing, zero
wrong-type), **93.10%** average source-shape, **81.51%** strict
fuzzy, **2,403 linked-exact**, **1,008 byte-exact** and 123
extras. Pair-set comparison found zero exact additions or losses
and no compared-function changes. The local report is
`_work/player_update_docking_full_20261014.json`.

## Remaining risk and next priority

The smoke substitutes surrounding game services. Full `update`
gameplay and ARM32 runtime were not tested. The original branch
also enters asteroid teardown when the target pointer is null,
while the teardown reads that pointer before unlocking radar;
whether this state is reachable in a real session needs a focused
control-flow/runtime audit. Next, examine that path and the
`approachAsteroid` call arguments without relying on the very low
whole-function shape score as a semantic oracle.
The later [ARM32 layout audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md)
shows the underlying PlayerEgo member offsets are still wrong;
the branch smoke cannot establish ABI-correct runtime behavior.
