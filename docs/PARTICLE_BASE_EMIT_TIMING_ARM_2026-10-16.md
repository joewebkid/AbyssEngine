# Base particle emission position and timing, Android ARM (2026-10-16)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA analysis of `_ZN15IParticleSystem4emitEi` at `0x181dc4` identifies
the speed-squared input in `S28` and the Newton inverse-square-root
factor initially in `S17` (`0x181fc4`–`0x181fd0`). The inverse-speed
estimate is `S26 = S17 * S2`. The inverse *travel* length is stored at
`0x181f18` and later passed as the second vector multiplier at
`0x1822e2`–`0x1822ee`, which places particles along the frame's path.
The C++ body had incorrectly used the inverse-speed estimate for this
position multiplier.

Crucially, `S17` is reused at `0x1822c4` or `0x1822cc` for the
emission step: traveled distance divided by count, or the configured
step for flag `0x10`. At `0x18268c`–`0x182698`, the original update
delay is `S26 * (S17 * (count - phase) * 1000)`, then capped by frame
delta. The first timing audit wrongly treated `S17` as the unchanged
Newton factor. This follow-up corrects that interpretation and uses
the retained emission step. It does not claim recovery of the whole
emitter.

The extracted-production-body smoke in `tools/run_particle_emit_smoke.py`
calls `IParticleSystem::emit` for 150 ms and 200 ms frames with the
same 10-unit/s speed and two emissions. It checks midpoint positions
and frame-proportional first-particle update times, then checks the
configured-step branch independently. These scenarios distinguish
both errors from the ARM behavior.

Validation: `python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe`, native `gof2` build, 206/206 ARM units,
and 40 verifier tests (one existing skip) pass. Focused ARM comparison
of `IParticleSystem::emit` is 88.5% source-shape and 16.7% strict fuzzy;
it remains nonexact. The strict fuzzy figure is down from 17.5% in the
prior report; semantic evidence takes precedence over that score.
Full ARM corpus: **4,323/4,524** original unique functions compared,
**201 missing**, **124 extras**, zero skipped, **93.62%** average
source-shape, **82.69%** average strict fuzzy, **2,507 linked-exact**
and **1,074 byte-exact**. Exact `(unit, symbol)` pair-set comparison
against `particle_emit_timing_full_20261016.json` shows zero losses
or gains. Focused and full reports are
`A:/Galaxy on Fire new version/_work/particle_emit_step_focused_20261016.json`
and `A:/Galaxy on Fire new version/_work/particle_emit_step_full_20261016.json`.

This smoke isolates position, timing and ring advancement with a deterministic
particle set; it does not validate all random emission, GPU output,
particle lifetime, or live ARM32 behavior. Next work should examine
remaining emitter branches and their call targets, then run broader
stateful particle scenarios before attempting shape-only changes.
