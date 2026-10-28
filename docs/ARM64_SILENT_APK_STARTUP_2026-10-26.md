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
A signed APK also includes the recovered glow shader layout,
GLSL, first-render setup, and texture selection:
`D:\gof2-arm64-crypto\gof2-arm64-glow-debug-20261028.apk`
(SHA-256 `60df8bce966a647df1688c633eb18ca8e2d5a94665621a7edba90768f25daec0`).
It has not yet been tested on the disconnected target phone. See
[glow evidence](ARM_GLOW_PP_SHADER_RECOVERY_2026-10-28.md).
A signed APK adds the recovered DrawFBO shader sources and
GLSL input names:
`D:\gof2-arm64-crypto\gof2-arm64-drawfbo-debug-20261028.apk`
(SHA-256 `70c6d1117a45703752d3a481b9efd6b00a50f97002d9c3f7c47a93fb16cdd86e`).
It has not yet been tested on the disconnected target phone. See
[DrawFBO evidence](ARM_DRAW_FBO_SHADER_GLSL_2026-10-28.md).
The current signed APK adds the original TexOnly shader sources and
sampler name:
`D:\gof2-arm64-crypto\gof2-arm64-texonly-debug-20261028.apk`
(SHA-256 `67dc890c341c0f8af29db9db1b9d1afc5fc86de2499f469dd3c27eb7aa8f60a5`).
It has not yet been tested on the disconnected target phone. See
[TexOnly evidence](ARM_TEX_ONLY_SHADER_GLSL_2026-10-28.md).

A signed APK adds the original `GenericShader` vertex/fragment sources
and their ARM-observed input names:
`D:\gof2-arm64-crypto\gof2-arm64-generic-debug-20261028.apk`
(SHA-256 `3f46f969838795ddbe0015cb9f4655a641dbfcfd19fdf7391ebf79008edcaed4`).
It has not yet been tested on the disconnected target phone. See
[GenericShader evidence](ARM_GENERIC_SHADER_GLSL_2026-10-28.md).

A signed APK adds the original `GenericShader1` GLSL, names and
attribute-slot mapping:
`D:\gof2-arm64-crypto\gof2-arm64-generic1-debug-20261028.apk`
(SHA-256 `1314686eefbb027cf5f6be3c3a6332305f9aa3f6f0a53964315f5af2c2b94ad6`).
It has not yet been tested on the disconnected target phone. See
[GenericShader1 evidence](ARM_GENERIC_SHADER1_GLSL_2026-10-28.md).

A signed APK adds the original `GenericShader2` GLSL, names and
attribute-slot mapping:
`D:\gof2-arm64-crypto\gof2-arm64-generic2-debug-20261028.apk`
(SHA-256 `7662b9c28fc9618cfbca7f463aa4c1e60082ba445deec0dc3050c4e46a06306d`).
It has not yet been tested on the disconnected target phone. See
[GenericShader2 evidence](ARM_GENERIC_SHADER2_GLSL_2026-10-28.md).

A signed APK adds the original `BumpMapping` GLSL/input names and
conditioned `dirty` reset:
`D:\gof2-arm64-crypto\gof2-arm64-bumpmapping-debug-20261028.apk`
(SHA-256 `dd6a2edf0f0455e8fe09035584aacca942f3b2dee2d3bc2e454d702a0d1fd70e`).
It has not yet been tested on the disconnected target phone. See
[BumpMapping evidence](ARM_BUMP_MAPPING_GLSL_2026-10-28.md).

A signed APK adds the original `DNSShader` file-load/fallback path,
GLSL, input names and engine LOD biases:
`D:\gof2-arm64-crypto\gof2-arm64-dns-aligned-debug-20261028.apk`
(SHA-256 `6bd1b953779f9dde90aac3b10fbe9e1393bfaed2cd0cddcfeb102a6a8d12fcce`).
It has not yet been tested on the disconnected target phone. See
[DNSShader evidence](ARM_DNS_SHADER_RECOVERY_2026-10-28.md).

A signed APK uses the original `TextureVtxColorShader` normal and fog
GLSL on both Android ABIs and the ARM-observed value source for
`u_eyeposmodel`:
`D:\gof2-arm64-crypto\gof2-arm64-texture-vtx-debug-20261028.apk`
(SHA-256 `05d354e94061dc8c45da818dfb8632d30763bfad780435a7afae6d0bdabb0f23`).
It has not yet been tested on the disconnected target phone. See
[TextureVtxColorShader evidence](ARM_TEXTURE_VTX_COLOR_GLSL_2026-10-28.md).

A signed APK corrects the shared shader dirty-byte layout and uses
the original `SimpleShader` GLSL and uniform name on all builds:
`D:\gof2-arm64-crypto\gof2-arm64-simple-abi-debug-20261028.apk`
(SHA-256 `8526f1d0f532a611fec1367822d90d6e1d2afc9fb3da1c9717362a1562963106`).
It was later installed on `25053RT47C`. GOF2 was confirmed in the
foreground during capture, but the screen was almost black with
a small `BUY` fragment and 25 Adreno GLSL
syntax errors. See
[shader ABI evidence](ARM_SHADER_DIRTY_BYTE_ABI_2026-10-28.md).

A signed APK restores the two no-texture shader source pairs for
all builds and the original 2D line pointer/index path:
`D:\gof2-arm64-crypto\gof2-arm64-notex-line-debug-20261028.apk`
(SHA-256 `863298557bdbc40403321dcb0bbb5db7d49eac2678f5e3b9a9ce0d72ea7a0321`).
It was installed on `25053RT47C`; GOF2 was confirmed in the
foreground during capture, but its late screen remains almost
black with a small `BUY` fragment and 25 Adreno shader syntax
errors. The process later crashed on a null menu initialization
holder. See [no-texture and 2D line evidence](ARM_NOTEX_2D_LINE_RECOVERY_2026-10-28.md).

A later signed APK restores five Bloom fragment programs, the main menu's
loading return contract and sound singleton, and a source-backed singleton
call in `Level::createSpace`:
`D:\gof2-arm64-crypto\gof2-arm64-bloom-menu-level-debug-20261028.apk`
(SHA-256 `0f8aa0b4f06413ab30c69cf0cca2aea7769ff9793aa6f26be4009ea9a783998d`).
It installed on `25053RT47C`; GOF2 was observed in the foreground, but its
late screen remains almost black. Startup dotted GLSL syntax errors fell
from 25 to 20. The game then crashed while opening an image file from the
cutscene; a material ID 0 is likely being misread as a texture path. Startup
remains unstable.
See [Bloom/menu startup evidence](ARM_BLOOM_MENU_STARTUP_RECOVERY_2026-10-28.md).

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
