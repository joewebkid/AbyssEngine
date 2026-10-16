# ObjectGun render math calls (Android ARM)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`;
`ObjectGun::render()` starts at `0x15fda8`.

The prior source declared several free, `void`-returning math shims.
The ARM function instead calls the typed `AbyssEngine::AEMath` functions
which return `Matrix` or `Vector` by value. In the ARM ABI these matrix
returns use a hidden output pointer as well as a distinct mutable input
`Matrix&`. Treating the output pointer as the sole matrix argument left
the orientation matrix unscaled/unrotated and generated the wrong call
targets. The original calls `Matrix::operator*=` for the turret camera
matrix (`0x15fea0`) and ego player-view matrix (`0x160034`); the old
`MatrixMultiply(const Matrix&,const Matrix&)` call discarded its product.

The restored calls now mutate `ObjectGun::orientation` for translation,
scaling and mine wobble, rotate the turret and ego matrices with `*=`,
and use the typed vector cross/normalization/get-direction results.
The source keeps return-value copies only where the ARM disassembly
shows a consumed result. In particular, assigning every returned
matrix to the local scratch matrix added extra `Matrix::operator=`
calls and reduced source-shape; those unused copies were removed.
The identity initializer's unusual `m[14] = 1` is retained because
the ARM code explicitly writes that offset.

The extracted production fragments in
`tools/run_objectgun_canvas_calls_smoke.py` check that iPad scaling
changes `ObjectGun::orientation` and that ego rotation changes both
the local and player-view matrices, at host `-O0` and `-O2`.
Native `gof2`, 207/207 ARM translation units and 40 verifier tests
(one existing skip) pass.

Focused render source-shape changes **90.4% -> 89.6%** while strict
fuzzy improves **34.6% -> 39.7%**. This is a semantic/ABI repair,
not an exact match. The full ARM report compares **4,323/4,524**
original functions, with **201 missing**, **124 extras**, no skipped
comparisons or wrong signatures, **93.66%/82.75%** average
source-shape/strict fuzzy, and **2,510 linked-exact / 1,074
byte-exact**. Exact `(unit,symbol)` sets against the preceding full
report have zero gains and zero losses. Reports:
`A:/Galaxy on Fire new version/_work/objectgun_render_bindings_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_render_math_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_render_math_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_render_math_full_20261016.json`.

The fixture checks call routing and mutation, not frame-perfect
rendering or live ARM32 GPU output. `ObjectGun::update` still has
separate declared-only math shims, and the exact behavior of all
render branches is not yet proven.
