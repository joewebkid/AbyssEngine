# ARM64 CubeMapping shader recovery and emulator check (2026-10-30)

The source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`CubeMapping::Init` at `0x907d8` passes embedded GLSL to
`ES2LoadProgram`. Its two PC-relative literals resolve to ELF addresses
`0x1f6ae0` (vertex, 1,777 bytes) and `0x1f71d2` (fragment, 1,200
bytes). The same function queries the attribute and uniform names now
used in `CubeMapping.cpp`. `EmbeddedCubeMappingShader.h` contains the
exact NUL-terminated source bytes, with per-string hashes and offsets.

The original APK and OBB ran in the Android 9 x86 emulator with ARMv7
translation, reaching the menu and opening scene. The rebuilt ARM64
APK ran in a separate Android 15 x86_64 emulator with ARM64 translation
and 7 GiB of free app storage. The latter emulator served as the
same-environment before/after test for this patch. Native-realm Frida
hooks on `libGLESv2_enc.so` observed 21 shader compile failures before
the patch, including `CubeMapping.vsh` passed as literal source. After
the patch, the CubeMapping vertex and fragment sources had lengths
1,777 and 1,200, compile failures fell to 20, and successful program
links rose from 35 to 36 in the 18-second startup sample. The traces,
APK and screenshot remain outside Git in `D:\gof2-arm64-crypto`.

The signed APK's AArch64 game library linked with `--no-undefined`,
`--gc-sections`, a JNI export map and the pinned libzip/crypto archives.
All 53 JNI exports matched the preceding APK. Native `gof2` and ARM64
static builds pass; ARMv7 compiles 209/209 translation units. Focused
ARM comparison reports six CubeMapping pairs at 99.5% mean source-shape,
99.3% strict fuzzy, five linked-exact and zero byte-exact. `Init`
remains linked-exact.

The fresh full ARMv7 corpus compares **4,323/4,524** original
functions across **4,745** pairs, with **94.86%** mean source-shape,
**84.43%** strict fuzzy, **2,638 linked-exact** and **1,086
byte-exact**. There are zero skipped units and 201 missing original
functions. The complete `(unit, symbol)` linked-exact and byte-exact
sets are identical to the pre-patch `ship17-19-full-20261030.json`
snapshot, so this change loses no exact matches. The focused and full
reports are saved outside Git in `D:\gof2-arm64-crypto`.

The rebuilt game still renders mostly black in this emulator, with a
small `BUY` fragment and a Google Play Services error dialog in the
captured run. The patch does not establish correct menu or gameplay
rendering. The remaining 20 placeholder shader failures, repeated GL
texture-target errors, and Google Play interaction need separate
diagnosis. The original ARM game did not exhibit this black frame in
its Android 9 emulator, but the different emulator API and ABI do not
support a direct pixel comparison.
