# ParticleSystemMesh ordinary-emission dispatch, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA analysis of `_ZN18ParticleSystemMesh4emitEi` at `0x18618c`
shows a tail branch at `0x1861a2` to `ParticleSystemMesh::emitTrail(int)`
when flag `0x8000` is set and a tail branch at `0x1861a6` to
`IParticleSystem::emit(int)` otherwise. The same body sets
`newSectionStarted` at `+0x90` when emission or rendering is disabled.

The reconstructed source already had the flag and state branches but sent
the ordinary path to declared-only `_psm_emitUsual`, not to the base
method. It now calls the existing `IParticleSystem::emit(int)` body.
The rebuilt ARM object has an `R_ARM_THM_JUMP24` relocation to
`_ZN15IParticleSystem4emitEi`, matching the original target name.
The separate trail branch still uses the previously documented
forwarding shim; masked-byte equality of `emit` does not establish that
trail branch's destination. This package does not revise the large base
emitter algorithm.

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` now extracts the production `emit`,
`emitTrail` and forwarding bodies. It checks ordinary/base dispatch,
trail dispatch, flag-`0x80` suppression and disabled-emission section
reset, alongside the earlier trail-state cases. Native `gof2`, **206/206**
ARM units, and **40** verifier tests (one existing skip) pass.

Focused `ParticleSystemMesh::emit` remains **100% strict fuzzy / 100%
source-shape and linked-exact**, not byte-exact. `emitTrail` remains
**37.5% / 94.9%** and nonexact. Full corpus remains **4,323/4,524**
original unique functions compared, **201 missing**, **124 extras**, zero
skipped, **93.62%** average source-shape, **82.69%** strict fuzzy,
**2,507 linked-exact** and **1,074 byte-exact**. Exact `(unit,symbol)`
pair-set comparison with the previous full report shows zero losses or
gains. Reports: `A:/Galaxy on Fire new version/_work/particle_emit_trail_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/particle_emit_dispatch_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/particle_emit_dispatch_full_20261016.json`.

The base `IParticleSystem::emit` is present but itself only **88.5%**
source-shape and **17.9%** strict fuzzy in this corpus. Its input and
particle-lifetime behavior, actual ARM32/GPU output and the trail shim's
call-target parity remain unverified. Next priority: audit the base
emission branches against ARM and a controlled particle-state fixture.
