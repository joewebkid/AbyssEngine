# PlayerEgo asteroid approach stage zero (2026-10-14)

Android Thumb `PlayerEgo::approachAsteroid` at `0x9f5ac–0x9f8de`
is the primary source, cross-checked with the IDA and Ghidra dumps.
The stage-zero branch was a call to declared-only `PE_aa_approachStep`;
the original branch is inline, so that call is removed.

The original stores asteroid-minus-ship displacement at `+0xec`,
truncates its length to an integer and compares it with the approach
threshold at `+0x1d8`. Outside the threshold it steers toward the
asteroid geometry position at speed `min(handling + 2.7f, 4.0f)` and
sets the near-target flag when the post-move distance is at most
19,999. Inside the threshold it enters state 1 only after the
readiness byte at `+0x1d4` is set. Thumb tests asteroid byte `+0x3c`
before disabling rotation; its field name remains unresolved.

Within 2,000 units beyond the approach threshold, the branch sets
thrust to 1, stops exhaust and plays event 2 on first entry, seeds
the boost timer from `field_0xcc`, disables the follow camera, and
stores normalized geometry up/direction vectors at `+0x1c8/+0xe0`.
An angle at most 0.2 radians or a settle timer at most -1024 sets
readiness and clears the two bytes at `+0x2f4`; otherwise frame time
reduces the timer and the body geometry receives the X-axis rotation.

The extracted production-body smoke covers dying-target early exit,
both speed paths, far and close approach, alignment rotation, state-1
entry, and the existing mining-phase scenarios. Native `gof2`, all
206 ARM translation units and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy rises **40.8%/21.6% →
91.9%/47.3%**; the function is not exact.

Full report `_work/player_approach_asteroid_stage0_full_20261014.json`
compares **4,323/4,524** original functions at **93.37%** average
source-shape and **82.26%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, **201 missing** and zero skipped.
Linked- and byte-exact pair sets have no losses or gains against
`_work/player_dock_point_full_20261014.json`.

The fixture does not prove live asteroid approach or ARM32 runtime.
IDA infers an integer return while Ghidra uses void; the visible
`PlayerEgo::update` caller at IDA line 84150 ignores the result, so
the declaration remains void pending stronger ABI evidence.
Next priority: the declared-only pitch/yaw ramp calculations used by
the adjacent ship controls.
