# MeshIntersect shader flag — Android ARM, 2026-10-16

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `AbyssEngine::MeshIntersect` at `0x6d818`.
REA/IDA assembly at `0x6da90–0x6dab0` loads the byte
`Engine::enableShader`, stores the average triangle V coordinate,
then replaces it with `1.0f - V` only when that byte is nonzero.
The address, symbol, branch and floating-point stores are visible
in the ARM instructions; the decompiler's guessed return type is
not used as evidence.

The recovered function instead dereferenced
`g_MeshIntersect_flipVFlag`, a zero-initialized pointer with no
source-tree assignment. Any successful triangle hit could therefore
crash before returning its UVs. It now reads the existing
`Engine::enableShader` byte. The unused placeholder declaration
and definition were removed; triangle search and UV averaging were
not changed.

`python tools/run_mesh_intersect_flag_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe`
compiles the unchanged production function body in a host fixture.
It checks a triangle hit, a miss, an empty index list, and V output
with shader mode both off and on. This is representative geometry,
not an exhaustive collision or Android runtime test.

Native `gof2`, **206/206** ARM translation units, **40** verifier
tests (one pre-existing skip), and the host fixture pass. Focused
source-shape/strict fuzzy improves **84.7%/43.0% → 86.2%/43.7%**;
the function remains non-exact. The rebuilt ARM object references
the named `Engine::enableShader` symbol, not the placeholder.

The full corpus compares **4,323/4,524** unique original functions,
with **201 missing**, **123 extras**, zero skipped, **93.59%/82.63%**
average source-shape/strict fuzzy, **2,503 linked-exact** and
**1,074 byte-exact**. Against the prior sprite-creation baseline,
exact `(unit,symbol)` sets have zero losses and zero gains. Reports
are on A: at `_work/mesh_intersect_flag_before_20261016.json`,
`_work/mesh_intersect_flag_after_20261016.json`, and
`_work/mesh_intersect_flag_full_20261016.json`.

Live Android ARM32 behavior, nonrepresentative triangle geometry,
and the remaining `EngineFlags.cpp` placeholders are unverified.
The next bounded priority is to identify an original global target
for one remaining placeholder before changing its source.
