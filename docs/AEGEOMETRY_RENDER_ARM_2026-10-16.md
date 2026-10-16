# AEGeometry render routing (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0xa4a50`, `AEGeometry::render()` reads the low visibility byte at
object offset `+0x48`; zero returns. Otherwise it loads the transform
index from `+0x0c` into `r1`, sets `r2 = 0`, loads `PaintCanvas*` from
`+0x2c` into `r0`, and tail-branches via veneer `0x19bd98`. The veneer
resolves to PLT `0x6306c`, named
`PaintCanvas::DrawTransform(unsigned int, const Matrix*)`. Thus the
call contract is `canvas->DrawTransform(transform, nullptr)`.

The previous body called the declared-only `_ae_geom_render` hook with
the same register arguments. Its ARM body was already 100% source-shape,
100% strict fuzzy and linked-exact because a relocation masks the branch
destination; those metrics did not prove the destination was correct.
The body now calls the existing typed canvas method, without narrowing
the canvas pointer to 32 bits on a native host. A production-body host
fixture checks visibility values `0`, `0x0100`, and `0x0101`, plus the
canvas method, transform index and null matrix argument. The expanded
fixture failed to compile against the old undefined hook and passes
with the recovered call.

The generic direct-call audit excludes `R_ARM_THM_JUMP24` tail branches,
so the original destination above was resolved manually from the veneer
and named PLT entry. Native `gof2`, 207/207 ARM translation units and 40
verifier tests (one existing skip) pass. Full corpus compares
4,323/4,524 original functions at 93.64%/82.72% average source-shape/
strict fuzzy, 2,508 linked-exact and 1,074 byte-exact. Against the
preceding report, exact `(unit,symbol)` sets have zero gains or losses.
Reports: `A:/Galaxy on Fire new version/_work/aegeometry_set_position_split_full_20261016.json`
and `A:/Galaxy on Fire new version/_work/aegeometry_render_full_20261016.json`.

This verifies routing, not live Android ARM32 rendering, camera culling,
or the complete `PaintCanvas::DrawTransform` implementation.
