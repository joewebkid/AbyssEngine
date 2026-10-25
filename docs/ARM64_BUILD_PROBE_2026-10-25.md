# Android arm64 build probe (2026-10-25)

An Android NDK 28.2 / API 28 CMake build with `ANDROID_ABI=arm64-v8a`
compiles the current **209** game/engine translation units into
`_work/arm64-build-20261025/libgof2.a` (20,266,296 bytes, local build
output, not committed). `llvm-readelf` reports ELF64 / AArch64 for a
member object. The only source change required for this compilation
guards two empty `+t` inline-assembly register constraints in
`HangarWindow::getRelativeScrollHeight`; the existing ARMv7 and native
paths keep the same source operations. Native `gof2` and **209/209**
ARMv7 translation units still build. The ARMv7 `HangarWindow.o` SHA-256
before and after the guard is identical
(`47e6bc5cb68b52878d0f911fbf94d99c60a158d36807af0631bbbc0444564842`),
so the arm64-only guard does not perturb that object or its existing
exact-match symbols. Focused `getRelativeScrollHeight` remains
**96.3% source-shape / 63.0% strict fuzzy**, nonexact.

Reproduction on this Windows host:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe -S . -B _work/arm64-build-20261025 -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=D:/AndroidSdk/ndk/28.2.13676358/build/cmake/android.toolchain.cmake `
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 `
  -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release
C:\msys64\ucrt64\bin\cmake.exe --build _work/arm64-build-20261025 `
  --target gof2 --parallel 8
```

This is **not an arm64 game build**. `CMakeLists.txt` defines `gof2`
as a static library and has no Android app/Gradle target. An attempted
whole-archive shared-library link with `--no-undefined` fails first
on duplicate `AEMath::Vector`, `AEMath::Matrix`, and `Quaternion`
symbols. A diagnostic repeat allowing duplicates exposes unresolved
engine functions such as `AE_getInitGLThis`, `AE_getInitGLWidth`,
`AE_getInitGLHeight`, `AEFile_GetAppRootDir`, and `MeshCreate`, plus
FMOD event/audio functions. No `libgof2hdaa.so` was produced.
Allowing duplicate definitions was diagnostic only, not an accepted
link or runtime fix. The repository contains FMOD 4.44.07 headers but
no arm64 FMOD libraries. `JNI_OnLoad` and some JNI routes exist, but
the Android host, full dependency closure, and game startup still need
implementation and validation. ARMv7 data structures also retain
32-bit layout assumptions that require an arm64 ABI audit.

The original APK contains only `lib/armeabi-v7a/` native libraries,
while the connected Android 16 phone reports only `arm64-v8a`.
Installing the original APK failed with `INSTALL_FAILED_NO_MATCHING_ABIS`.
Adding the static archive to that APK cannot make it runnable. No APK
was generated or installed as part of this probe.

## Link-closure follow-up (2026-10-25)

The two `src/game/core/Matrix.cpp` and `Vector.cpp` files re-define
symbols already supplied by `src/engine/math/Matrix.cpp`, `Vector.cpp`,
and `AEMath.cpp`. The engine-side matrix constructor reaches **100%**
ARMv7 source-shape in the current corpus, versus **78.7%** for the
game-side copy. CMake now omits only the two game-side duplicates for
`arm64-v8a`; the ARMv7 comparison and native source lists are unchanged.
The arm64 archive has **207** unique translation units, 20,240,668
bytes. A strict, whole-archive shared-link diagnostic now reports
**zero duplicate definitions**, down from the duplicate-symbol failure
above. This is a link-closure step, not a claim that the entire game
can be linked or that the two variants have identical runtime semantics.

The same diagnostic still reports **583 unique unresolved symbols**.
They include **56** names containing FMOD/FMod, **20** `RB_` bridge
helpers, **25** `_mtw_`/`_mw_` UI helpers, math and engine helpers, and
ZIP/crypto calls. These categories overlap and are not additive. A
whole-archive link intentionally includes code that a final app might
discard, so **583 is a closure inventory, not a count of required
startup fixes**. The vendored libzip and OpenSSL archives are ELF32;
they cannot be linked into an AArch64 shared library. No `.so` or APK
has been produced.

FMOD's maintainer states that **FMOD Ex never supported Android arm64**
([FMOD forum](https://qa.fmod.com/t/where-can-i-get-a-fmodex-pre-built-library-to-use-on-build/14634?page=5)).
Current FMOD has arm64 binaries, but its own documentation says sound
banks built with FMOD Ex/Designer may use obsolete codecs and are not
compatible with the modern engine
([FMOD glossary](https://www.fmod.com/docs/2.04/api/glossary.html));
FMOD support also says the modern Core API cannot read `.fev` files
([FMOD forum](https://qa.fmod.com/t/how-can-i-update-a-fsb-file-to-a-bank-file/21726)).
The recovered game loads `.fev` projects in `FModSound::init`. Thus
switching to current FMOD is an audio-content migration, not a binary
replacement. A silent/alternate backend would change behavior and
requires a separate decision and validation.

Validation after the source-list change: the Android arm64 archive builds;
the native `gof2` target builds without work; the NDK r18b verifier
compiles **209/209** ARMv7 units with zero errors. `GOF2_MATCH` returns
from CMake before the arm64-specific source filter, so the ARMv7
comparison inputs and the last full corpus exact sets (**2,600
linked-exact**, **1,082 byte-exact**) are unchanged by this CMake edit.
The full corpus was not rerun for this build-configuration-only change.
