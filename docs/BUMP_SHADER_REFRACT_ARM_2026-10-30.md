# BumpShaderRefract original GLSL and attribute ABI (2026-10-30)

Primary source: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `BumpShaderRefract::Init` at ELF `0x8f7ec` passes embedded
GLSL to `ES2LoadProgram`. Its PC-relative literals select the vertex
string at `0x1f4a10` (568 bytes, SHA-256
`3bccbc7706cd2fff627eb34127168a392835f3a41ba83f6b79aefb17994f3021`)
and fragment string at `0x1f4c49` (930 bytes, SHA-256
`265d1255b39e8f6ec759b256d129833b47fae4ca3efb244172f7fd99fe3ff776`).
Both reconstructed C++ arrays were decoded and compared byte-for-byte
with their original ELF ranges.

The original queries `a_position`, `a_texCoord`, and `a_VertexColor` with
`glGetAttribLocation`, storing the third result at `this+0x28`. The
previous source queried a generic `u0` with `glGetUniformLocation` at
that slot, while `SetInActive` and `UpdateMeshData` used it as an
attribute. The field is now named `aVertexColor` and its query matches
the original attribute API. Seven uniform query names and the late
`s_texture_base` query were recovered from the same `Init` literals;
their original storage offsets and sampler assignments are preserved.

Focused ARM verification: `Init` improves from **90.2% to 100%
source-shape** and from **90.5% to 100% strict fuzzy**, and becomes
linked-exact. `UpdateMeshData` remains **99.0% source-shape**, **95.7%
strict fuzzy**, and nonexact. The unit now has **5/6 linked-exact**
functions; this does not prove the whole refractive draw path.

Native `gof2` and ARM64 static builds pass, and ARMv7 compiles all
**209/209** translation units. The strict ARM64 shared library links
without undefined symbols and preserves the preceding **53 JNI
exports**. A signed, zip-aligned parallel-install APK contains that
library byte-for-byte at
`D:\gof2-arm64-crypto\gof2-arm64-bumprefract-poco-clone-debug-20261030.apk`.
Its signature verifies. The APK has not been installed or run because
Play Protect blocked earlier clone installations on Poco X3; runtime
shader compilation, rendering, and gameplay remain unverified.

Focused report:
`D:\gof2-arm64-crypto\bump-refract-focused-20261030.json`.
The fresh full ARM corpus compares **4,323/4,524** original functions
across **4,745** pairs, with **94.87%** mean source-shape, **84.45%**
strict fuzzy, **2,642 linked-exact**, **1,086 byte-exact**, zero skipped,
and 201 missing originals. Relative to
`bump-main-full-final-20261030.json`, complete `(unit, symbol)` sets
show one linked-exact gain (`BumpShaderRefract::Init`) and no linked-exact
or byte-exact loss. The new full report is
`D:\gof2-arm64-crypto\bump-refract-full-20261030.json`. The aggregate
source-shape and strict fuzzy means each rise by 0.01 percentage point.
