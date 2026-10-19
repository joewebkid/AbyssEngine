# PaintCanvas fog parameters (2026-10-19)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`AbyssEngine::PaintCanvas::FogSetParameter` at `0x7cf28`–`0x7d068`.
The ARM instructions at `0x7cf48`–`0x7cf5c` read
`Engine::enableShader`, not a local flag. Both color paths divide channel
values by 255. The fixed-function path (`0x7cfc0`–`0x7d050`) sends mode,
density, start and end to `glFogf`, then sends four normalized channels to
`glFogfv` in high-byte to low-byte order. The shader path
(`0x7cf5e`–`0x7cfba`) writes start/end to `Engine+0x3e8/+0x3ec` and copies
three normalized high-to-low channels into the vector at `Engine+0x3f0`.
The shader path does not use the low byte.

The previous source always used a local zero flag, divided colors by zero,
omitted the fourth fixed-function channel, put `fogStart` in that channel,
and consequently could never update shader fog state. The source now
follows the binary's branch and channel order. The production body passes
an extracted host smoke at `-O0` and `-O2` for both paths, including the
GL call sequence and shader state. Native `gof2`, 208/208 ARM TUs and the
40 verifier tests pass (one existing skip). Focused source-shape rises
65.2% to 92.0%, strict fuzzy 31.8% to 96.2%; it is not exact.
Focused report: `A:/Galaxy on Fire new version/_work/fog_parameter_focused_20261019.json`.
The full ARM corpus compares 4,323/4,524 original functions (201 absent),
with 93.87% mean source-shape and 83.05% mean strict fuzzy. It retains
2,528 linked-exact and 1,076 byte-exact pairs, with no losses against
`level_create_player_full_20261019.json`. Full report:
`A:/Galaxy on Fire new version/_work/fog_parameter_full_20261019.json`.

All 51 distinct skybox mesh/texture paths found in `BuildResourceList.cpp`
and `LowResourceTable.cpp` exist in the extracted Android OBB data. This
is a path/inventory check, not proof that the native host can load or draw
them. The full asset-backed `Level::init` → `createSpace` → `renderBG`
runtime gate remains open; in particular, GPU/context integration and
resource loader behavior still need direct execution.
