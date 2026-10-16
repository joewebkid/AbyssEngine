# Trail canvas routing (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `Trail::render()` at `0x15c36c` loads the canvas through
GOT slot `0x2101f4`; the ELF relocation for that slot names
`Globals::Canvas` (`_ZN7Globals6CanvasE`). Its tail branch via veneer
`0x19bd98` reaches the PLT entry for
`PaintCanvas::DrawTransform(unsigned, const Matrix*)`, with the trail's
transform index and a null view matrix. The constructor at `0x15bd58`,
`update(float,float,float,float,float,float)` at `0x15bfac`, and
`setWidth(int)` at `0x15c184` use the same GOT slot. Each branch of
`changeType(int)` at `0x15bef4` also reads that slot and tail-branches
via veneer `0x19c5e8` to `PaintCanvas::TransformSetColor(unsigned,unsigned)`.

The old C++ used ten different file-static `PaintCanvas**` pointers,
all initialized to null, plus two declared-only render/color hooks.
The affected methods now read the source-backed `Globals::Canvas` and
call the existing typed canvas methods. Trail layout, point updates,
mesh construction, and color values are unchanged. The native static
library defines `Globals::Canvas`; the two undefined Trail hooks are
no longer referenced. `Globals::Canvas` is initialized from the
application's paint canvas in `Globals.cpp`.

`python tools/run_trail_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` extracts the production `render` and
`changeType` bodies. Its recording fixture checks the render transform
and null view, the 13 tested type/color selections, and routing after
the global canvas pointer changes. It does not execute mesh creation,
point updates, or live GPU rendering. Native `gof2`, 207/207 ARM units,
and 40 verifier tests (one existing skip) pass.

Focused Trail verification compares 12 functions. Mean source-shape
rises **93.65% -> 98.76%**; strict fuzzy rises **83.60% -> 88.72%**.
`render` rises **69.3%/54.5% -> 100%/100%** source-shape/strict fuzzy
and becomes linked-exact; `changeType` rises **75.1%/58.2% ->
100%/100%**, nonexact. Focused linked-exact rises **6 -> 7** and
byte-exact stays **1**. The individual strict fuzzy scores of the
constructor, float `update`, and `setWidth` fall slightly despite
improved source-shape; the correct global binding takes precedence
over tuning those residual compiler/layout differences.

The full corpus compares **4,323/4,524** original functions with
**201 missing**, **124 extras**, zero skipped, **93.66%/82.73%**
average source-shape/strict fuzzy, **2,509 linked-exact / 1,074
byte-exact**. Against the preceding report, there is one linked gain
(`Trail::render`) and zero linked or byte losses. Reports:
`A:/Galaxy on Fire new version/_work/aegeometry_render_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/trail_canvas_full_20261016.json`.
The
constructor still lacks an independent behavioral fixture. The float
`update` and `setWidth` now have a bounded ARM-derived behavior check
in [the Trail semantics audit](TRAIL_UPDATE_WIDTH_SEMANTICS_ARM_2026-10-16.md),
but neither has an exact instruction match. Their remaining compiler
differences require a separate recovery pass.
