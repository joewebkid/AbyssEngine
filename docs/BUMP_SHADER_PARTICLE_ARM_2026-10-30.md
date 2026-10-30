# BumpShaderParticle ARM recovery (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`BumpShaderParticle::Init` starts at ELF `0x87dc0`; its two PC-relative
arguments to `ES2LoadProgram` resolve to embedded GLSL, not shader
filenames. The previous implementation passed placeholder filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1e86d0` | 1,223 | `b3111eac32e811247b3a993fbd7af5755e3b0e81a501c6f69645061dfc0807e9` |
| Fragment | `0x1e8b98` | 1,181 | `89dc30ae67ef2d4c5fc79861e53691b2cb6db643fef90c35655206c785bb6391` |

Both compiled C++ string contents were decoded and compared byte-for-byte
with those original ELF ranges. All 19 queried input names, including
their order, match the `Init` PC-relative literals: six attributes and
13 uniforms. The sixth attribute is `a_VertexColor`; the old source
incorrectly called `glGetUniformLocation` for that class slot. The slot
is used by `glEnableVertexAttribArray`, `glVertexAttribPointer`, and
`glDisableVertexAttribArray`, so this was a behavior error as well as
an ARM mismatch.

The original `UpdateMeshData` loads `Engine::lodBiasDiffuse` and
`Engine::lodBiasNormal` through GOT entries `0x210028` and `0x21002c`.
The previous implementation supplied two zero-initialized file-local
floats. The new values follow the confirmed ARM symbols.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. Focused ARM verification improves from **4/6** to
**6/6 linked-exact**, with **100% source-shape and strict fuzzy** for
the whole unit. `Init` and `UpdateMeshData` are the two new exact
matches. Their previous source-shape scores were **100.0%** and
**98.3%**; their previous strict fuzzy scores were **95.8%** and
**97.4%**. Focused report:
`D:\gof2-arm64-crypto\bump-particle-focused-20261030.json`.

The strict ARM64 shared library links without undefined symbols and
retains all **53 JNI exports**. Its signed, zip-aligned parallel-install
APK is
`D:\gof2-arm64-crypto\gof2-arm64-bumpparticle-poco-clone-debug-20261030.apk`;
the embedded library was checked byte-for-byte. The APK has not been
installed or run because Play Protect blocked earlier clone
installations on Poco X3. Shader compilation, rendering, and gameplay
on the phone remain unverified.

The full ARM corpus compares **4,323/4,524** original functions across
**4,745** pairs: **94.87%** mean source-shape, **84.45%** strict fuzzy,
**2,646 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Compared with
`D:\gof2-arm64-crypto\speccube-family-full-20261030.json`, the exact
`(unit, symbol)` sets gained precisely `Init` and `UpdateMeshData` in
this unit and lost no linked-exact or byte-exact members. Full report:
`D:\gof2-arm64-crypto\bump-particle-full-20261030.json`.
