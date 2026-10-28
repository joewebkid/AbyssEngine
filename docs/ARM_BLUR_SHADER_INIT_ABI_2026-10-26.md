# Android BlurShader initialization and layout (2026-10-26)

Final verification: 2026-10-28.

## Original evidence and reconstruction

The reference is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BlurShader::Init` at `0x8a68c` passes embedded GLSL sources to
`ES2LoadProgram`, not shader filenames. The exact null-terminated source
bytes come from file offsets `0x1ec470` (311-byte vertex source) and
`0x1ec5a8` (773-byte fragment source). Both embedded strings were checked
byte-for-byte against the binary after generation.

The original queries `a_position`, `a_texCoord`, `u_WorldMatrix`,
`s_texture_base`, `u_texel_size`, `u_radial_origin`,
`u_blur_strength`, and `u_radial_bright` in that order. The reconstructed
Android path now uses these names and source strings. The PC path retains
its existing file names and queried names, and also publishes the compiled
program through the base-class field as before this change.

The original `Init` writes the GL program at object offset `0x24` and
the queried locations at `0x28` through `0x44`; the inherited program
field at `0x04` is a different slot. `SetInActive` at `0x8a748` reads
attribute fields at `0x48` and `0x50`. The constructor at `0x8a5e0`
writes the two strength floats at `0x58` and `0x5c`. The recovered
`BlurShader` layout reserves the intervening words and uses the derived
program field in its render path. `UpdateMeshData` at `0x8ab42` passes
`0.5f` and `0.3f` to the radial-origin uniform; the previous source
passed `0.5f` twice.

## Validation and remaining uncertainty

ARMv7 compiles 209/209 translation units. Native and ARM64 static builds
pass. The ARM64 shared game library links without undefined symbols, and
the rebuilt silent test APK has a valid signature. The target ARM64 phone
remains absent from ADB, so shader compilation and visual blur behavior
have not been observed on device.

Focused ARMv7 verification compares ten function pairs. The family rises
from 95.10% to 96.22% unweighted source-shape, and from 94.44% to
95.51% weighted by original instructions. `Init` and `SetInActive` are
newly linked-exact. `UpdateMeshData` rises from 93.7% to 96.0%
source-shape, but is not linked-exact. Both `RenderEffect` overloads
remain below 98% source-shape; in particular the short overload's
original ARM code dispatches through a vtable entry at offset `0x20`,
whereas the earlier source called the long overload directly. That ABI
relationship was subsequently restored; see the
[RenderEffect vtable evidence](ARM_SHADER_RENDER_EFFECT_VTABLE_2026-10-28.md).

The final ARMv7 object is byte-identical after the PC-only preservation
change. The full ARMv7 corpus compares 4,323/4,524 original functions
across 4,745 pairs, with 94.82% source-shape, 84.36% strict fuzzy,
2,612 linked-exact, 1,083 byte-exact, and zero skipped units. Compared
with the preceding full report, the pair set is identical, the only two
new linked-exact pairs are `BlurShader::Init` and `SetInActive`, and there
are no linked-exact or byte-exact losses. The `Engine` unit retains its
40 linked-exact functions despite the corrected `BlurShader` size.
