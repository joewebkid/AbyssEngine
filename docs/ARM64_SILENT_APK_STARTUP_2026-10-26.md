# ARM64 silent APK startup (2026-10-26)

## Result and limits

The AArch64 static library builds and a shared game library links with no
undefined symbols while retaining all 53 exported JNI entry points, including
`JNI_OnLoad`. A debug-signed test APK packages that library and the original
APK's Java/assets. It uses local, no-output FMOD compatibility libraries and
a minimal Crashlytics loader compatibility library. These test libraries and
the APK are kept outside this repository. Sound is deliberately absent.
The latest clean APK is
`D:\gof2-arm64-crypto\gof2-arm64-silent-debug-20261026.apk`
(SHA-256 `97bb71e32a6322ad95300f2f5ad65119cfb33fbd76086ccb2382306080a9d53d`).

The APK installed on a 25053RT47C (Android 16, arm64-v8a). The original OBB
was copied to the package's OBB directory. The game reached `MTitle`, found
3,250 resources, created both logo images, and executed its render callback
for multiple frames without a native crash. The observed screenshot was black.
This is not yet a visually successful game startup. The other connected phone
with the original game was not modified.

After this observation, the title render path was traced to invalid OpenGL
operations in image drawing. The original Android ARM `TextureShader::Init`
and `ConnectShaderComponents` were checked against the original binary:
the shader inputs are embedded GLSL, and the actual attribute/uniform names
are `a_position`, `a_texCoord`, `u_WorldMatrix`, `glColor`, and `s_texture`.
The prior reconstruction passed placeholder file names as GLSL and queried
placeholder uniform names. The AArch64 path now uses the original shader
literals and names. `Engine::ShaderSetActive` also publishes the draw flag
observed in the original routine. This revised APK is built and signed, but
its image output remains **unverified on a device** because the ARM64 phone
disconnected before installation. The installed ARM64 emulator image cannot
run on this x86-64 host (the Android emulator rejects the CPU architecture).

## Source and validation

- Original ARM library SHA-256:
  `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
- Embedded `TextureShader` GLSL is copied byte for byte from file offsets
  `0x1f8140` (vertex, 1,155 bytes), `0x1f85c4` (fragment, 439 bytes), and
  `0x1f877c` (fog fragment, 603 bytes). The original function's IDA analysis
  confirms this vertex and these two fragment strings are passed to
  `ES2LoadProgram`.
- The AArch64 game library is ELF64/AArch64 and the test APK verifies under
  Android APK signature schemes v1, v2, and v3.
- Native static build succeeds. ARMv7 builds 209/209 translation units.
  The full ARM corpus compares 4,323/4,524 original functions across 4,745
  pairs, with 94.79% source-shape and 84.31% strict fuzzy averages, 2,607
  linked-exact and 1,083 byte-exact pairs, and zero skipped units. The exact
  sets have no losses against the pre-port baseline.

## Remaining work

Install the revised APK on an ARM64 device, verify that the title and main
menu render, and diagnose any remaining GL error or runtime failure. The
remaining shaders still have incomplete recovered source and may block later
scenes. The silent FMOD layer is temporary; audio requires a separately
validated ARM64 implementation.
