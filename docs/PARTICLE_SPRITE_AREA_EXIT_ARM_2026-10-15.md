# ParticleSystemSprite area-exit recovery (2026-10-15)

## Source and repair

The previous `ParticleSystemSprite::updateAreaExitParticle(int,float)` was
only a call to the undefined `ParticleSystem_updateAreaExitParticleImpl`.
The native archive could build, but this path was not implemented or linkable
as a complete runtime path.

REA/IDA inspection of Android 2.0.16 ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
identified the method at `0x182d6c`. Instruction anchors:

- `0x182d94–0x182dca`: signed particle-set ID at `+0x6c`, velocity at
  `+0x64`, `velocity * dt * 0.001`, then `SpriteSystemAddPosition`.
- `0x182dd2–0x182e32`: get sprite and matrix positions, subtract, compare
  squared distance with the squared `+0x78` radius.
- `0x182e36–0x182f80`: zero alpha beyond the radius; beyond 101% of its
  squared value, draw four `nextInt(2000)` values, normalize the resulting
  four-component vector when its squared length is at least one, reposition
  the sprite and copy the `+0x58..+0x60` velocity.
- `0x182f86–0x183012`: inner/middle/outer squared-radius alpha bands using
  `+0x7c`, `+0x80`, and `+0x40`, with color from `+0x34`.

Correction to the initial recovery: its `+0x40` field identification was
wrong. The checked ARM layout places the float `colorFlag` at `+0x40`,
integer `posBase` at `+0x44`, and `posSpread` at `+0x48`. The first
implementation reinterpreted `posBase` as float; it now reads `colorFlag`
directly. The two built-in `0x81` area-exit definitions (sets 4 and 7)
leave `colorFlag` at its zero default, which masked the error in those
fixtures. Other configured or runtime-modified settings have not been
exhaustively audited.
REA did not report complete function-body ranges or complete indirect-call
relationships; the listed ranges are instruction anchors, not a proven
complete CFG.

## Verification and limits

`tools/run_particle_area_exit_smoke.py` extracts the production method,
`setAlpha` and `updateSingle` into a focused host test. It covers four alpha bands,
the narrow just-outside band, velocity integration, random rejection and
successful repositioning, all four RNG draws and subsequent state, the
rebirth direction and the assigned velocity. The renderer and math services
are fixtures; this is not a complete OpenGL frame or original-binary runtime
execution. The smoke passes on UCRT64.

The native `gof2` build, all **206/206** ARM translation units and 37
verifier tests (one skipped) pass. Focused ARM source-shape/strict fuzzy
improves from **1.8%/0.9%** (one-instruction stub) to **94.6%/66.7%**
after the field correction, with 220 original versus 224 reconstructed
instructions. This method is not linked- or byte-exact. Live particle
behavior remains unverified; next check configured overrides and controlled
ARM/iOS execution.

The historical full ARM report `_work/particle_area_exit_full_20261015.json`
(before the `colorFlag` correction) compares
**4,323/4,524** original functions at **93.45%** average source-shape and
**82.34%** strict fuzzy (previously 93.43%/82.33%), with **2,485
linked-exact**, **1,065 byte-exact**, **201 missing**, **123 extras**, and
zero skipped. Both exact-symbol sets are identical to the preceding
`aerandom_oracle_full_20261015.json` report.
The corrected field read is included in the subsequent
`_work/particle_sprite_update_full_20261015.json` report, whose exact-symbol
sets remain identical and strict fuzzy average is 82.35%.
