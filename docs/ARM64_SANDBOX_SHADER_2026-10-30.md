# ARM64 SandboxShader source and attribute recovery (2026-10-30)

The reference is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`SandboxShader::Init` at `0x91818` passes two embedded GLSL strings to
`ES2LoadProgram`. Its PC-relative literals resolve to ELF addresses
`0x1f9380` (vertex, 1,030 bytes) and `0x1f9787` (fragment, 799
bytes). Twelve following attribute/uniform queries give the names
now used in `SandboxShader.cpp`.

The ARM evidence also fixes a meaningful field mapping: the second
attribute query is `a_texCoord`, followed by `a_normal`, `a_tangent`
and `a_bitangent`. `UpdateMeshData` already routes mesh texture
coordinates, normals, tangents and binormals through those respective
fields, so the former query names would have assigned the wrong
locations even with valid GLSL. `EmbeddedSandboxShader.h` records the
source offsets and per-string hashes. Both complete NUL-terminated
strings are present in the linked ARM64 library and signed APK.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles 209/209
translation units. Focused ARM comparison before and after gives six
SandboxShader pairs at **100% source-shape / 100% strict fuzzy**, all
six linked-exact and zero byte-exact. `Init` remains linked-exact. The
strict ARM64 shared library links with no undefined symbols and keeps
the same 53 JNI exports. The APK signature verifies with v1/v2/v3.

The fresh full ARMv7 corpus compares **4,323/4,524** original
functions across **4,745** pairs, with **94.86%** mean source-shape,
**84.43%** strict fuzzy, **2,638 linked-exact** and **1,086
byte-exact**. There are zero skipped units and 201 missing original
functions. The complete `(unit, symbol)` linked-exact and byte-exact
sets are identical to the preceding `cubenormal-full-20261030.json`
snapshot. The focused before/after and full reports are saved outside
Git in `D:\gof2-arm64-crypto`.

The previous same-emulator startup trace showed `SandboxShader.vsh`
passed literally to the shader compiler and failing. After installing
the updated APK in the same Android 15 ARM64-translation emulator,
the vertex and fragment sources had lengths 1,030 and 799 bytes;
shader compile failures fell **19 → 18** and successful program
links rose **37 → 38** in 18-second startup samples. The game process
stayed alive, but after dismissing the Google Play Services dialog,
the screen still showed only a small `BUY` fragment against black.
The APK, traces and screenshot remain outside Git in
`D:\gof2-arm64-crypto`.

Eighteen placeholder shader failures remain, starting with
`BumpShaderV2.vsh` in this trace. The menu output and repeated
texture-target GL errors still need independent diagnosis. Original
and rebuilt games use different Android emulators, so this test does
not establish visual equivalence.

The next priority is the texture-target path before changing more
shader sources. A previous native GL trace saw texture names **2**
and **5** bound first as `GL_TEXTURE_2D`, then as
`GL_TEXTURE_CUBE_MAP`; this is direct evidence of a conflicting target
use in the rebuilt runtime, but does not identify the incorrect
caller or resource. The current `Engine::SetTextures` is **70.1%**
ARM source-shape. Compare its original ARM branches and material flags
with a caller-level runtime trace before changing the target selection.
