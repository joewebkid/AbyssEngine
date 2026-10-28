# GlowPPShader initialization, ABI, and GLSL recovery (2026-10-28)

## Confirmed Android ARM evidence

Reference: `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The long `GlowPPShader::RenderEffect` at `0x8d120` reads
`AbyssEngine::firstRenderGlow`, clears it, and calls `InternalInit`
before accessing the four render targets. The original byte is `1`
at virtual address `0x215140` (file offset `0x214140`). The prior
source dereferenced a null pointer instead. The mode switch reads
`Engine::switchGlow`, whose initial value is `3` at virtual address
`0x2150d4` (file offset `0x2140d4`); the prior source read a second
null pointer. The original uses `-0.05f` (`0xbd4ccccd`) for
`projMatrix[10]`, whereas the prior source used `-1.0f`.

Original ARM field accesses place the copy, horizontal blur, vertical
blur, and black targets at offsets `0x3c`, `0x58`, `0x74`, and `0xa0`.
The two mesh attributes are at `0x90` and `0x98`. The source class
previously placed these members too early. The layout now includes
reserved words at the observed gaps. The original `InternalInit` names
are `GlowPPShader fboMasked`, `GlowPPShader fboBlurH`,
`GlowPPShader fboBlurV`, and `GlowPPShader fboBlack`.

`Init` at `0x8cf20` loads one embedded vertex shader and four fragment
shaders. The recovered strings match NUL-terminated bytes in the
original library at file offsets `0x1f1610` (217 bytes), `0x1f16ea`
(213), `0x1f17c0` (682), `0x1f1a6b` (682), and `0x1f1d16` (276).
The original attribute and uniform names are `a_position`,
`a_texCoord`, `u_WorldMatrix`, `s_texture_base`, `texSize`, and
`s_texture_glow`. The prior source passed placeholder file names and
unrelated variable names. In the mode switch, modes 0, 1, and 2 use
the source, copy, and vertical-blur textures respectively with the
black texture; mode 3 uses the source with vertical blur. The original
`UpdateMeshData` clears `dirty` only when it is set.

## Validation and limits

ARMv7 compiles 209/209 translation units. Native and ARM64 static
builds pass. The ARM64 shared library links without undefined symbols
and exports all 53 JNI entry points. Its silent test APK is signed and
verifies under Android signature schemes v1, v2, and v3:
`D:\gof2-arm64-crypto\gof2-arm64-glow-debug-20261028.apk`
(SHA-256 `60df8bce966a647df1688c633eb18ca8e2d5a94665621a7edba90768f25daec0`).

Focused ARMv7 verification raises the long `RenderEffect` from 89.9%
to 95.8% source-shape and `UpdateMeshData` from 93.1% to 99.3%.
The eleven-function family rises from 97.64% to 99.55% unweighted,
or from 94.32% to 98.09% weighted by original instruction count.
`Init`, `InternalInit`, and `SetInActive` become linked-exact, bringing
the family from six to nine linked-exact functions. The long render
function remains nonexact and is not proven semantically complete.

The full ARMv7 corpus compares 4,323/4,524 original functions across
4,745 pairs, with 94.85% source-shape, 84.40% strict fuzzy,
2,619 linked-exact, 1,085 byte-exact, and zero skipped units. The
`(unit, symbol)` pair set is unchanged from the preceding full report.
`Init`, `InternalInit`, and `SetInActive` are the only new linked-exact
matches; neither exact-match set has a loss.

The target ARM64 phone is still absent from ADB. No live glow output
has been verified. Modes outside 0 through 3 are unverified. Next,
exercise a confirmed foreground scene on the target phone and inspect
the remaining long render-function differences against Android ARM.
