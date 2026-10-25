# Postprocess shader Engine call targets (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`BloomShader`, `BlurShader`, `DrawFBOShader`, `GlowPPShader` and
`PostBWShader` contained 95 calls through eleven distinct undefined
`Engine_*` or `AbyssEngine_Engine_*` wrapper names. The original ARM
functions call `Engine::GetDisplayWidth`, `GetDisplayHeight`,
`SetWorldViewMatrix`, and `DrawQuad` directly. Representative call
sites are `DrawFBOShader::RenderEffect` at `0x885a8`, `0x885c2`,
`0x88616`, and `0x88716`; `PostBWShader::RenderEffect` at `0x8f07e`,
`0x8f098`, `0x8f0ec`, and `0x8f1ce`. Original ARM calls to the same
methods also appear in the Bloom, Blur, and GlowPP render effects.

The original `DrawFBOShader` additionally calls
`Engine::IsPostEffectActivated` at `0x88648`/`0x8873a`,
`ActivateRender2FracFBO` at `0x88650`, and
`DeactivateRender2FracFBO` at `0x88742`. The five recovered shader
files now call these existing methods directly. Local 16-word matrix
arrays are passed as a `const Matrix&` to match the original method
signature; the method reads the first 12 floats from that view.

Native `gof2`, AArch64 static `gof2`, and focused ARMv7 verification
for all five units build. The 50 focused comparisons retain their
**24 linked-exact** and **zero byte-exact** symbols with no losses.
Render-effect source-shape and strict fuzzy scores are unchanged in
focused verification because relocation targets are masked in this
metric. The strict AArch64 whole-archive inventory falls **543 → 532**
unique unresolved names, with no new names. A shared game library and
live postprocessing output remain unverified.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs with zero skips. Mean source-shape/strict
fuzzy remains **94.72%/84.21%**. The **2,603 linked-exact** and
**1,083 byte-exact** symbol sets have zero gains and zero losses.
Formatting cleanup after the full run changed no ARMv7 object bytes
in any of the five shader units; focused verification and native and
AArch64 static builds were repeated on the final sources.
