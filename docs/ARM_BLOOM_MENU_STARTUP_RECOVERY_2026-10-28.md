# Bloom GLSL and main-menu initialization (2026-10-28)

## Original evidence

The Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
passes one embedded vertex source and five embedded fragment sources to
`BloomShader::Init` at `0x895ec`. The six NUL-terminated strings were copied
byte for byte from file offsets `0x1eb170` (220 source bytes), `0x1eb24d`
(197), `0x1eb313` (621), `0x1eb581` (817), `0x1eb8b3` (820), and
`0x1ebbe8` (564). The fragment sampler is `s_texture_base`. The previous
code passed dotted filenames directly to `ES2LoadProgram`, which expects
GLSL source text. The machine-code comparison of `BloomShader::Init` was
already linked-exact because it cannot validate string contents behind
relocations.

The preceding signed APK later crashed with a null dereference in
`ModMainMenu::OnInitialize`. The Android ARM function at `0x175844` reads
`Globals::globals` and calls `Globals::startNewSoundResourceList` followed by
`Globals::addSoundResourceToList` for IDs `126, 21, 18, 19, 20`. The old
reconstruction used two unwritten holder variables. The original function
returns loading progress `100`, `80`, `60`, `30`, or `0` depending on its
stage; `ApplicationManager::OnUpdate` consumes that return value. Its first
stage passes a writable texture slot and `false` to `TextureCreate`, then
passes the resulting slot to `ChangeCubeTexture`. These behaviors have been
restored. The ARM64 path obtains the full canvas pointer from
`PaintCanvas::gCanvas`; the original 32-bit member remains in use on ARMv7.

An intermediate package then exposed a second null dereference while
`CutScene::initialize` called `Level::createSpace`: the sound registration for
resource `34` used unwritten `Globals::gGlobals`. Tombstone PC `0xeb054`
shows `Globals::addSoundResourceToList` receiving null `this`. The original
ARM call at `0xaeb62` resolves through GOT `0x210218` to
`Globals::globals`, so that one call now uses the canonical singleton.
The cutscene loop itself matches the original and was preserved.

## Verification

Native and ARM64 static libraries build, and ARMv7 builds all 209 translation
units. The focused Bloom family remains 11 comparable functions, 90.78%
source-shape, 81.97% strict fuzzy, seven linked-exact and no byte-exact
matches. The final main-menu family has 21 comparable functions, 88.37%
source-shape, 82.2% strict fuzzy, ten linked-exact and four byte-exact
matches. `ModMainMenu::OnInitialize` is 96.0% source-shape and 71.9% strict
fuzzy; it is not exact. The full ARM corpus compares **4,323/4,524**
originals across **4,745** pairs: **94.86% source-shape**, **84.43% strict
fuzzy**, **2,638 linked-exact**, **1,086 byte-exact**, and zero skipped
units. The pair set and both exact-match sets have no losses against the
preceding full report. The full report is
`D:\gof2-arm64-crypto\bloom_menu_level_full_20261028.json`.
`Level::createSpace` remains 86.1% source-shape and 46.4% strict fuzzy;
its one-call data reference change is not captured by the source-shape score.

The signed ARM64 test package
`D:\gof2-arm64-crypto\gof2-arm64-bloom-menu-level-debug-20261028.apk`
has SHA-256
`0f8aa0b4f06413ab30c69cf0cca2aea7769ff9793aa6f26be4009ea9a783998d`.
It contains the built shared library byte for byte, passes ZIP alignment,
and verifies under Android APK signatures v1, v2, and v3. The library retains
the previous Java JNI export set and `JNI_OnLoad`.

## Device observation and remaining work

The package installed on `25053RT47C`; GOF2 was confirmed as the foreground
activity. Adreno startup errors of the form `ERROR: 0:1: '.'` fell from 25
to 20, consistent with replacing the five Bloom filename placeholders. The
late screen remains almost black with a small `BUY` fragment. The foreground
capture `D:\gof2-arm64-crypto\bloom_menu_level_late_foreground_20261028.png`
has SHA-256 `956010635137e29ff8c9a0133532f602c8c3a0ab0ea06426fdc9b4a64b4dbd09`,
identical to the preceding package's late capture. The package then crashed
at 18:57:04 (PID 31017) in `AbyssEngine::String::Set(char const*)`, reached
through `AEFile::OpenRead`, `ImageCreateFromFile`,
`PaintCanvas::MaterialCreate`, `Level::init`, `CutScene::initialize`, and
`ModMainMenu::OnInitialize`. The fault address is `0xffffffff00000000`.
The bytes `{0,0,0xffff,0xffff}` at the start of a registered ID-0 material
produce this value when misread as a 64-bit path pointer. Several material
entries still refer to texture ID 0; the tombstone does not identify which
entry reached this call. Original ARM resource construction shows that at
least station materials 20004 and 20005 should refer to existing texture
IDs `33010/33011` and `33006/33007`, respectively. Those table entries and
the resource-kind handling are the next bounded recovery target.
This confirms that the earlier null sound call was passed, but the startup
sequence is still unstable. More startup shader
sources still need to be recovered. Other unwritten main-menu holders and
the remaining 64-bit pointer paths require separate source-backed review.
