# Engine::SetTextures first-slot ARM recovery (2026-10-30)

The primary source is original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Engine::SetTextures` spans `0x84c44`–`0x84d66`. At `0x84c64`–`0x84cd6`
it loads the first texture entry, checks the bound GL name, selects
`GL_TEXTURE0`, updates the texture environment, and binds the first
name to `GL_TEXTURE_2D` unconditionally. The reconstructed C++ had
delegated that slot to `SetTextureSlot`, whose own code selects 2D or
cube map from `TextureEntry::isCube`. The first-slot source now follows
the observed ARM branch and leaves the separately implemented
`SetTextureSlot` path for `SetTexturesExt` unchanged.

Original ARM `SetTextures` chooses the **second** slot target from
`g_Engine_useShaders` and `glEnableFlags & 0x80008` at `0x84d3a`–`0x84d5a`.
That branch remains in the reconstruction. Focused ARM comparison of
`SetTextures` improved from **70.1% to 86.3% source-shape** and from
**46.6% to 51.5% strict fuzzy**. It remains nonexact. Native `gof2`,
ARM64 static, and all 209 ARMv7 translation units build successfully.

The fresh full ARMv7 corpus compares **4,323/4,524** original
functions across **4,745** pairs, with **94.86%** mean source-shape,
**84.43%** strict fuzzy, **2,638 linked-exact** and **1,086
byte-exact**. There are zero skipped units and 201 missing original
functions. The complete `(unit, symbol)` linked-exact and byte-exact
sets are identical to the preceding `sandbox-full-20261030.json`
snapshot. The focused before/after and full reports are saved outside
Git in `D:\gof2-arm64-crypto`.

A strict ARM64 shared library linked without undefined symbols and kept
the preceding 53 JNI exports. Its signed APK was installed in the same
Android 15 emulator as the preceding build. In a bounded Frida trace of
the first 90 GL binds from each run, GL names 2 and 5 were bound as both
`GL_TEXTURE_2D` and `GL_TEXTURE_CUBE_MAP` before this patch. After it,
both names were bound only as `GL_TEXTURE_2D`; no name in that sample
used both targets. The respective traces are saved outside Git as
`settextures-gl-before-20261030.json` and
`settextures-gl-after-20261030.json`. Sample timing and total bind
counts differ, so this establishes removal of the observed target
conflict, not a global rendering equivalence.

The patched process remained alive. Its captured GL log contained no
`s_glBindTexture` errors, although it still recorded 240
`s_glGetUniformLocation` and 72 `s_glGetAttribLocation` errors (0x501).
After dismissing the Google Play Services dialog, the frame remained
black. The second-slot entry and material flags in the original game
have not been traced directly. The original Android 9 emulator runs
through the opening scene, but its ARM native bridge has not provided
stable internal-function instrumentation. Use it for gameplay and JNI
boundary comparisons; an ARM device or another ARM-aware tracer is
needed for direct original `SetTextures` arguments.
