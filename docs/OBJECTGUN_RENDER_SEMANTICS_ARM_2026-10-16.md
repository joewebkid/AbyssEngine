# ObjectGun render gating and animation (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::render()` begins at `0x15fda8`.

The ARM function loads `50000.0f` at `0x15fec0` and compares each
gun position's X against it at `0x15fefc-0x15ff04`; equality skips
the draw and counts an inactive slot. `Gun` construction also
initializes its position X to `50000.0f`. The previous C++ tested
`-1000.0f` and therefore rendered inactive entries.

The turret-visible rotation passes `0.0f`, the float-PI literal
at `0x1600cc` (`0x40490fdb`), and `spinAngle` to
`MatrixSetRotation` at `0x15fe90`. The previous source supplied
`spinAngle, 0, 0`, rotating about a different axis. The ARM
loads `1000.0f` at `0x15fec4`, then divides the lifetime by it
in both visible and world-orientation branches at `0x15ffd4`
and `0x1602a6`; C++ previously divided by 750.

The world-orientation branch passes the direction at `this+0x50`
first and normalized side at `this+0x68` second to `VectorCross`
at `0x160242-0x160248`, then normalizes the result as up. The
previous C++ reversed them, flipping up. The mine-wobble step
uses `deltaTime * 0.0005f` (`0x15fecc`, `0x16016a-0x16016e`),
not `deltaTime * 0.02f`.

The extracted production fragments in
`tools/run_objectgun_canvas_calls_smoke.py` exercise all five
corrections under host `-O0` and `-O2`: inactive sentinel,
rotation arguments, lifetime scaling in both branches, vector
cross order and wobble sign/step. Native `gof2`, all **207** ARM
translation units and **40** verifier tests (one existing skip)
pass. Focused `render` source-shape/strict fuzzy changes
**89.6%/39.7% -> 89.7%/41.5%**; it remains nonexact.

The full ARM report compares **4,323/4,524** original functions:
**201 missing**, **124 extras**, zero skipped or wrong signatures,
**93.67%/82.77%** average source-shape/strict fuzzy, **2,513
linked-exact / 1,074 byte-exact**. Against the preceding
`objectgun_visible_axes_full_20261016.json`, neither exact
`(unit,symbol)` set gained or lost a member. Full report:
`A:/Galaxy on Fire new version/_work/objectgun_render_semantics_full_20261016.json`.

Live ARM32 frame output, GPU state and timing are unverified. The
fixture checks extracted expressions, not the entire `render` loop.
The next ObjectGun priority is the remaining render branches and
their runtime-facing canvas/geometry effects; a separate global
priority is triage of the 201 missing functions.
