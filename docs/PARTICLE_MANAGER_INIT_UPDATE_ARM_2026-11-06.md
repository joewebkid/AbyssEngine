# ParticleSystemManager init update call (2026-11-06)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `ParticleSystemManager::init` at `0x1836c6`. After
`initSprites`, `initMesh` and setting `enabled`, it zeroes the two
registers of the `long long` argument and tail-branches through the
veneer at `0x19c8d8`. That veneer resolves to the
`ParticleSystemManager::update(long long)` PLT entry at `0x644c4`.
The only recovered caller, `Level::initParticleSystems`, discards
the result; the original tail call creates no integer return value.

The ARM32 reconstruction previously called the undefined
`_psm_firstUpdate(void*, int, int, int)` placeholder. The method now
calls `update(0)` on every architecture and has a `void` return type.

Focused ARMv7 verification compiles **211/211** translation units.
`init` rises from **97.1% to 100%** source-shape and from **88.9% to
100%** strict fuzzy. It remains non-exact: the reconstructed object
uses a local branch to `update`, while the linked original uses a
veneer to its PLT entry. The raw unmatched-symbol inventory over 211
ARM32 objects falls **233 -> 232**, removing only the placeholder.
Windows native and ARM64 static builds pass, as does strict ARM64
shared linking.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.81%** mean source-shape, **86.85%** strict fuzzy,
**2,791 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match pair is lost or gained. Live
particle behavior and full ARM32 shared linking remain unverified.
