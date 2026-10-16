# ObjectGun mesh replacement (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x160374`, `ObjectGun::replaceGun(unsigned,int)` reads GOT slot
`0x2101f4`; its ELF relocation names `Globals::Canvas`. It preserves
the address of this global across the first call and reads the canvas
pointer again for the second. It removes the old mesh using the lower
16 bits of the field at `+0x28`, stores the full incoming mesh ID at
that offset, then adds the lower 16 bits of the new ID with a false
flag. The first call resolves to
`PaintCanvas::TransformRemoveMesh(unsigned, unsigned short)`; the tail
branch via veneer `0x19bd38` resolves to PLT `0x6303c`,
`PaintCanvas::TransformAddMesh(unsigned, unsigned short, bool)`.

The previous C++ used an always-null file-static `g_PaintCanvas`
pointer and declared-only helper functions in this method. The
recovered body uses the typed methods through `&Globals::Canvas`,
preserving the original re-read between calls. Other ObjectGun methods
still use unresolved state and are outside this bounded package.

`python tools/run_objectgun_canvas_calls_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe` extracts the production body and runs it
under host `-O0` and `-O2`. It checks old/new 16-bit ABI truncation,
storage of the full incoming ID, transform index, false add flag,
and switching the global canvas both between calls and from inside
`TransformRemoveMesh`. The expanded fixture could not compile the old
body without its undeclared hooks and passes after the recovery.
Native `gof2`, 207/207 ARM units and 40 verifier tests (one existing
skip) pass.

Focused source-shape/strict fuzzy rises **83.0%/41.2% -> 100%/100%**;
the method becomes linked-exact, not byte-exact. The 16-function
ObjectGun unit rises **92.62%/73.59% -> 93.68%/77.27%** average
source-shape/strict fuzzy and **7 -> 8** linked-exact, with three
byte-exact unchanged. The full corpus compares **4,323/4,524**
original functions, with **201 missing**, **124 extras**, zero skipped,
**93.66%/82.74%** average source-shape/strict fuzzy, **2,510
linked-exact / 1,074 byte-exact**. Exact `(unit,symbol)` sets against
the preceding report show one linked gain and zero linked/byte losses.
Reports: `A:/Galaxy on Fire new version/_work/trail_canvas_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/objectgun_replace_full_20261016.json`.

This tests the replacement method's canvas contract, not construction
or the much larger ObjectGun update/render paths. Live ARM32 rendering,
resource ownership and external game initialization remain untested.
The constructor's canvas, iPad and mesh-ID-table bindings were
subsequently recovered in
[the constructor audit](OBJECTGUN_CONSTRUCTOR_BINDINGS_ARM_2026-10-16.md).
Later [update](OBJECTGUN_UPDATE_BINDINGS_ARM_2026-10-16.md)
and [render](OBJECTGUN_RENDER_BINDINGS_ARM_2026-10-16.md)
passes recovered their canvas bindings. Their full behavior
still needs validation.
