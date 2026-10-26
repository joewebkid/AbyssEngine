# PaintCanvas resource release (2026-10-26)

Primary evidence is the original Android 2.0.16 ARMv7
`libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
IDA's `PaintCanvas::ReleaseAllResources` at `0x77c84` resets
`CubeMapSetIndex`, invalidates resource handles, deletes loaded GL
textures, decrements `Engine::ImageCount` and the engine's texture-byte
counter, then releases fonts, images, meshes, transforms, cameras,
materials, and sprite systems. Mesh release also subtracts its
`+0x7c` VBO contribution from `Engine::vboSize`.

The recovered function now calls the already implemented typed engine
release functions and C++ destructors. It reads loaded textures through
`AELoadedTexture`, including their `String` name and byte count,
instead of the fixed ARM32-offset overlay. That preserves the correct
member addresses on AArch64. The GLES1 header now declares the
standard `glDeleteTextures` call. The old local fake counters and nine
unresolved release placeholders were removed.

Focused ARMv7 verification compiles **209/209** units and compares
**198** `PaintCanvas` symbols. `ReleaseAllResources` rises **81.5% →
83.9% source-shape** and **45.5% → 56.5% strict fuzzy**; it remains
nonexact. The focused mean rises **95.98% → 95.99% source-shape** and
**82.87% → 82.92% strict fuzzy**. All **93 linked-exact / 23 byte-exact**
symbols in the focused set are preserved.

Native and AArch64 static builds pass. The production function body
passes a 64-bit host smoke test at `-O0` and `-O2`, checking GL deletion,
counter changes, destructors, release calls, array counts, and cleared
slots. The same strict AArch64 JNI-initialize link diagnostic falls
**139 → 130** unresolved names: the nine removed release placeholders
are absent and no new names appear. The link still fails, so no game
shared library or APK was produced.

The diagnostic uses NDK 28.2's AArch64 API 28 linker with
`--no-undefined`, `--gc-sections`, `--exclude-libs,ALL`, and the
`Java_net_fishlabs_gof2hdallandroid2012_ToJNI_initialize` root, linking
`libgof2.a`, the built AArch64 libzip/crypto archives, and Android
GLES/EGL/log/zlib libraries. The comparison is against the prior
identical JNI-initialize diagnostic, not a whole-archive inventory.

The full ARM corpus compares **4,323/4,524** original functions over
**4,745** pairs: **94.77% mean source-shape / 84.28% strict fuzzy**,
**2,606 linked-exact / 1,083 byte-exact**, zero skips, and no losses
from either exact-symbol set. The rounded corpus means and coverage
are unchanged from the preceding package.

Device GL lifecycle, resource ownership across other `PaintCanvas`
methods, and game startup remain unverified. The next resource-lifecycle
priority is the canvas destructor and cube-texture suspend/restore
paths, which still contain unresolved calls and ARM32-offset overlays.
