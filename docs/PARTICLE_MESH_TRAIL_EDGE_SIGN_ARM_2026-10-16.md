# ParticleSystemMesh trail-edge Y sign, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `ParticleSystemMesh::updateTrailEdges(int,int)`, the original ARM
negates all three components of the first edge movement before
`MeshTranslatePoint`: X by sign-bit XOR, Y and Z by VFP `VNEG` at
`0x18671a`–`0x186734`. The second edge repeats this at
`0x18678e`–`0x1867ac`. The paired positive movements keep all three
components positive. The recovered C++ incorrectly passed positive Y
in both negative calls, making one side of the trail move in the wrong
vertical direction.

The particle trail fixture now extracts the production
`updateTrailEdges` body and records `MeshTranslatePoint` arguments.
With nonzero X/Y/Z for both edge vectors, it failed before the fix
and passes after correcting both Y signs. The existing dormant,
resumed and ongoing trail-update checks still pass. Host fixture runs
at `-O0` and `-O2`; it checks submitted geometry deltas, not a live
GPU frame.

Native `gof2`, **208/208** ARM translation units, and 40 verifier
tests (one existing skip) pass. Focused `updateTrailEdges` moves from
**96.1% source-shape / 67.2% strict fuzzy** to **96.8% / 73.4%**;
it is still nonexact. Reports:
`A:/Galaxy on Fire new version/_work/particle_trail_edge_sign_before_20261016.json`
and `particle_trail_edge_sign_after_20261016.json`.

The full ARM report
`A:/Galaxy on Fire new version/_work/particle_trail_edge_sign_full_20261016.json`
compares **4,323/4,524** original functions, **201 missing**,
**123 extras**, zero skipped, **93.67%** average source-shape and
**82.77%** strict fuzzy. Exact `(unit,symbol)` sets are unchanged
against `particle_mesh_update_sentinel_full_20261016.json`:
**2,513 linked-exact**, **1,074 byte-exact**, zero losses/gains.
Live ARM32 rendering and neighboring color/expiry output are not
verified here. Next priority: test the mesh color and expiry calls
with a recording canvas against their ARM call order.
