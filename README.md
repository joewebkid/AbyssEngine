# Galaxy on Fire 2 — decomp

A work-in-progress decomp of the *Abyss Engine* + *Galaxy on Fire 2* game
(Fishlabs, ~2012), recovered by decompiling the Android `libgof2hdaa.so`.

## Build
Open the folder in **CLion** (it reads `CMakePresets.json` and configures automatically), or from a shell:
```
cmake --preset debug
cmake --build cmake-build-debug --target gof2_host   # runnable placeholder today
cmake --build cmake-build-debug --target gof2        # the engine/game library (cleanup ongoing)
```
`compile_commands.json` is exported for clangd/CLion code intelligence. Requires a C++14 (gnu++14)
clang; no external deps yet (SDL2/GLES2 arrive with the platform layer).

### Windows native build (MSYS2 UCRT64)

On Windows this tree is known to build with MSYS2 UCRT64 GCC 16.1.0, Ninja 1.13.2 and CMake 4.3.x:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```

Expected output:

- `cmake-build-ucrt/libgof2.a`

The native build is a compile/regression gate. ARM byte-match validation remains the separate `match` preset below.
The current 2026-06-30 recovery package, including `MovingStars`, `MeshMerger`,
`SimpleMeshMerger`, `LodMeshMerger`, confirmed `Radar` layout offsets and Radar behavior first pass,
the 2026-07-01 `AEGeometry`/`ParticleSystem` helper-route pass and small upstream
byte-match/engine fixes through `7995687`, the `BuildResourceList` resource-table rewrite, `Mesh::pivotZ`, weapon runtime
layout (`Gun`, `AbstractGun`, `SpriteGun`, `BeamGun`, `ObjectGun`), FMOD helper routing including
`EventSystem::init/update`, `AERandom::nextInt` helper routing, small PaintCanvas/SpaceLounge helper routes, the follow-up
`SpriteGun`/`AbstractGun` ctor/export audit, and the AEM/AEI native loader notes, is tracked in
`docs/DECOMP_NOTES.md`, `docs/AEM_AEI_NATIVE_LOADER_SPEC.md`, and the workspace recovery note
`../docs/findings/gof2hd_decomp_recovery_status_20260630_ru.md`.

### ASM validation (matching build)
To check whether our code still assembles to the **same instructions** as the original `.so`,
function by function, build with the matching NDK r18b toolchain (via OrbStack) and diff against the
binary:
```
bash tools/verify/setup.sh                              # one-time OrbStack provisioning
cmake --preset match
cmake --build cmake-build-match --target verify         # prints a per-function match-% table
```
This is independent of the native build above. See **[docs/VALIDATION.md](docs/VALIDATION.md)**.

## Ground truth
The original binary is loaded in Ghidra as `android_2.0.16_libgof2hdaa.so` (ARM32). It remains the
authoritative reference for function signatures, struct field types, and behavior during the port.
It is not redistributed by this repository: place a lawfully obtained copy in `_work/bins/` for local
ARM validation.
