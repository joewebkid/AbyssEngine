# BumpShaderV2/V3/V4 original GLSL and ARM ABI (2026-10-30)

The primary source is Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The `Init` routines at `0x8d744` (V2), `0x8c1a8` (V3), and `0x8b29c`
(V4) pass embedded GLSL to `ES2LoadProgram`. Their PC-relative
literal targets and the exact NUL-terminated source bytes are:

| Variant | Vertex ELF address, bytes, SHA-256 | Fragment ELF address, bytes, SHA-256 |
| --- | --- | --- |
| V2 | `0x1f1e50`, 1,071, `94ac153a52c402263fe4df930dfe8a887d30b76db5dfab51fa4c197deb68cafe` | `0x1f2280`, 886, `2521ea985025093ed032f446d6823f5d065692fa5763653e1e4704056b814498` |
| V3 | `0x1efe80`, 1,346, `78f777e73827bdfeb4a8f44e89136d8c35b57510a8cc66dbd26e14aa7a8244c8` | `0x1f03c3`, 2,000, `1c2503175d9b422d7c2f768e90105ae1ce611ecdb5f8db0647e54829937a20ee` |
| V4 | `0x1ed820`, 1,470, `213f5e4e7d1097767f7278ae7b77a3fd3a3b46953442a3da52216c2042daf852` | `0x1edddf`, 1,098, `bf88eb014508165716cb387d9e881b501cec56f5328f951c8b17012a9bf247d6` |

All six C++ string arrays in `EmbeddedBumpShaderVariants.h` were
compared byte-for-byte with those original ELF ranges. Each `Init`
now queries the names recovered from its ARM PC-relative literals:
five attributes and ten uniforms for V2/V4, five attributes and
twenty uniforms for V3. V2 stores the `a_texCoord` location in the
field currently named `aNormal` and `a_normal` in `aTexCoord`, matching
the original storage order and the existing mesh-data routing. V3's
old field labels similarly do not consistently describe the original
uniform names; the restored queries follow the ARM call and store order.

The original V2 `UpdateMeshData` reads its first uniform location at
`this+0x38`, while our previous ARM object read `this+0x34`. Original
`Init` also stores the first uniform at `+0x38`. A 4-byte unused slot
after the five attributes restores this class layout; the rebuilt
`UpdateMeshData` now reads `+0x38` and both `Init` and `UpdateMeshData`
compare linked-exact. This fixes an ABI offset, not merely a spelling
of a shader input.

V3 previously dereferenced two zero-initialized global pointers when
its texture-bias uniforms were active. Original `UpdateMeshData` loads
GOT slots `0x210028` and `0x21002c`, whose ELF relocations name
`Engine::lodBiasDiffuse` and `Engine::lodBiasNormal`. The reconstructed
function now reads those existing static floats. Focused V3
`UpdateMeshData` rises from **98.3% to 99.8% source-shape** and from
**96.7% to 98.6% strict fuzzy**. The remaining instruction difference
is an equivalent conditional VFP selection, not evidence for a
different bias value.

Native `gof2`, ARM64 static, and all 209 ARMv7 translation units build.
Focused ARM comparisons show V2 **6/6 linked-exact**, up from 4/6;
V3 **5/6**, up from 4/6, with `Init` moving from **91.1% to 100%**
source-shape; and V4 retaining **6/6**. The strict ARM64 shared library
links without undefined symbols, retains the preceding 53 JNI exports,
and a signed parallel-install APK is saved outside Git at
`D:\gof2-arm64-crypto\gof2-arm64-bumpvariants-final-poco-clone-debug-20261030.apk`.
Play Protect blocked the preceding test APK on Poco X3; this package
has not been installed or run. Shader compilation, draw output, and
the broader gameplay path remain unverified on device.

The fresh full ARM corpus compares **4,323/4,524** original functions
across **4,745** pairs, with **94.86%** mean source-shape, **84.44%**
strict fuzzy, **2,641 linked-exact**, and **1,086 byte-exact**. It has
zero skipped units and 201 missing original functions. Against the
preceding `vertex-color-full-20261030.json` snapshot, complete
`(unit, symbol)` sets show zero linked-exact or byte-exact losses.
The three linked-exact gains are V2 `Init`, V2 `UpdateMeshData`, and
V3 `Init`; the byte-exact set is identical. The focused and full JSON
reports are saved outside Git in `D:\gof2-arm64-crypto`. The aggregate
source-shape rounds to the previous 94.86%; strict fuzzy increases
from 84.43% to 84.44%.
