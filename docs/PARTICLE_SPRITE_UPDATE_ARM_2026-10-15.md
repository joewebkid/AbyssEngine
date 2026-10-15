# ParticleSystemSprite ordinary update (2026-10-15)

## Evidence and repair

REA/IDA read the Android 2.0.16 ARM `libgof2hdaa.so` method
`ParticleSystemSprite::updateSingle(int,float)` at `0x1830b4` (binary SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The area-exit flag at `0x1830da–0x1830e0` delegates to the separately
recovered `updateAreaExitParticle`. In the ordinary path, instructions
confirm three previously incorrect behaviors:

- Expired particles are marked age `-1`, positioned at `(2^32, 2^32, 2^32)`
  and set to size zero (`0x18311c–0x183168`). The old body used
  `(age, 0, 0)` and left size unchanged.
- The preceding animation-frame index subtracts the integer-converted `dt`
  from `age - 1` before division (`0x18322c–0x183252`). The old body
  converted the result of the floating subtraction instead; they differ
  at fractional frame boundaries. The UV fractional split uses the ARM
  `1/1024` bias at `0x183278–0x183288`, formerly zero.
- Movement uses two vector multiplications, `velocity * dt * 0.001`, before
  `SpriteSystemAddPosition` (`0x1832f0–0x18332a`). The old body omitted the
  millisecond-to-second conversion.

The size delta uses the signed integer at setting offset `+0x44`
(`posBase`) multiplied by `dt * 0.001` (`0x18319c–0x1831c2`). The old
source was already correct here. Layout assertions for `colorFlag` (`+0x40`),
`posBase` (`+0x44`) and `posSpread` (`+0x48`) caught a proposed incorrect
size-field edit before acceptance. They also revealed the preceding
area-exit recovery's mistaken `+0x40` field identification; that method
now reads `colorFlag`, not reinterpreted `posBase` bits.

The existing `tools/run_particle_area_exit_smoke.py` now extracts the real
`updateSingle` body, `updateAreaExitParticle`, `setAlpha`, and their source
constants. Its focused fixtures check the ordinary size, movement, RGBA,
UV wrap, fractional frame boundary, expiry sentinel and size reset, as well
as the earlier area-exit branches. This is a host check with fixture canvas,
math and settings, not an original `.so` or OpenGL execution. The built-in
area-exit sets 4 and 7 leave `colorFlag` at zero; configured overrides and
live behavior remain open.

Native `gof2`, **206/206** ARM translation units, the production-body smoke
and 37 verifier tests (one skip) pass. Focused `updateSingle` source-shape/
strict fuzzy improves **87.3%/39.8% → 92.9%/49.0%**, with 216 original
versus 221 reconstructed instructions; it is not linked- or byte-exact.
The adjacent corrected `updateAreaExitParticle` reaches **94.6%/66.7%**
and remains non-exact.
REA lacks complete body ranges and indirect-call coverage, so live particle
timing, field overrides and rendering remain to be checked on a controlled
ARM/iOS runtime.

Full ARM report `_work/particle_sprite_update_full_20261015.json` compares
**4,323/4,524** original functions, with **201 missing**, **123 extras** and
zero skipped. Average source-shape is **93.45%** (unchanged from the preceding
area-exit report); strict fuzzy is **82.35%** (previously 82.34%). Both
linked-exact (**2,485**) and byte-exact (**1,065**) symbol sets are
identical to the preceding full report: zero losses and zero gains.
