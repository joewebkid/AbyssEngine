# ARM64 silent APK startup (2026-10-26)

## Result and limits

The AArch64 static library builds and a shared game library links with no
undefined symbols while retaining all 53 exported JNI entry points, including
`JNI_OnLoad`. A debug-signed test APK packages that library and the original
APK's Java/assets. It uses local, no-output FMOD compatibility libraries and
a minimal Crashlytics loader compatibility library. These test libraries and
the APK are kept outside this repository. Sound is deliberately absent.
The initial clean APK is
`D:\gof2-arm64-crypto\gof2-arm64-silent-debug-20261026.apk`
(SHA-256 `4194a1c56bbb80cbf2aef3069c2411ff6bf214b4f6aa984b0aae771ad811b74b`).
The same artifact path was rebuilt with the basic shader dispatch recovery;
It was rebuilt again with two no-texture shader sources; this latest image
has not yet been visually checked in the game foreground. See
[no-texture shader evidence](ARM64_NO_TEXTURE_SHADERS_2026-10-26.md).
The same path was then rebuilt with the recovered alpha-test texture shader;
that newest package is also not yet visually checked. See
[alpha-test shader evidence](ARM64_ALPHA_TEST_SHADER_2026-10-26.md).
It was rebuilt once more with the original illuminated texture shader;
that latest APK has not been installed on the disconnected ARM64 phone. See
[texture-light shader evidence](ARM64_TEXTURE_LIGHT_SHADER_2026-10-26.md).
It was rebuilt again after correcting the texture shaders' fog-controlled
program selection and a uniform's value source; the target phone has not
yet been available for a visual check. See
[texture fog dispatch evidence](ARM_TEXTURE_FOG_DISPATCH_2026-10-26.md).
It was rebuilt again with the original blur shader GLSL and corrected class
layout; this package has not been run on the disconnected ARM64 phone. See
[blur shader evidence](ARM_BLUR_SHADER_INIT_ABI_2026-10-26.md).
It was rebuilt again with the original virtual `RenderEffect` table order;
the new package is signed but has not been tested on the target phone. See
[render-effect vtable evidence](ARM_SHADER_RENDER_EFFECT_VTABLE_2026-10-28.md).
It was rebuilt again with the original blur projection initialization;
live image output for this package is not yet verified. See
[blur projection evidence](ARM_BLUR_PROJECTION_2026-10-28.md).
A signed APK includes the recovered bloom initialization,
projection coefficient, and mode selection:
`D:\gof2-arm64-crypto\gof2-arm64-bloom-debug-20261028.apk`
(SHA-256 `2f844c2d8e3a98a772b5158bac865997c338eb563a7bfd2200eb2b9f8ba4acd3`).
It has not yet been tested on the disconnected target phone. See
[bloom evidence](ARM_BLOOM_FIRST_RENDER_AND_MODE_2026-10-28.md).
The current signed APK also includes the recovered glow shader layout,
GLSL, first-render setup, and texture selection:
`D:\gof2-arm64-crypto\gof2-arm64-glow-debug-20261028.apk`
(SHA-256 `60df8bce966a647df1688c633eb18ca8e2d5a94665621a7edba90768f25daec0`).
It has not yet been tested on the disconnected target phone. See
[glow evidence](ARM_GLOW_PP_SHADER_RECOVERY_2026-10-28.md).

The APK installed on a 25053RT47C (Android 16, arm64-v8a). The original OBB
was copied to the package's OBB directory. The game reached `MTitle`, found
3,250 resources, created both logo images, and executed its render callback
for multiple frames without a native crash. A screenshot appeared black, but
the foreground activity was not recorded with it; visual startup remains
unverified. The other connected phone
with the original game was not modified.

After this observation, the title render path was traced to invalid OpenGL
operations in image drawing. The original Android ARM `TextureShader::Init`
and `ConnectShaderComponents` were checked against the original binary:
the shader inputs are embedded GLSL, and the actual attribute/uniform names
are `a_position`, `a_texCoord`, `u_WorldMatrix`, `glColor`, and `s_texture`.
The prior reconstruction passed placeholder file names as GLSL and queried
placeholder uniform names. The AArch64 path now uses the original shader
literals and names. `Engine::ShaderSetActive` also publishes the draw flag
observed in the original routine. This revised APK was installed after the
ARM64 phone reconnected. The first run crashed at
`PaintCanvas::FontCreate` on title release. Resetting the ARM64 fallback-font
index during resource cleanup removed that crash in a second run. The game
process then stayed alive in the foreground activity. A dark screenshot with
a small `BUY` fragment was captured without recording the foreground activity.
Because the device later showed another game, that image does not prove GOF2
rendering. The menu's visual output remains unverified. The installed ARM64
emulator image cannot run on this x86-64 host (the Android emulator rejects
the CPU architecture).

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

Capture GOF2 with its foreground activity confirmed, recover remaining
shader literals, and inspect 2D projection if the verified image is wrong.
Repeat the device test until the menu is legible and interactive, then test
later scenes. The silent FMOD layer is temporary; audio requires a separately
validated ARM64 implementation.
