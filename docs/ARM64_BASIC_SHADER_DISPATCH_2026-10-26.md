# ARM64 basic shader and mesh dispatch recovery (2026-10-26)

## Source evidence

The reference is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The ELF `.data` symbol `ShaderBaseStruct::shaderIndexIntern` is at virtual
address `0x215154` (file offset `0x214154`). Its initial four bytes are
`ff ff ff ff`, or **-1**. `ShaderBaseStruct` increments it during construction,
so the first registered shader has index zero and matches the first entry
in `Engine::shaders`.

The original `SimpleShader::Init` at `0x88fdc` passes embedded GLSL to
`ES2LoadProgram`, then queries `a_position`, `u_WorldMatrix`, and `glColor`.
The original `TextureVtxColorShader::Init` at `0x8bed0` passes one embedded
vertex program and two embedded fragment programs; its
`ConnectShaderComponents` at `0x8bdc8` confirms the attribute and uniform
names. The literal source bytes are copied from original file offsets
`0x1ea7f0`, `0x1ea898`, `0x1ef460`, `0x1ef95a`, and `0x1efb98` into
`EmbeddedBasicShaders.h`.

The original `Engine::RenderMesh` at `0x85f24` selects a shader from its
render-mode flags and mesh vertex format. The reconstructed ARM64 path had
always selected the first shader. The source-backed selection is now restored,
including the distinct texture, vertex-color, and no-texture cases and the
original priority of effect modes. This also makes the corrected zero-based
shader indices operational.

## Validation and limits

Native and AArch64 static builds pass; the AArch64 game library links all
53 exported JNI entry points with no undefined symbols. ARMv7 compiles
209/209 translation units. Focused ARM comparisons show no linked-exact or
byte-exact losses in `ShaderBaseStruct`, `SimpleShader`,
`TextureVtxColorShader`, or `Engine` compared with the prior full corpus.
All five newly recovered GLSL literals, and the three title texture shader
literals, pass standalone GLSL validation with the Android SDK's
`glslangValidator`. This checks shader syntax, not live GPU behavior.
The full corpus result is recorded in `PROJECT_STATUS.md`.

A debug-signed silent ARM64 APK with these changes was installed on the
25053RT47C. Its first title-to-menu transition crashed in
`PaintCanvas::FontCreate`: `ReleaseAllResources` left the ARM64 engine's
fallback-font index pointing into the cleared font array. Resetting the index
after releasing fonts removed the observed crash. The same game process
remained in the foreground activity after the transition. Device logs
repeatedly report shader compilation failures from remaining placeholder
shader sources. A dark screenshot with a small `BUY` fragment was captured,
but the foreground activity was not recorded at capture time; the device
later showed another game. That screenshot does not verify GOF2 rendering.
The menu's visual output remains unverified. Next, capture GOF2 with its
foreground activity confirmed, then inspect the active shaders and 2D
projection.

The full ARMv7 corpus remains at 4,323/4,524 original functions over 4,745
pairs, 94.79% source-shape, 84.31% strict fuzzy, 2,607 linked-exact and
1,083 byte-exact pairs, zero skipped units. Comparison by `(unit, symbol)`
found no exact-set losses against the pre-dispatch report.
