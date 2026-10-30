# ARM64 CubeNormalMapping shader recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`CubeNormalMapping::Init` at `0x90b4c` passes two embedded strings to
`ES2LoadProgram`. Its PC-relative literals resolve to ELF addresses
`0x1f76b0` (vertex, 1,393 bytes) and `0x1f7c22` (fragment, 1,255
bytes). The following attribute and uniform queries identify five
attribute names and thirteen uniform names. The recovered strings in
`EmbeddedCubeNormalMappingShader.h` include offsets and SHA-256 hashes;
both complete NUL-terminated strings are present in the ARM64 library
and in the signed APK.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles 209/209
translation units. Focused ARM comparison gives all six
CubeNormalMapping functions **100% source-shape / 100% strict fuzzy**,
with six linked-exact and zero byte-exact pairs. `Init` remains
linked-exact. The strict ARM64 shared library links with no undefined
symbols and retains exactly the preceding 53 JNI exports; the APK
signature verifies with v1/v2/v3.

The fresh full ARMv7 corpus compares **4,323/4,524** original
functions across **4,745** pairs, with **94.86%** mean source-shape,
**84.43%** strict fuzzy, **2,638 linked-exact** and **1,086
byte-exact**. There are zero skipped units and 201 missing original
functions. The complete `(unit, symbol)` linked-exact and byte-exact
sets are identical to the preceding `cubemapping-full-20261030.json`
snapshot. Focused and full reports are saved outside Git in
`D:\gof2-arm64-crypto`.

The original game and rebuilt game run in separate Android emulators
with different Android versions and CPU translation. Source-shape and
successful linking alone do not establish correct menu or gameplay
rendering. In the same Android 15 ARM64-translation emulator, native
Frida hooks previously observed `CubeNormalMapping.vsh` passed as
shader source and failing compilation. With the updated APK, the
sources are 1,393 and 1,255 bytes; shader compile failures fell
**20 → 19** and successful program links rose **36 → 37** in
18-second startup samples. The game process stayed alive, but after
dismissing the Google Play Services dialog, the screen still showed
only a small `BUY` fragment against black. The traces, APK and
screenshots remain outside Git in `D:\gof2-arm64-crypto`.

Nineteen placeholder shader failures remain, beginning with
`SandboxShader.vsh` in this startup trace. Repeated texture-target GL
errors and the incomplete visual output remain separate priorities;
the present patch does not verify their behavior.
