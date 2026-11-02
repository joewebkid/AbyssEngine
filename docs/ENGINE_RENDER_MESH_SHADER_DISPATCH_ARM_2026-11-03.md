# Engine mesh shader dispatch on Android ARM

Source-backed evidence is the original Android ARMv7
`libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Engine::RenderMesh` at `0x85f24` selects a shader from render-mode
flags and mesh vertex format. `Engine::ShaderSetActive` at `0x85e60`
falls back to `TextureShader` or `SimpleShader` for index `-1`, marks
the shader draw flag after finding a valid shader, and records the
active shader index when the program changes.

The reconstructed selector was present only in the ARM64 path. The
ARMv7 path passed a zero-initialized default index instead, and its
`ShaderSetActive` path did not set the draw flag. Therefore the ARMv7
shader path could skip its indexed or array draw entirely. The
source-backed selector, fallback and draw flag are now shared by
ARMv7 and ARM64. The priority of all render-mode branches and the
special mode that intentionally skips shader drawing are preserved.

Focused ARMv7 verification compiled **209/209** translation units.
`RenderMesh` changed from **15.9% to 65.2%** source-shape and
**7.1% to 56.8%** strict fuzzy. `ShaderSetActive` changed from
**85.3% to 98.5%** source-shape and **32.4% to 92.4%** strict fuzzy.
Neither function is exact. Windows native and ARM64 static builds pass.

The recovered `RenderMesh` still omits some original draw-statistics
updates. Their field layout needs separate verification. These builds
do not establish live rendering, and the user requested no further
Redmi testing.

The full ARM corpus (`_work/render_mesh_dispatch_full_20261103.json`)
compared **4,323/4,524** original functions in **4,762** pairs with
**209/209** units, **96.22%** mean source-shape, **85.99%** strict
fuzzy, **2,741 linked-exact**, **1,096 byte-exact**, 201 missing and
zero skipped. Compared with the prior full report
`_work/mtitle_end3d_full_20261102.json`, the complete comparison-pair
set and both exact sets have zero gains and zero losses. The mean
source-shape gain is 0.01 percentage point; the focused gain is much
larger because it concerns two formerly incomplete functions.
