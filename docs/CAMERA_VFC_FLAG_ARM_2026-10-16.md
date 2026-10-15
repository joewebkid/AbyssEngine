# Camera view-frustum flag — Android ARM, 2026-10-16

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `AbyssEngine::CameraIsPointinViewFrustum` at `0x7144c`
and `CameraIsSphereinViewFrustum` at `0x715cc`. REA/IDA assembly
shows the point function loading the byte `Engine::vfc` at
`0x7145c–0x71478`, returning true when it is false. The sphere
function reads the same flag at `0x715e0–0x7160a` and skips the
frustum check if radius is zero or the flag is false. These are
instruction-level observations, not inferred decompiler types.

Both recovered bodies instead dereferenced
`g_Camera_frustumEnabledFlag`, a zero-initialized pointer with no
source-tree assignment. They now read the existing static
`Engine::vfc` byte. The unused placeholder declaration and
definition were removed. No frustum-plane math was changed.

Native `gof2` build, **206/206** ARM translation units, **40**
verifier tests (one pre-existing skip) and the existing
`run_canvas_sphere_frustum_smoke.py` fixture pass. That fixture
checks production canvas wrappers with substitute core functions;
it does **not** exercise the production core frustum calculations.
The rebuilt ARM objects reference the named `Engine::vfc` symbol,
not the placeholder.

Focused point source-shape/strict fuzzy changes **81.1%/53.8% →
82.3%/54.0%**; sphere changes **82.5%/34.9% → 82.4%/46.3%**.
Neither function is exact. The full corpus compares **4,323/4,524**
unique original functions, with **201 missing**, **123 extras**, zero
skipped, **93.59%/82.64%** average source-shape/strict fuzzy,
**2,503 linked-exact** and **1,074 byte-exact**. Compared with the
previous full report, exact `(unit,symbol)` sets have zero losses
and zero gains. Reports are on A: at
`_work/camera_vfc_before_20261016.json`,
`_work/camera_vfc_after_20261016.json`, and
`_work/camera_vfc_full_20261016.json`.

The source tree defines `Engine::vfc = false` but contains no other
assignment found by source search. Whether a missing path enables
view-frustum culling is unresolved. Live Android ARM32 behavior,
actual core frustum math and boundary cases remain unverified. The
next bounded priority is to trace this flag's original writes or
another independently anchored placeholder without changing its
semantics for metric improvement alone.
