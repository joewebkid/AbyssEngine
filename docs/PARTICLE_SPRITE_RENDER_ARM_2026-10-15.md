# ParticleSystemSprite camera-matrix render recovery (2026-10-15)

## Evidence and change

REA/IDA inspected Android 2.0.16 ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
static `ParticleSystemSprite::render(PaintCanvas*,uint32_t)` at `0x18347c`.
The function checks the sprite handle for `-1`, obtains the current camera
and its local 15-float matrix twice (`0x183490–0x1834e2`), copies both
matrix values and passes them to `DrawSpriteSystem` (`0x18356a`). The ARM
body has no matrix default-constructor calls or element-copy loop.

The prior C++ default-constructed two `Matrix` objects and then overwrote
all 15 elements via loops. The replacement copies each 60-byte matrix
directly from `CameraGetLocal`. `Matrix` is asserted to be 0x3c bytes;
other `PaintCanvas` code already treats the same camera storage as a
`Matrix`. This is a source-form repair with no intended value change. It
does not alter the separate four-argument render overload.

The neighboring `IParticleSystem::calcEmitterVelocity(int)` was screened
because its source-shape was 72.2%. REA/IDA at `0x181b90` confirms its
existing position difference, `1000.0f / delta` scale and state writes;
no behavior change was justified, so it was left untouched.

## Verification and limits

Focused ARM comparison improved this render overload from **71.0%**
source-shape and **29.8%** strict fuzzy (135 reconstructed vs 107 original
instructions) to **100%/100%** (107/107), gaining one linked-exact match;
it remains non-byte-exact. All **206/206** ARM translation units and the
native `gof2` build pass. Full-corpus metrics and exact-symbol regression
results: `_work/particle_render_full_20261015.json` compares
**4,323/4,524** original functions, with **201 missing**, **123 extras**
and zero skipped. Average source-shape improves **93.45% → 93.46%** and
strict fuzzy **82.35% → 82.36%**. The linked-exact set gains precisely
this render symbol (**2,485 → 2,486**) with zero losses; the byte-exact
set is unchanged at **1,065** with zero losses.

REA's IDA adapter does not prove complete body ranges or indirect-call
coverage. No original Android/iOS frame was executed for this change.
The camera-buffer ABI has since been audited: `Camera::projection` is a
constructed `Matrix` subobject at `+0x0c`; `CameraGetLocal` returns its
address and `CameraSetLocal` assigns to it. The `float[15]` in
`PCCameraView` is only a raw overlay, not the allocation's object type.
See [the camera ABI audit](PAINTCANVAS_CAMERA_CREATE_ARM_2026-10-15.md).
Controlled Android/iOS rendering remains unverified.
