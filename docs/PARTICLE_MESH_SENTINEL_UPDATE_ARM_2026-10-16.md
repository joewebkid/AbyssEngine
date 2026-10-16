# ParticleSystemMesh dormant trail sentinel, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `ParticleSystemMesh::updateSingle(int,float)` at `0x1868b8`–`0x1868c8`,
the original calls `updateTrailEdges`, checks whether the particle age
is the trail sentinel `-2`, and branches to the function epilogue when
`newSectionStarted` at `+0x90` is zero. It does not increment age or
update color on that path. When a new section has started, it finishes
the pending trail particle and continues with age/color/lifetime work.

The recovered C++ previously skipped the finish call but still fell
through to age and color updates. The extracted production method in
`run_particle_mesh_trail_smoke.py` reproduced this error before the
fix: the pending `-2` particle changed age. The source now returns
on the confirmed dormant-sentinel branch. Host tests at `-O0` and
`-O2` cover dormant, newly resumed and ordinary ongoing trail states.
`IParticleSystem::update` skips only age `-1`, so this `-2` path can be
reached on successive frames; preserving the sentinel is observable
lifecycle behavior.
The fixture stubs mesh rendering and neighboring methods, so it proves
the control-flow/state boundary, not GPU output or the full neighboring
algorithms.

Native `gof2`, **208/208** ARM translation units, the particle fixture
and 40 verifier tests (one existing skip) pass. Focused `updateSingle`
changes from **97.2% source-shape /
69.2% strict fuzzy** to **97.2% / 70.5%**; it remains nonexact. Reports:
`A:/Galaxy on Fire new version/_work/particle_mesh_update_sentinel_before_20261016.json`
and `particle_mesh_update_sentinel_after_20261016.json`.

Full ARM report:
`A:/Galaxy on Fire new version/_work/particle_mesh_update_sentinel_full_20261016.json`.
It compares **4,323/4,524** original functions, with **201 missing**,
**123 extras**, zero skipped, **93.67%** average source-shape and
**82.77%** strict fuzzy. Both exact `(unit,symbol)` sets are unchanged
from `particle_emit_direct_split_full_20261016.json`: **2,513
linked-exact** and **1,074 byte-exact**, zero losses or gains. Live
ARM32 execution, mesh output and other trail lifetime branches remain
unverified. Next priority: audit the neighboring trail color/expiry
path against ARM and a recording canvas fixture.
