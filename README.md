# Abyss Engine / Galaxy on Fire 2 HD Decompilation

Work-in-progress C++ reconstruction of the Android ARMv7 build of *Galaxy on
Fire 2 HD* and its proprietary Abyss Engine runtime.

This is independent reverse-engineering research. It is not an official
Fishlabs product, not a playable game, and does not redistribute game binaries,
assets, APKs, IPAs, OBB files, or JARs.

## Current State

The tree contains 204 C++ translation units and 234 headers. The native static
library builds on Windows/MSYS2 UCRT64 and is used as a compile regression gate.
ARM validation is a separate function-by-function comparison workflow.

The checked-in `report.json` snapshot records 4,507 compared symbols out of
4,522 original functions, with 1,697 linked-exact and 948 raw-byte-exact
functions. These are code-generation comparison figures, not a claim that each
function has a complete or semantically verified body.

Read [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md) for the recovery map,
evidence terminology, confirmed packages, and open work.

## Included And Excluded

Included: recovered C++ under `src/`, build and verification tools, public
documentation, symbol maps, and small third-party compatibility inputs.

Excluded: original `libgof2hdaa.so`, all game assets, APK/IPA/OBB/JAR files,
extraction dumps, local NDK installations, and build output.

For local ARM validation, place a lawfully obtained original binary at
`_work/bins/android_2.0.16_libgof2hdaa.so`. That directory is ignored by Git.

## Build

### Windows Native Regression Build

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```

Expected output: `cmake-build-ucrt/libgof2.a`.

### ARM Function Validation

ARM comparison needs an NDK r18b-compatible toolchain and a local copy of the
original binary. On a supported Linux/OrbStack setup:

```bash
bash tools/verify/setup.sh
cmake --preset match
cmake --build cmake-build-match --target verify
```

On Windows, set `GOF2_VERIFY_LOCAL_NDK=1` and follow
[docs/VALIDATION.md](docs/VALIDATION.md). This is optional for normal native
development and does not distribute the reference binary.

## Repository Map

| Path | Purpose |
| --- | --- |
| `src/engine/` | Recovered Abyss Engine systems. |
| `src/game/` | Recovered UI, ships, missions, weapons, and world logic. |
| `src/platform/android/` | Android/JNI boundary helpers. |
| `tools/` | Recovery, reconciliation, and matching tools. |
| `_work/symbols/` | Public symbol and address maps. |
| `docs/` | Status, evidence, validation, relink, and format notes. |

## Supplementary Source Archives

The following separately packaged research sources are available on Google
Drive. They contain recovered source, decompiler output, symbols, and analysis
metadata only; original game binaries and asset dumps are not included.

- [GOF2 recovered SDK and gameplay sources](https://drive.google.com/file/d/19qaY3gI8KJo8puPdbdxN4V4XPQZFPBzY/view?usp=drivesdk)
- [GOF2 IDA decompilation dumps and excerpts](https://drive.google.com/file/d/1hFRpZI6mPOCAI5l6di8DG9wF8AIZchyU/view?usp=drivesdk)
- [GOF2 Ghidra decompilation dumps](https://drive.google.com/file/d/1uufno17h1NFYXnTKOPxNigpPIdZBlR7A/view?usp=drivesdk)
- [GOF2 J2ME decompiled Java sources](https://drive.google.com/file/d/1qEeIncQSas8585afKe1ICyZ0FCGjbBkA/view?usp=drivesdk)
- [GOF1 analysis sources and decoded tables](https://drive.google.com/file/d/1Pp790FyWJLmA17yJPcNe4Xys4C0f0sSr/view?usp=drivesdk)
- [Galaxy on Fire Alliances analysis sources](https://drive.google.com/file/d/11tcxQrg--2YaA-JvxfXUnHgj5GhButIl/view?usp=drivesdk)

## Documentation

- [Project status](docs/PROJECT_STATUS.md)
- [Contributing](CONTRIBUTING.md)
- [Agent guide](AGENTS.md)
- [Validation](docs/VALIDATION.md)
- [Identicality roadmap](docs/IDENTICALITY.md)
- [AEM/AEI loader notes](docs/AEM_AEI_NATIVE_LOADER_SPEC.md)
- [Notice](NOTICE.md)

## Contributing

Contributions are welcome when they preserve the evidence trail. Read
[CONTRIBUTING.md](CONTRIBUTING.md), keep each patch focused, and do not add
original game binaries or asset dumps.
