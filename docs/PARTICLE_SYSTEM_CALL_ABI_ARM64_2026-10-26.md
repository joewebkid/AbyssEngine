# Particle-system call ABI recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `AEMath::MatrixGetRight`, `MatrixGetUp`, `MatrixGetDir`, and
`MatrixGetPosition` bodies are at `0x80704`–`0x8072e`; the particle
manager's `reset` body is at `0x183ce8`.

`IParticleSystem` now calls those existing, typed matrix functions instead
of four undefined global stand-ins. `ParticleSystemManager` now dispatches
`reset` through the `IParticleSystem` virtual slot used by the original ARM
code. The mesh-system counter now calls virtual `getQuadCount`, as the
original `addMeshSystem` does at `0x183a56`; the sprite counter calls the
existing `getParticleCount` method. This removes six
undefined call targets without introducing a parallel implementation of
the same behavior. The velocity methods also avoid an extra copy of the
matrix-position result.

Native and AArch64 static archives build. Focused ARMv7 verification
compares **17** `IParticleSystem` and **42** `ParticleSystemManager`
symbol/unit pairs. The manager's `reset` rises **65.4% → 95.9%
source-shape** and **33.3% → 51.7% strict fuzzy**;
`addMeshSystem` rises **89.1% → 96.9% source-shape** and
**90.7% → 96.1% strict fuzzy**. The typed matrix
return changes code generation in `IParticleSystem::emit`
(**88.8% → 87.6% source-shape**) and `resetEmitterVelocity`
(**99.4% → 97.1%**); their strict fuzzy scores also fall from
**19.9% → 14.7%** and **91.4% → 74.3%**, respectively. Both focused
units retain every linked-exact and byte-exact match. The ARMv7 call
sites and the existing matrix bodies support the typed calls despite
that metric tradeoff.

A strict JNI-initialize AArch64 link probe falls **130 → 124** unresolved
names; the broader `JNI_OnLoad` probe falls **347 → 341**. The six removed
names are the four matrix stand-ins, `_ips_reset`, and
`_ips_getParticleCount`. No new undefined names appear. These are
diagnostics: the game `.so` and APK are still not built, and live particle
rendering remains unverified.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,745** pairs, with zero skips. Its averages remain **94.78%
source-shape / 84.29% strict fuzzy** after rounding. The complete
**2,607 linked-exact** and **1,083 byte-exact** symbol sets have zero
losses and zero gains against the previous full report.
