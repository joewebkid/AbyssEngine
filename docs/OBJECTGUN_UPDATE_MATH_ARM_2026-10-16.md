# ObjectGun update math (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::update(int)` starts at `0x15f964`.

The original calls the typed by-value
`AbyssEngine::AEMath::MatrixRotateVector(const Matrix&,const Vector&)`
at `0x15fada` and `0x15fc74`; the former declared-only free shim had
the wrong symbol/signature and used a `Matrix` as the vector output.
The first result is added to the gun's position and the second is
passed to `AEGeometry::translate` after setting its matrix.

The non-turret branch copies the camera matrix at `0x15fb5c-0x15fb6a`.
However, `MatrixGetDir` at `0x15fb78` receives the *player matrix*
at `sp+0x78`, whereas `MatrixGetUp` at `0x15fb84` receives the
camera matrix at `sp+0x28`. The old C++ supplied the camera matrix
to both calls. The restored calls use the typed vector-returning
functions and their distinct inputs.

The turret branch obtains `AEGeometry::getMatrix()` from the
`PlayerEgo` object's pointers at offsets `0x2c` and `0x28`, in that
order (`0x15fb16-0x15fb24`). It passes both matrices to the
by-value matrix `operator*` at `0x15fb2a`, stores the result, and
later passes it to `setMatrix` at `0x15fc7e`. The old C++ called
`MatrixMultiply(const Matrix&,const Matrix&)` on a scratch matrix
and the gun's geometry; that helper discards its product.

The extracted production fragments in
`tools/run_objectgun_canvas_calls_smoke.py` check the distinct
camera/player direction inputs and ordered turret multiplication
under host `-O0` and `-O2`. Native `gof2`, all **207/207** ARM
translation units, and **40** verifier tests (one existing skip)
pass. Focused `update` source-shape/strict fuzzy changes from
**87.8%/29.0%** to **87.9%/31.0%**; it is still nonexact. A tried
directly initialized scratch matrix kept behavior but worsened the
focused metrics to **86.8%/29.6%**, so the better-shaped assignment
was retained.

The full ARM corpus compares **4,323/4,524** original functions,
with **201 missing**, **124 extras**, zero skipped and zero wrong
signatures, **93.66%/82.75%** average source-shape/strict fuzzy,
and **2,510 linked-exact / 1,074 byte-exact**. Exact `(unit,symbol)`
sets against the preceding report have zero gains and zero losses.
Reports:
`A:/Galaxy on Fire new version/_work/objectgun_render_math_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_update_math_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_update_math_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_update_math_full_20261016.json`.

These checks do not validate every branch of `update`, the math
implementation itself, or live ARM32 rendering. Explosion timing
and full native game startup remain unverified.
