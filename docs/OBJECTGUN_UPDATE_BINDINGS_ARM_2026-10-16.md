# ObjectGun update call bindings (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::update(int)` begins at `0x15f964`.

The PIC load at `0x15f974` resolves to GOT slot `0x2101f4`,
relocated as `Globals::Canvas`. The original calls
`PaintCanvas::TransformGetTransform(unsigned)` through PLT
`0x62088`, then `Transform::Update(long long,bool)` through
`0x5f7cc`. The same typed transform/update pair recurs for the
geometry transform at `0x15fafc-0x15fb08`.

For a gun with inactive delay, the indirect calls at
`0x15fa42-0x15fa7e` use GOT slots `0x210234` and
`0x210238`. Their ELF relocations name
`PaintCanvas::TransformGetTransform(unsigned)` and
`Transform::SetAnimationState(AnimationMode,void*)`.
The code re-reads `Globals::Canvas` and obtains the transform
three times, passing modes `0`, `3`, `1` in that order with
a null second argument. The non-turret camera branch at
`0x15fb4e-0x15fb5c` calls the typed `CameraGetCurrent()`
and `CameraGetLocal(unsigned)` PLT entries.

The C++ update path previously dereferenced a null canvas
placeholder, used declared-only free helpers, and called two null
function pointers in the inactive-delay branch. These sites now
use the typed methods. `AnimationMode` has an explicit `int`
underlying type so the observed numeric modes `0-3` remain
representable without changing the ARM call width. The
production-branch fixture checks the three calls, null arguments,
geometry transform index, and canvas replacement between calls
under host `-O0` and `-O2`.

Native `gof2`, **207/207** ARM translation units, the extracted
canvas smoke and **40** verifier tests (one existing skip) pass.
Focused `ObjectGun::update` source-shape/strict fuzzy improves
**83.8%/20.5% -> 87.8%/29.0%**; it is not exact. The full ARM
corpus compares **4,323/4,524** original functions, with **201
missing**, **124 extras**, zero skipped, **93.66%/82.75%**
average source-shape/strict fuzzy and **2,510 linked-exact /
1,074 byte-exact**. Exact `(unit,symbol)` sets against the
preceding full report have zero gains and zero losses. Reports:
`A:/Galaxy on Fire new version/_work/objectgun_ctor_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_update_bindings_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_update_bindings_enum_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_update_bindings_enum_full_20261016.json`.

Only the inactive-delay branch has a host behavior fixture.
The complete `update` math/explosion path, live ARM32
rendering, and full native game startup remain unverified.
The later
[render binding pass](OBJECTGUN_RENDER_BINDINGS_ARM_2026-10-16.md)
removed its null canvas and scale-flag placeholders.
