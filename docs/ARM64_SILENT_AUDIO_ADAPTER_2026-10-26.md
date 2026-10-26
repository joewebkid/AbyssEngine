# ARM64 silent audio adapter (2026-10-26)

The first `arm64-v8a` startup milestone is explicitly silent. The old
FMOD Ex event library used by the Android ARMv7 release is unavailable
as an arm64 binary in this project. `GOF2_ARM64_SILENT_AUDIO` selects
an ARM64-only no-output compatibility file outside `src/`. It provides
the FMOD methods and bridge functions needed to link the currently
reachable native game path, returns empty event lookups, and produces
no sound. This is a port scaffold, not recovered FMOD behavior.

`FModSound::init` skips loading the original `.fev` bank in this mode
and leaves `system`, `music`, and `initialized` empty. Normal ARMv7
source stays on its original branch. No game assets or proprietary
FMOD binary are added to the repository.

The ARM64 `gof2` archive and native `gof2` regression build pass after
the silent initializer is selected. A host smoke executable using the
production `FModSound.cpp` and `SilentFmod.cpp` passes with
`AEFile::GetAppRootDir()` returning null: `init()` returns zero,
the audio system stays null, `isPlaying(0)` stays false, and release
does not access a library instance. This tests the no-bank startup
path; it is not an Android device test.

The NDK r18b rebuild compiles **209/209** ARMv7 translation units.
`FModSound.o` retains its previous SHA-256
`6b95b2e76497a31a51a9d6b44f219aa8adf995b4efe680183018d79880da4cce`.
The compatibility file is outside `src/` and is added only for Android
`arm64-v8a`; no ARMv7 comparison input changed. Therefore the last
full corpus remains **4,323/4,524** originals over **4,745** pairs,
**94.79% source-shape / 84.30% strict fuzzy**, **2,607 linked-exact /
1,083 byte-exact**, zero skips. A new full comparison was not run for
this ARM64-only package.

With the compatibility file alone, the AArch64 static library builds.
The strict JNI-initialize link diagnostic falls from **88 to 56**
unresolved names and the broader JNI-onload diagnostic from
**304 to 266**, with no new names. The remaining audio-shaped names
are game-side `FModSound` wrappers that need separate recovery or
silent-mode routing. A complete game shared library and APK are not
built, and no Android runtime behavior is verified yet.
