# ObjectGun render bindings (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::render()` begins at `0x15fda8`.

The PIC literals at `0x1600c8`, `0x1600dc`,
`0x160348`, and `0x160354` all resolve to GOT slot
`0x2101f4`, relocated as `Globals::Canvas`.
The turret and muzzle branches call
`PaintCanvas::CameraGetCurrent()` (PLT `0x617f4`) and
`CameraGetLocal(unsigned)` (PLT `0x5ff1c`).
The ego-orientation branch at `0x1600b2-0x1600be` calls
`PaintCanvas::TransformSetLocal(unsigned,const Matrix&)`
(PLT `0x621c0`). The final draw pair at
`0x1601c4-0x1601da` uses that same setter followed by
`PaintCanvas::DrawTransform(unsigned,const Matrix*)`
(PLT `0x6306c`) with a null matrix pointer. It retains
the address of `Globals::Canvas` and reloads the canvas
pointer after the first call.

The literal at `0x160350` resolves to GOT slot `0x21032c`,
relocated as the byte `Globals::iPad`. The nonzero check
at `0x1600fc-0x160116` controls scaling. The former
C++ used null file-static canvas and scale-flag placeholders
and declared-only free call shims. The render bindings now
use typed `PaintCanvas` calls and `Globals::iPad`.

The extracted production fragments in
`tools/run_objectgun_canvas_calls_smoke.py` run under
host `-O0` and `-O2`. They check the iPad flag on/off
scaling and the final set-local/draw order, including a
canvas swap inside `TransformSetLocal`. Native `gof2`,
**207/207** ARM translation units and **40** verifier tests
(one existing skip) pass.

Focused render source-shape/strict fuzzy improves
**86.8%/30.9% -> 90.4%/34.6%**; it remains nonexact.
The full ARM corpus compares **4,323/4,524** original
functions, with **201 missing**, **124 extras**, zero
skipped, **93.66%/82.75%** average source-shape/strict
fuzzy and **2,510 linked-exact / 1,074 byte-exact**.
Exact `(unit,symbol)` sets against the previous full
report show zero gains and zero losses. Reports:
`A:/Galaxy on Fire new version/_work/objectgun_update_bindings_enum_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_render_bindings_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_render_bindings_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_render_bindings_full_20261016.json`.

This establishes these call targets, not full render semantics.
Camera-facing matrix construction, animation/muzzle orientation,
resource ownership, GPU output, and live ARM32 game startup
remain unverified. Those are the next ObjectGun priorities.
