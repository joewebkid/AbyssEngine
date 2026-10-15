# SpriteSystemCreate shader flag — Android ARM, 2026-10-16

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `AbyssEngine::SpriteSystemCreate` at `0x86adc`.
REA/IDA assembly at `0x86c22–0x86c30` loads the byte
`Engine::enableShader`. The loop at `0x86c34–0x86c58` always writes
the normal `(0,0,1)` and writes the tangent `(1,0,0)` and binormal
`(0,1,0)` only when that byte is nonzero. These facts are confirmed
by the ARM instructions, not just the decompiler's guessed types.

The recovered function instead dereferenced
`g_SpriteSystem_tangentFlag`. That pointer was zero-initialized in
`EngineFlags.cpp` and had no source-tree assignment; successful
sprite creation could therefore dereference null before filling
normals. The function now reads `Engine::enableShader` directly.
The unused placeholder pointer was removed. Allocation, cleanup,
index generation, and mesh writes were otherwise left intact.

Native `gof2`, **206/206** ARM translation units, **40** verifier
tests (one pre-existing skip), and the separate production UV/RGBA
host smoke pass. Focused `SpriteSystemCreate` strict fuzzy rises
**66.9% → 67.1%**, while source-shape changes **95.2% → 95.0%**;
it remains non-exact. The rebuilt ARM object references the named
`Engine::enableShader` symbol and not the placeholder.

The full corpus still compares **4,323/4,524** unique original
functions, with **201 missing**, **123 extras**, zero skipped,
**93.59%/82.63%** average source-shape/strict fuzzy,
**2,503 linked-exact** and **1,074 byte-exact**. Comparing exact
`(unit,symbol)` sets to the preceding UV/RGBA baseline gives zero
losses and zero gains. Reports are on A: at
`_work/sprite_create_flag_before_20261016.json`,
`_work/sprite_create_flag_after_20261016.json`, and
`_work/sprite_create_flag_full_20261016.json`.

There is no live Android ARM32/GL creation test here. The host
fixture does not execute this entire function: its original `0x14`-
byte SpriteSystem allocation and 32-bit pointers require a matching
32-bit runtime. Other `EngineFlags.cpp` placeholders remain; audit
each original global target before replacing them.
