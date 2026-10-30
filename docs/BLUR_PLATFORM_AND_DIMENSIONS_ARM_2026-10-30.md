# BlurShader platform sources and screen dimensions (2026-10-30)

Primary reference: original Android ARM libgof2hdaa.so, SHA-256
6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58.
BlurShader::Init starts at ELF 0x8a68c; its long RenderEffect
overload starts at 0x8a7a8, and UpdateMeshData at 0x8aab8. The
short RenderEffect overload at 0x8a760 already dispatches through the
original virtual table slot, as recorded in
ARM_SHADER_RENDER_EFFECT_VTABLE_2026-10-28.md.

The existing embedded vertex and fragment sources at ELF 0x1ec470
(311 bytes) and 0x1ec5a8 (773 bytes) were rechecked byte-for-byte
against the source header. The Windows Init branch had supplied
BlurShader.vsh / .fsh to ES2LoadProgram, which compiles its arguments
as GLSL text, and queried names that do not occur in the original
shader. All platforms now use the confirmed embedded source and GL
query names. The Windows-only assignment of blurProgram to the
inherited program field is retained because that host path previously
published it; Android keeps its original derived program slot.

The long RenderEffect now clears all 16 projection entries on
Windows and uses the original Android Z entry, -0.05f, instead of
the old -1.0f. Its Android NEON zeroing path remains unchanged.
In UpdateMeshData, both orientation branches now convert display
dimensions through signed int before float. The original ARM uses
vcvt.f32.s32 for these values; the earlier source generated
vcvt.f32.u32.

Native gof2 and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. Focused BlurShader verification retains **6/10
linked-exact**. UpdateMeshData source-shape rises from **96.4% to
96.9%**, while strict fuzzy remains **88.0%**. The family averages
**99.2% source-shape / 93.5% strict fuzzy**. The long RenderEffect
remains **95.3% source-shape / 53.6% strict fuzzy** and is not a
verified semantic match; its full GL-state and matrix flow still
needs investigation. Focused reports:
D:\gof2-arm64-crypto\blur-platform-before-20261030.json and
D:\gof2-arm64-crypto\blur-platform-final-20261030.json.

The full ARM corpus compares **4,323/4,524** originals across
**4,745** pairs: **94.88%** mean source-shape, **84.49%** strict
fuzzy, **2,656 linked-exact**, **1,087 byte-exact**, zero skipped,
and **201 missing** (all absent). Both exact-symbol sets are
unchanged from
D:\gof2-arm64-crypto\texture-light-platform-full-20261030.json.
The local improvement is below full-corpus rounding precision.
Full report:
D:\gof2-arm64-crypto\blur-platform-full-20261030.json.

The ARM64 game shared library/APK has not been rebuilt with this
packet. GPU compilation, blur visual output, and the Windows host
runtime are unverified.
Next, investigate the long RenderEffect GL-state and matrix flow
against the original ARM code before a permitted live blur test.
