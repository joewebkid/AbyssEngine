# FMOD app-root call and ARM64 link closure (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`FModSound::FModSound` at `0x92b70` calls `AEFile::GetAppRootDir`
at `0x92b8a` and stores the returned pointer at `this+0x0c`.

The recovered constructor instead called an undefined
`AEFile_GetAppRootDir` helper. It now calls the existing
`AEFile::GetAppRootDir` directly. The pointer is borrowed; the
constructor does not allocate or own a path. Both constructor aliases
were already 100% source-shape, strict-fuzzy and linked-exact before
the change because linked-exact masks relocation targets. They retain
those scores after a rebuilt ARMv7 comparison.

Native `gof2`, the AArch64 static archive, and **209/209** ARMv7
translation units compile. A strict AArch64 whole-archive shared-link
diagnostic with system GL/log/Android/zlib libraries now reports
**582** unresolved symbols, down from **583**. It still fails to
produce a shared library. No audio playback or Android startup is
claimed. The live lifetime of the borrowed app-root pointer remains
unverified.

The full ARM corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs with zero skips. Mean source-shape and
strict fuzzy remain **94.71%/84.21%**. The linked-exact and
byte-exact sets remain **2,600** and **1,082**, respectively, with
zero symbol-level gains or losses versus the preceding full report.
