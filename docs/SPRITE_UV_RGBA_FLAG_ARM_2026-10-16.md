# Sprite UV/RGBA data helpers — Android ARM, 2026-10-16

## Evidence and correction

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `AbyssEngine::SpriteSystemSetAllUv` at `0x86c6c`, `SetUv` at
`0x86cd4`, and `SetRGBA` at `0x86d44`. REA/IDA assembly for `SetUv`
loads the byte `Engine::enableShader` at `0x217abb` and flips the two V
coordinates only when it is zero (`0x86d04–0x86d22`). The original
also checks `idx < sys->count` before writing eight floats at
`mesh->texCoords + idx * 32`; `SetRGBA` writes RGBA to four vertices
at `mesh->colors + idx * 64`. `SetAllUv` writes eight floats per sprite.
These addresses, checks, offsets, and store order are visible in the
original ARM instructions, not inferred solely from pseudocode.

The recovered `SetUv` instead dereferenced
`g_SpriteSystem_uvFlipFlag`, a separate pointer defined as null and
never initialized in the source tree. This was a real native-runtime
null-dereference risk, not merely a source-shape discrepancy. It now
reads the existing `Engine::enableShader` static byte. The unused
placeholder pointer definition and declaration were removed. No
coordinates, bounds checks, or public signatures changed.

## Validation

`python tools/run_sprite_uv_rgba_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`
compiles the three unchanged production function bodies in a host
fixture. It checks null/empty systems, out-of-range indices, writes
to the selected sprite only, UV inversion with `enableShader == 0`,
uninverted UV with `enableShader != 0`, and RGBA replication to four
vertices. This is a host behavior check, not a live Android/GL test.

Native `gof2`, **206/206** ARM translation units, the fixture, and
**40** verifier tests (one pre-existing skip) pass. Focused `SetUv`
source-shape/strict fuzzy improves **87.9%/46.6% → 88.9%/52.8%**;
it remains non-exact. The unchanged `SetAllUv` remains **100%/100%**
and byte-exact; unchanged `SetRGBA` remains **76.4%/53.3%** and
non-exact. The rebuilt ARM object references the named
`Engine::enableShader` symbol and no longer references the placeholder.

The full corpus compares **4,323/4,524** unique original functions,
with **201 missing**, **123 extras**, zero skipped, **93.59%/82.63%**
average source-shape/strict fuzzy, **2,503 linked-exact** and
**1,074 byte-exact**. Exact `(unit,symbol)` sets have zero losses
and zero gains versus the preceding sprite-call baseline. Reports:

- `A:/Galaxy on Fire new version/_work/sprite_uv_flag_before_20261016.json`
- `A:/Galaxy on Fire new version/_work/sprite_uv_rgba_after_20261016.json`
- `A:/Galaxy on Fire new version/_work/sprite_uv_flag_full_20261016.json`

The test does not establish visible sprite output or full ARM32
runtime behavior. The remaining sprite creation path still uses
`g_SpriteSystem_tangentFlag`, another uninitialized placeholder
pointer; audit its original global target separately before changing it.
