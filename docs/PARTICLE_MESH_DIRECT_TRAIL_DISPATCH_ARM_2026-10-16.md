# ParticleSystemMesh direct trail dispatch, Android ARM (2026-10-16)

Primary binary: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x1861a2`, original `ParticleSystemMesh::emit(int)` tail-branches to
`ParticleSystemMesh::emitTrail(int)` when bit `0x8000` is set; at
`0x1861a6`, the ordinary path tail-branches to `IParticleSystem::emit(int)`.
The reconstructed trail path previously used a separate `_psm_emitTrail`
forwarder, adding a symbol absent from the original. Although `emit` was
linked-exact after normalized comparison, its target was not the original
method.

The source now calls `emitTrail` directly. Its existing body was moved,
without changing its algorithm, to `ParticleSystemMeshTrail.cpp`. Keeping
caller and callee in separate translation units makes the ARM object emit
an `R_ARM_THM_JUMP24` relocation to
`_ZN18ParticleSystemMesh9emitTrailEi`; the ordinary path still relocates
to `_ZN15IParticleSystem4emitEi`. A direct call with both methods in the
same translation unit was tested and rejected: its call bytes normalized
identically but lost `emit`'s linked-exact result because no relocation
was emitted. The forwarder was removed from `ParticleSystemManager.cpp`.

The extracted production `emit` and `emitTrail` methods pass the
`run_particle_mesh_trail_smoke.py` host fixture, including both dispatch
paths, suppressed emission, section reset and trail state. Native `gof2`,
**208/208** ARM translation units, and **40** verifier tests (one existing
skip) pass. Focused `emit` is **100%** source-shape and strict fuzzy,
**linked-exact** but not byte-exact. `emitTrail` remains nonexact; after
the move it is **94.0%** source-shape and **38.9%** strict fuzzy.

Full report:
`A:/Galaxy on Fire new version/_work/particle_emit_direct_split_full_20261016.json`.
Against `objectgun_update_offsets_full_20261016.json`, coverage is still
**4,323/4,524** original functions, with **201 missing**, no skipped
or wrong signatures, **93.67%** average source-shape and **82.77%**
strict fuzzy. Both exact `(unit,symbol)` sets are unchanged at **2,513
linked-exact** and **1,074 byte-exact**. Extra symbols fall **124 → 123**.
This establishes the static ARM call target, not runtime ARM32/GPU
behavior. The base emitter and trail emitter algorithms remain
incompletely matched and need separate behavioral audits.
