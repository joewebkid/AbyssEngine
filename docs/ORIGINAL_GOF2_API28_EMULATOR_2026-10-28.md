# Original GOF2 HD on the Android 9 emulator (2026-10-28)

## Stable original run and native-bridge observation (2026-10-30)

After a fresh boot without Frida, the same original APK and OBB reached the
title, main menu, campaign selection, and opening GOF2 scene; the game process
remained alive. A screenshot of the opening scene is outside Git at
`D:\gof2-arm64-crypto\gof2-original-opening-native-trace-20261030.png`.
This confirms the emulator is useful for repeatable gameplay and visual
comparison.

Attaching Frida 17.24.3 to the already-running game in the `emulated` ARM
realm again closed the connection and coincided with a game-process SIGSEGV
at address `0x3` in `libndk_translation`/`linker_arm`. After restarting the
game, attaching in the x86 `native` realm succeeded repeatedly without
killing it. A bounded hook on the exported
`ndk_translation::RunGuestCall` recorded 1,652 entries into the game's ARM
library during a ten-second opening-scene interval, out of 2,121 total guest
entries. The frequently observed game-library offsets map through the
original ARM symbol table to `ToJNI_renderstep` (`0x6b848`, 194 entries),
`ToJNI_handleAccelerometer` (`0x6b866`, 483 entries), and status or Google
Play Games JNI getters (`0x6b850`, `0x6b854`, `0x6ba20`, `0x6ba40`,
`0x6ba60`, 194 entries each). The exact sample is saved outside Git at
`D:\gof2-arm64-crypto\original-api28-native-bridge-jni-20261030.log`.

These are calls crossing from Android/x86 into ARM, not internal calls such
as `MaterialCreate` or `MeshCreate`; their arguments do not establish the
ship's material ID. A hook on the host `libGLESv1_CM.so` exports recorded no
calls during this interval, so that export path is not a useful rendering
trace on this emulator. Continue using the emulator for behavior and JNI
boundary checks. Use a physical ARM device or another verified ARM-aware
tracing method for internal game-function arguments and returns.

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

## Targeted tracing attempt (2026-10-29)

Frida 17.24.3 client and matching x86 server were prepared under `D:\gof2-arm64-crypto` (outside Git). On the emulator, ADB root worked, the server was pushed to `/data/local/tmp/frida-server`, and `pidof frida-server` returned a live PID. The original package launched into `DownloaderActivity`, and its process was present. The emulator later disappeared from ADB before the client could attach in the emulated ARM realm. A second cold boot with a 1024 MiB RAM setting also ended before a trace was captured. The available emulator logs do not identify why those runs ended, so neither the original material ID nor the first `PlayerEgo::setShip` path was observed dynamically.

The next trace should attach to the original ARM process before navigating from the menu into a new campaign. Capture `PaintCanvas::MeshCreate`, `MaterialCreate`, `MeshCreateFromFile`, and `Globals::getShipGroup`, including arguments and returned pointers. Until then, the null material in the rebuilt ARM64 run remains an open diagnostic, and the source should not be changed on the assumption that material resource ID zero is correct or incorrect.

## Retry and crash evidence (2026-10-30)

In a continuous session the emulator booted, remained connected to ADB, and ran Frida server 17.24.3 as root. The original GOF2 process appeared twice. Each attempt to attach in Frida's `emulated` realm returned `ProtocolError: the connection is closed`; Android's crash buffer records the original game process (PIDs 3143 and 4701) dying with `SIGSEGV`, fault address `0x3`, while executing the x86 `libndk_translation.so` ARM guest-call path through `linker_arm`/`android_dlopen_ext`. The emulator itself stayed alive. This corrects the earlier inference that the entire emulator had failed during the first attempt. The evidence does not establish whether attaching Frida caused the guest fault or merely coincided with an already failing load. The crash log is outside Git at `D:\gof2-arm64-crypto\original-api28-crash-20261030.log`.

The new `ResourceMesh` argument for ship 010 is compiled into the ARM64 static library, but no rebuilt-game run has yet tested it. A physical ARM device is the next practical target for the original ARM function trace; the currently connected phone was in active use and was not interrupted. The Android 9 emulator was shut down after log capture to release host memory.
