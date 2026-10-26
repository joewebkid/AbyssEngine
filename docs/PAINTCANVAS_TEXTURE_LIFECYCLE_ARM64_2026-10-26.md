# PaintCanvas texture suspend and resume (2026-10-26)

Primary source: Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
IDA shows `ChangeCubeTexture` at `0x7c288`, `Suspend` at `0x7e7c8`,
`Resume` at `0x7e828`, and `ReloadTextures` at `0x7e8e4`.

The ARM code checks byte `+0x14` of a loaded texture before cube binding:
that is `AELoadedTexture::isCube`. `Suspend` deletes each loaded GL id
and writes `-1`. `Resume` recreates every texture using its saved name
and scale, stores the returned id, and rebinds the selected cube map.
`ReloadTextures` recreates only entries whose id is `-1`. The prior
recovered code wrote zero instead of the created id in both creation
paths, used a detached stand-in for `CubeMapSetIndex`, and read the
texture's `String` and scale through ARM32-offset overlays on AArch64.

All four methods now use `AELoadedTexture` and the existing typed
`TextureCreateFromFile`, `String::GetAEChar`, and GLES functions. The
`Suspend` local array retains the original ARM call shape while using
the real `glDeleteTextures` symbol. A production-body 64-bit smoke test
passes at `-O0` and `-O2`: cube selection, deletion, repeated suspend,
new id storage, saved-name/scale use, selective reload, and cube rebind.
Native and AArch64 static builds pass; **209/209** ARMv7 translation
units compile.

Focused ARMv7 verification compares **198** `PaintCanvas` symbols.
`Resume` rises **71.7% → 92.8% source-shape**; `Suspend` reaches
**100% source-shape / 100% strict fuzzy** and adds one linked-exact
symbol. The focused mean changes **95.99% → 96.09% source-shape** and
**82.92% → 82.71% strict fuzzy**. `ReloadTextures` drops **94.9% →
92.1% source-shape** and **89.5% → 60.8% strict fuzzy** after synthetic
calls are replaced with real typed calls; the nonexact change is
accepted for the ARM-confirmed behavior and AArch64 pointer layout.
The focused set has **94 linked-exact / 23 byte-exact** symbols; no
prior exact symbol is lost.

The strict AArch64 JNI-initialize diagnostic remains at **130**
unresolved names, with no new ones. A broader exported-symbol link
diagnostic falls **361 → 354**, removing seven lifecycle placeholders
with no additions. Both links still fail; there is no game `.so` or APK.
Both use NDK 28.2 AArch64 API 28, the current `libgof2.a`, AArch64
libzip/crypto, and Android GLES/EGL/log/zlib. The strict diagnostic
roots `ToJNI_initialize` and excludes unreferenced archive exports;
the broader one roots `JNI_OnLoad` and retains exported symbols.

The full ARM corpus compares **4,323/4,524** originals over **4,745**
pairs: **94.77% mean source-shape / 84.27% strict fuzzy**,
**2,607 linked-exact / 1,083 byte-exact**, zero skips, and no losses from either
previous exact-symbol set. `Suspend` is the one gained linked-exact
symbol; the corpus strict-fuzzy mean changes **84.28% → 84.27%**.

Live GL context loss/restore, texture data, and a running game are not
yet verified. The `PaintCanvas` destructor still has separate release
placeholders and ARM32 texture overlays. Compatible audio and Android
host packaging remain major priorities for a playable ARM64 build.
