# TextureLightShader platform source and ARM shape (2026-10-30)

Primary source: original Android ARM libgof2hdaa.so, SHA-256
6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58.
TextureLightShader::Init is at ELF 0x8db68 and UpdateMeshData at
0x8dd14. The engine registers this shader and selects it for a
textured lighting path. The previous Android recovery is documented
in ARM64_TEXTURE_LIGHT_SHADER_2026-10-26.md.

The existing embedded vertex and fragment strings at ELF 0x1f2620
(2,786 bytes) and 0x1f3103 (666 bytes) were rechecked byte-for-byte
against the source header. The former Windows branch supplied
TextureLightShader.vsh / .fsh to ES2LoadProgram, which treats its
arguments as GLSL source text, then queried placeholder names such
as u0 and a0. This branch also queried a_normal as a uniform. The
confirmed Android source and attribute/uniform queries now apply to
all platforms.

The original ARM UpdateMeshData branches on engine field +0x32c:
when it is at least two, it uploads the second light's ambient,
diffuse, and specular vectors; otherwise it uploads zero vectors.
The source now expresses this branch in the original order. The
values and branch condition are unchanged.

Native gof2 and ARM64 static builds pass; ARMv7 compiles 209/209
translation units. Focused ARM verification remains **5/6
linked-exact**, while UpdateMeshData rises from **98.8% to 99.6%
source-shape** and from **91.5% to 98.7% strict fuzzy**. The family
averages **99.9% source-shape / 99.8% strict fuzzy**. The sole
remaining instruction difference is that the original uses three
separate loads for the second light direction and this compiler
uses one grouped load. The update function remains nonexact.
Focused reports:
D:\gof2-arm64-crypto\texture-light-platform-before-20261030.json
and D:\gof2-arm64-crypto\texture-light-platform-after-20261030.json.

The full ARM corpus compares **4,323/4,524** originals across
**4,745** pairs: **94.88%** mean source-shape, **84.49%** strict
fuzzy, **2,656 linked-exact**, **1,087 byte-exact**, zero skipped,
and **201 missing** (all absent). Both exact-symbol sets are
unchanged from
D:\gof2-arm64-crypto\texture-shader-platform-full-20261030.json.
The family improvement is below the full-corpus rounding precision.
Full report:
D:\gof2-arm64-crypto\texture-light-platform-full-20261030.json.

The ARM64 game shared library/APK has not been rebuilt with this
packet. GPU compilation and lighting behavior on Poco remain
unverified until the phone is available again.
Next, inspect the BlurShader platform fallback and short RenderEffect
dispatch before a permitted live lighting test.
