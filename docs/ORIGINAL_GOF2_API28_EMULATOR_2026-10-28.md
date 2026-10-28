# Original GOF2 HD on the Android 9 emulator (2026-10-28)

## Reproducible environment

- AVD: `GOF2_Original_ARMv7_API28`, stored at `D:\AndroidAVD\GOF2_Original_ARMv7_API28.avd`.
- System image: Google APIs Android 9 (API 28), x86, revision 12, stored in `D:\AndroidSdk\system-images\android-28\google_apis\x86`. The downloaded archive's SHA-1 matched Google's SDK metadata: `e0541ac9b783ab91a00054e133bda340a2b9c757`.
- At runtime `ro.product.cpu.abilist=x86,armeabi-v7a,armeabi` and `ro.dalvik.vm.native.bridge=libndk_translation.so`. ADB root is available.
- Original APK: `net.fishlabs.gof2hdallandroid2012`, version 2.0.16 (versionCode 47947032), target SDK 27, `armeabi-v7a` only. SHA-256: `515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`.
- OBB: `main.47947006.net.fishlabs.gof2hdallandroid2012.obb`, 1,011,128,870 bytes. The SHA-256 on the emulator matched the local source: `42cfe13f0f3b9f99f38a1e77c74ac1cf41e542b3becbffea0f5bb7cfc4a6e4cb`.

## Observed result

The APK installed successfully. After granting storage access and dismissing the optional Google Play Games prompt, the original game displayed its title and main menu. Starting a new normal-difficulty GOF2 campaign reached the opening scene with two rendered ships and dialogue. The game process remained alive at that point. Screenshots are stored outside the repository at `D:\gof2-arm64-crypto\gof2-original-api28-state5.png` (menu) and `D:\gof2-arm64-crypto\gof2-original-api28-state8.png` (opening scene).

This confirms that the original APK and OBB can be used as a repeatable behavioral reference through ship rendering. It does **not** establish equivalence of the rebuilt ARM64 code or prove which material resource caused its `PlayerEgo::setShip` null dereference. ARMv7 instructions execute through the emulator's native bridge; use the original binary and a physical ARM device for instruction-level evidence.

## Next diagnostic step

Capture the original material/mesh resource IDs and success/failure path at the first ship creation, then compare them with the rebuilt ARM64 run. In the current source, `PaintCanvas::MeshCreate(resId, out, forceClone)` obtains `info->matResId`, calls `MaterialCreate`, and passes the resulting pointer to `MeshCreateFromFile`; this is the path to test before changing `PlayerEgo::setShip`. The emulator supports root and `strace`, but the initial five-second `openat` sample during the already-running scene contained only `/proc/self/cmdline` reads. A trace at the resource-loading point or targeted function instrumentation is still needed.

REA opened the original `libgof2hdaa.so` as ARM ELF. Its ADB provider lacked the path to `adb` in the current server process; `REA_ADB_PATH` was added to the Codex MCP configuration for the next REA restart. The emulator checks above used `D:\AndroidSdk\platform-tools\adb.exe` directly.
