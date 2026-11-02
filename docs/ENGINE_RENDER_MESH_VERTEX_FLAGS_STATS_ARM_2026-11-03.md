# Engine mesh vertex flags and draw counters on Android ARM

The source of truth is the original Android ARMv7 `libgof2hdaa.so`
(SHA-256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
especially `Engine::RenderMesh` at `0x85f24`.

The recovered function tested mesh vertex-format bits with expressions
such as `((uint32_t)vertexFormat << 27) < 0`. An unsigned value can
never be less than zero, so these expressions disabled the original
texture-coordinate, normal, color and indexed-draw paths. The direct
bit tests now use `2`, `4`, `8` and `0x10`, respectively, as observed
in the original ARM control flow. The indexed shader path can again
call `glDrawElements` instead of unconditionally drawing arrays.

The original indexed shader path also increments triangle and draw
counters. The triangle increment is unsigned `indexCount / 3`. The
bucket flag at engine offset `0xfd` chooses the A or B counters. For
client-side indices the triangle counters are at `0x58` and `0x5c`;
for uploaded indices they are at `0x68` and `0x6c`. The paired draw
counters are at `0x64` and `0x60`. ARM32 compile-time offset checks
cover the newly identified client counters and both draw counters.
These shader updates do not depend on `statsEnabled` in the original.

Focused ARM verification compiled **209/209** units. `RenderMesh`
source-shape rose from **65.2% to 90.0%** and strict fuzzy from
**56.8% to 58.3%**. It remains nonexact. The Windows native and
ARM64 static builds pass.

The full ARM corpus (`_work/render_mesh_bit_tests_full_20261103.json`)
compiled **209/209** units and compared **4,323/4,524** originals in
**4,762** pairs: **96.23%** mean source-shape, **85.99%** strict fuzzy,
**2,741 linked-exact**, **1,096 byte-exact**, 201 missing and zero
skipped. Compared with `_work/render_mesh_dispatch_full_20261103.json`,
the comparison-pair set and both exact-match sets have zero gains and
zero losses. Mean source-shape increased by 0.01 percentage point.

This restores source-backed paths; visible rendering has not been
verified on a device. The user requested no further Redmi testing.
The fixed-function draw-counter path and any remaining rendering
differences need separate inspection.
