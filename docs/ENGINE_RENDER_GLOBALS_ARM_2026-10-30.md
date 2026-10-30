# Engine rendering globals: ARM storage and byte flags (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `Engine::SetTextures` (`0x84c44`–`0x84d66`), the original loads the
texture environment, shader-mode flag, and dirty flag through GOT-backed
addresses. The flags are read and written as bytes (`ldrb`/`strb` at
`0x84ca6`–`0x84cb6`; shader-mode loads also occur at `0x84cf0` and
`0x84d40`). `Engine::InitGL` receives the shader-mode setting as `bool`.

The reconstruction had declared these values inside an anonymous namespace
as `int`, `float`, and `int`. Because no code in the translation unit sets
the dirty flag, the compiler removed the original dirty-flag branch. The
recovered declarations use external linkage, with `bool` for the two flags
and `float` for the environment. This restores the observed byte accesses
and preserves a state change by another translation unit. It also improves
the code generated for other Engine methods without changing their source
control flow.

Focused `Engine::SetTextures` comparison improves from **86.3% to 95.3%**
source-shape and **51.5% to 71.6%** strict fuzzy. It remains nonexact:
the compiler still retains different local pointers and registers across
GL calls and selects the second texture target with a different instruction
sequence. These differences have not been treated as a reason to alter GL
behavior. In the 96-function Engine unit comparison, linked-exact rises
from 40 to 43, byte-exact remains 21, and neither exact-symbol set loses
a member. ARMv7 compiles 209/209 translation units; native `gof2` and
ARM64 static libraries build.

The full ARM corpus compares **4,323/4,524** original functions across
**4,745** pairs: **94.89%** mean source-shape, **84.53%** strict fuzzy,
**2,659 linked-exact**, **1,087 byte-exact**, and zero skipped. Compared
with the preceding full report, the complete `(unit, symbol)` exact sets
have zero losses and three new linked-exact Engine functions:
`GlowEndGlow`, `LightSetRimColor`, and `LightSetMaterialColorShininess`.

The verifier's `missing.txt` contains 201 original symbols. None has an
AbyssEngine or GOF2 game symbol name; the names belong to zlib, libzip,
OpenSSL, C++ runtime, and runtime helper groups. The 201 count is therefore
not a count of missing game functions. It still belongs in the corpus
accounting and is not excluded from reported totals.

Live GL behavior and texture-state equivalence on a phone remain unverified.
The focused reports and full-corpus report are stored outside Git in
`D:\gof2-arm64-crypto`.
