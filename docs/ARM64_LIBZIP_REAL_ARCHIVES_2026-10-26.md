# ARM64 libzip and original archive read gate (2026-10-26)

The recovered Android file path uses the libzip 0.9.3 API. The existing
`third_party/libzip/libzip_gof2.a` contains ARMv7 objects and cannot link
into AArch64. `tools/android/build_arm64_libzip.ps1` builds the same upstream
0.9.3 source for Android API 28 with NDK 28.2. The source download is checked
against SHA-256 `0e282dfed9e3da7d52223a0ccaee7c950782f10ed73f9b778243128a03c8cb1c`.
The generated source, object files, test executable and library stay outside
Git. The AArch64 port uses Android libc's `mkstemp`; the separate original
ARMv7 build retains its bundled fallback.

Reproduce on Windows (choose a build directory with adequate free space):

```powershell
./tools/android/build_arm64_libzip.ps1 `
  -NdkRoot 'D:\AndroidSdk\ndk\28.2.13676358' `
  -BuildDir 'D:\gof2-arm64-libzip'
```

An object from the resulting archive is ELF64/AArch64. Adding that archive
after the game archive, with Android `log`, GLES, `android` and `z` libraries,
to the strict `--whole-archive --no-undefined` shared-link diagnostic reduces
the unresolved-name set from **532 to 526**. Exactly six names disappear:
`zip_open`, `zip_get_num_files`, `zip_get_name`, `zip_fopen`, `zip_fread`,
and `zip_fclose`. There are no new unresolved names. This is a diagnostic
link; the game shared library still fails to link on other missing symbols.

On the connected Surya device (`arm64-v8a,armeabi-v7a,armeabi`), the
AArch64 test executable built by the script read three entries through
libzip 0.9.3 itself:

| Original archive entry | Bytes | SHA-256 |
| --- | ---: | --- |
| Installed APK `assets/data/bin/items.bin` | 27,628 | `639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c` |
| OBB `47947_GOF2CONTENT_ETC/assets/data/bin/items.bin` | 27,628 | `55b953629d994d553604418988d43d2b1935f93061da7040a464acad57677e95` |
| OBB `47947_GOF2CONTENT_ETC/assets/data/bin/stations.bin` | 3,408 | `761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3` |

The hashes match the earlier independent host archive gate. The test exercises
`zip_open`, enumeration/name lookup, `zip_fopen`, repeated `zip_fread`,
`zip_fclose`, and `zip_close`. Temporary extracted files and the test binary
were removed from `/data/local/tmp` after verification. The original APK and
OBB were not changed. This proves the ARM64 ZIP dependency can read these
real archives; it does not execute the recovered game's `AEFile` or Android
startup code on ARM64. Crypto, FMOD, UI/bridge and other unresolved symbols
remain. The ARMv7 decompilation source and exact-match counts are unchanged
by this separate dependency build.
