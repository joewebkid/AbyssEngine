# No-texture shader and 2D line recovery (2026-10-28)

## Android ARM evidence and correction

Reference: Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`NoTexShader::Init` at `0x90654` and
`NoTexVtxColorShader::Init` at `0x8dfb8` pass embedded GLSL to
`ES2LoadProgram` and query the original attribute/uniform names.
The four source strings in `EmbeddedNoTexShaders.h` were verified
byte for byte, including terminating NULs, against file offsets
`0x1f6900` (209 bytes), `0x1f69d2` (227 bytes), `0x1f33d0`
(327 bytes), and `0x1f3518` (308 bytes). The prior ARMv7/native
paths passed file names as GLSL and queried placeholder names.
Both shader classes now use the confirmed strings and names on all
builds.

The original `Engine::DrawLine2D` at `0x85e2c` writes its
`float *verts` argument to the 32-bit object slot at `+0x348`,
activates `NoTexShader::ShaderIndex` with a null mesh, and draws
`count` vertices as lines or a line strip. The previous source
wrote `count` to `+0x348` and selected an uninitialized static
shader index. Both no-texture shader update routines read the slot
when their mesh argument is null. The source now stores and reads
the vertex pointer. A 64-bit-only sidecar at the end of `Engine`
keeps the full pointer on ARM64/native while preserving the
original 32-bit field positions and later named members.

## Verification and limits

ARMv7 compiles 209/209 translation units. Focused
`DrawLine2D` improves from **84.9% to 96.0% source-shape**
and **44.4% to 70.0% strict fuzzy**. The six-function
`NoTexShader` and `NoTexVtxColorShader` families retain
their existing five linked-exact functions each, with no exact
losses. Their GLSL text is data behind relocations and is not
validated by the linked-exact code metric.

The full ARMv7 corpus compares **4,323/4,524** original functions
over 4,745 pairs, with **94.86% source-shape / 84.43% strict
fuzzy**, **2,638 linked-exact / 1,086 byte-exact**, and zero
skipped units. The pair set and both exact-match sets are identical
to the preceding full report. Native and ARM64 static libraries
build. The ARM64 shared library links without unresolved symbols
and exports 52 Java entry points plus `JNI_OnLoad`. The signed
silent APK embeds that exact library, passes ZIP alignment and
verifies under Android signature schemes v1, v2 and v3:
`D:\gof2-arm64-crypto\gof2-arm64-notex-line-debug-20261028.apk`
(SHA-256 `863298557bdbc40403321dcb0bbb5db7d49eac2678f5e3b9a9ce0d72ea7a0321`).

## Device observation

On `25053RT47C`, the APK installs successfully. The original
964 MiB OBB remains present. The game process was alive with
`GOF2HD2012` confirmed as the top resumed activity during the
captures. An initial
capture showed part of the Fishlabs logo, but a later capture
`D:\gof2-arm64-crypto\notex_line_foreground_late_20261028.png`
is almost black with a small `BUY` fragment. Its SHA-256
`956010635137e29ff8c9a0133532f602c8c3a0ab0ea06426fdc9b4a64b4dbd09`
matches the preceding APK's late foreground screenshot exactly.
The app log still contains 25 Adreno GLSL syntax errors of the
form `ERROR: 0:1: '.'`. These observations do not establish
that the recovered 2D line path executes on the title screen or
that the menu is usable. At 18:11:50 the same process later crashes
with `SIGSEGV` in `ModMainMenu::OnInitialize` at
`src/game/menu/ModMainMenu.cpp:223`: its null
`g_ModMainMenu_initSoundRes` holder is dereferenced. This is
separate from the observed shader failures and rules out sustained
stability. The next priority is recovering that menu initialization
and the shader sources failing at startup.
