# Base particle emission timing, Android ARM (2026-10-16)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA analysis of `_ZN15IParticleSystem4emitEi` at `0x181dc4` identifies
the speed-squared input in `S28` and the Newton inverse-square-root
factor in `S17` (`0x181fc4`–`0x181fd0`). The inverse-speed estimate is
`S26 = S17 * S2`. At `0x18268c`–`0x182698`, the original computes the
per-particle update delay as `S26 * (S17 * (count - phase) * 1000)`.
The inverse *travel* length is held separately at `0x181f18` and is
used to obtain the traveled distance, not this delay.

The C++ body had multiplied the inverse-speed estimate by inverse
travel length, making the delay depend on the frame's travel distance.
It now retains the speed Newton factor and uses it in the final delay.
This is a behavior correction, not a claim that the whole emitter is
recovered. The extracted-production-body smoke in
`tools/run_particle_emit_smoke.py` calls `IParticleSystem::emit` for
150 ms and 200 ms frames with the same 10-unit/s speed and two emissions.
The first particle must receive the same speed-based delay in both
frames; the old travel-based formula fails this case.

Validation: `python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe`, native `gof2` build, 206/206 ARM units,
and 40 verifier tests (one existing skip) pass. Focused ARM comparison
of `IParticleSystem::emit` is 88.5% source-shape and 17.5% strict fuzzy;
it remains nonexact. The strict fuzzy figure is down from 17.9% in the
prior report; no machine-code parity is claimed for this change.
Full ARM corpus: **4,323/4,524** original unique functions compared,
**201 missing**, **124 extras**, zero skipped, **93.62%** average
source-shape, **82.69%** average strict fuzzy, **2,507 linked-exact**
and **1,074 byte-exact**. Exact `(unit, symbol)` pair-set comparison
against `particle_emit_dispatch_full_20261016.json` shows zero losses
or gains. Focused and full reports are
`A:/Galaxy on Fire new version/_work/particle_emit_timing_focused_20261016.json`
and `A:/Galaxy on Fire new version/_work/particle_emit_timing_full_20261016.json`.

This smoke isolates timing and ring advancement with a deterministic
particle set; it does not validate all random emission, GPU output,
particle lifetime, or live ARM32 behavior. Next work should examine
remaining emitter branches and their call targets, then run broader
stateful particle scenarios before attempting shape-only changes.
