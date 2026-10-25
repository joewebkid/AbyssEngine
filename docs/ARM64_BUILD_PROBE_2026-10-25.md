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
