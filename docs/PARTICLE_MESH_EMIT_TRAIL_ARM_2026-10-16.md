# ParticleSystemMesh trail emission, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA analysis of `_ZN18ParticleSystemMesh9emitTrailEi` at `0x18564c`
provided pseudocode and ARM instructions through the stack-protector exit at
`0x185cec`. IDA does not report complete body ranges in this profile, so this
address span is observed instruction evidence, not a formal body-range claim.

The previous C++ symbol was an empty static stub. The recovered body is now
an instance method with the original `(this, delta)` ABI. It reads the selected
particle definition; derives mirrored right/up/direction edges and a transformed
emitter position; handles the optional leading cap; updates live quad edges and
UVs; accumulates frame time; detects a distant new section; finishes the old
particle; advances the ring index; and writes the new section's edges, velocity,
colors, UVs, age and last position. The Android field accesses support signed
emitter coordinates at `+0x78/+0x7c`, a Vector at `+0x80`, and a signed frame
counter at `+0x94`. The ARM-sized class remains `0xa0` bytes.

The already linked-exact `ParticleSystemMesh::emit` uses an external
`_psm_emitTrail` dispatch. A forwarding definition in the separate
`ParticleSystemManager` translation unit makes that branch reach the recovered
method without changing its ARM callsite shape. This introduces **one extra
symbol** versus the original (`123 -> 124`); masked-byte equality of the
caller alone does not prove its call destination. The other `_psm_emitUsual`
route and mesh/math helper shims remain outside this package.

Follow-up on the same date: the ordinary path was subsequently tied to
`IParticleSystem::emit(int)` by the original ARM tail branch and fixed in
[the dispatch audit](PARTICLE_MESH_EMIT_DISPATCH_ARM_2026-10-16.md).

`python tools/run_particle_mesh_trail_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` extracts the unchanged production body into a
small host fixture. It covers initial and continuing sections, ring wrap,
position and edge calculations, UV progression, mask/color propagation,
the optional leading cap, mirrored diagonal edges and wide geometry.
It substitutes a recording canvas and math/neighbor implementations, so it
does not test GPU output, real assets or live ARM32 execution.

Native `gof2`, **206/206** ARM translation units, the host fixture and
**40** verifier tests (one existing skip) pass. Focused `emitTrail` rises
from **0% strict fuzzy / 0% source-shape** (one-instruction stub) to
**37.5% / 94.9%**; both the original and current body have **605**
instructions, but neither linked- nor byte-exactness is claimed. The old
`emit` linked-exact pair is retained. The full ARM corpus compares
**4,323/4,524** original unique functions, **201 missing**, **124 extras**
and zero skipped; average source-shape moves **93.60% -> 93.62%**, while
strict fuzzy rounds to **82.69%**. The exact `(unit,symbol)` pair sets have
zero losses or gains: **2,507 linked-exact**, **1,074 byte-exact**. The
linked-function direct-call audit remains **2,392 matches / 53 mismatch
candidates / 647 unresolved**; it does not audit this nonexact emitter.

Local reports: `A:/Galaxy on Fire new version/_work/aegeometry_lod_full_20261016.json`
(baseline), `A:/Galaxy on Fire new version/_work/particle_emit_trail_focused_20261016.json`,
`A:/Galaxy on Fire new version/_work/particle_emit_trail_full_20261016.json`, and
`A:/Galaxy on Fire new version/_work/particle_emit_trail_call_audit_20261016.json`.

Next priority: recover the separate ordinary-particle emission route and
replace its unresolved dispatch only with an independently confirmed contract;
then exercise actual ARM32 rendering and trail continuity when a controlled
runtime is available. Source-shape alone does not prove visual or lifetime
semantics.
