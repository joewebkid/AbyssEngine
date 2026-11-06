# PlayerEgo docking engine-sound calls (2026-11-06)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `PlayerEgo::PlayEngineSound()` twice from
`PlayerEgo::dockToDockingPoint(KIPlayer *, Radar *)`, at `0x9fa7e`
and `0x9fc0a`. Both calls use PLT entry `0x62b68`, with `this` in
`r0`.

The reconstructed ARM32 code called an undefined
`PlayEngineSound_(PlayerEgo *)` helper at both sites. Both now call
the existing typed method directly. This restores the call target
for docking and undocking paths; the sound system's runtime behavior
has not been established by this static comparison.

Focused ARMv7 verification compiles **211/211** translation units.
`dockToDockingPoint` retains **97.7%** source-shape and **66.4%**
strict fuzzy; it is not exact. The raw unmatched-symbol inventory
over 211 ARM32 objects falls **237 -> 236**, removing only
`_Z16PlayEngineSound_P9PlayerEgo` with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: **96.80%** mean source-shape, **86.83%** strict fuzzy,
**2,789 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped
and zero wrong-type. No exact-match pair is gained or lost relative to
the preceding report. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking. Device docking audio and full ARM32
shared linking remain unverified.
