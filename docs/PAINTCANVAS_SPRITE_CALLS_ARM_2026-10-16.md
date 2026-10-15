# PaintCanvas sprite-system call contracts — Android ARM, 2026-10-16

## Original evidence and correction

The original Android 2.0.16 ELF has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Direct Thumb disassembly establishes the wrappers' checks and calls:

| Wrapper start / call | Original callee | Arguments at call |
| --- | --- | --- |
| `SpriteSystemSetAllUv` `0x7a7bc` / `0x7a7f0` | `SpriteSystemSetAllUv(float,float,float,float,SpriteSystem*)` | four floats, array-selected sprite pointer |
| `SpriteSystemSetUv` `0x7a7f8` / `0x7a838` | `SpriteSystemSetUv(unsigned short,float,float,float,float,SpriteSystem*)` | sub-index, four floats, sprite pointer |
| `SpriteSystemSetRGBA` `0x7a840` / `0x7a880` | `SpriteSystemSetRGBA(unsigned short,float,float,float,float,SpriteSystem*)` | sub-index, four floats, sprite pointer |

Each wrapper rejects an out-of-range system index or null sprite.
The latter two also reject an out-of-range sub-index using the 16-bit
sprite count at offset zero. The recovered C++ already generated
matching instructions outside relocations but routed each call to an
undefined `paintcanvas_ext_sprite_*` symbol. The three existing
implemented helpers in `AbyssEngine.cpp` are now called with their
actual signatures; no check or float ordering was changed.

## Validation and limits

Native `gof2`, all **206/206** ARM translation units and **40** verifier
tests (one pre-existing skip) pass. Focused before/after comparison for
the three wrappers remains **100% source-shape / 100% strict fuzzy**,
**three linked-exact / zero byte-exact**. Each direct call-target
relocation now matches the original ELF's named callee.

| Full-corpus measure | Before | After |
| --- | ---: | ---: |
| Unique compared / original | 4,323 / 4,524 | 4,323 / 4,524 |
| Average source-shape / strict fuzzy | 93.59% / 82.63% | 93.59% / 82.63% |
| Linked-exact / byte-exact | 2,503 / 1,074 | 2,503 / 1,074 |
| Matching direct calls / mismatch candidates | 2,351 / 74 | 2,354 / 71 |

Exact `(unit,symbol)` sets have zero losses or gains. Missing remains
201, extras 123, skipped zero; the call audit still has 647 unresolved
sites elsewhere. Reproducible local reports are on A: at
`A:/Galaxy on Fire new version/_work/paintcanvas_sprite_calls_before_20261015.json`,
`paintcanvas_sprite_calls_after_20261015.json`,
`paintcanvas_sprite_calls_full_20261015.json`, and
`arm_call_targets_after_sprite_calls_20261015.json`.

This closes three symbol/ABI call routes, not a proof of visible sprite
output or helper-body equivalence. Original ARM32 execution, live GL,
and complete UV/RGBA data behavior remain unverified. The next bounded
step should test representative sprite data mutations through the
actual helpers and audit another call cluster only after that result.
