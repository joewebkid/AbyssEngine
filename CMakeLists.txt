cmake_minimum_required(VERSION 3.20)
project(GalaxyOnFire2 LANGUAGES CXX)

# ---- matching ASM-validation build (the `match` preset) ----------------------------------------
# When configured with cmake/orbstack-ndk-arm.toolchain.cmake, this is NOT the native build: it
# exposes function-level validation of our ARM build output against the original libgof2hdaa.so.
# `cmake --build cmake-build-match --target verify` compiles every TU with the NDK r18b toolchain
# (in OrbStack), delinks the matching originals out of the .so, disassembles both with GNU objdump
# (objdiff can't read ARMv7), and prints a per-function match-% table. Tune GOF2_MATCH_OPT (and the
# flags in tools/verify/match_flags.sh) and re-run to chase byte-exactness.
if(GOF2_MATCH)
    set(_verify_py "${CMAKE_SOURCE_DIR}/tools/verify/verify.py")
    set(_verify_dir "${CMAKE_BINARY_DIR}/verify")
    # --fail-on-wrong-type makes the target fail the build if any function is implemented under a
    # different (wrong) signature than the original .so (report.json missing_wrong_type > 0); the
    # full diff table still prints first. Keeps the 0-wrong-signature invariant from regressing.
    add_custom_target(verify
        COMMAND ${CMAKE_COMMAND} -E env "GOF2_MATCH_OPT=${GOF2_MATCH_OPT}"
                python3 ${_verify_py} --build-dir ${_verify_dir} --fail-on-wrong-type
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} USES_TERMINAL
        COMMENT "Validating ARM build vs original .so (opt=${GOF2_MATCH_OPT})")
    # Single-function side-by-side diff:  FN=<mangled> cmake --build ... --target verify-fn
    # verify.py reads the symbol from the FN environment variable (passed through
    # to the build at run time).
    add_custom_target(verify-fn
        COMMAND ${CMAKE_COMMAND} -E env "GOF2_MATCH_OPT=${GOF2_MATCH_OPT}"
                python3 ${_verify_py} --build-dir ${_verify_dir} --no-build --show
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} USES_TERMINAL
        COMMENT "Diffing one function (set FN=<mangled symbol>)")
    return()  # native library/host targets below are not part of the matching build
endif()

# Single source tree for native GOF2 recovery and future platform ports, based on
# decompilation of the Android libgof2hdaa.so. This replaces the original byte-matching ARM decomp
# (which lived under src/recovered, work/, tools/ and was driven by a Makefile + NDK toolchain) —
# that work is preserved in git history. Layout:
#   src/           one .cpp + its co-located .h per class; shared foundation (common.h/math.h/
#                  externs.h, platform/) at the tree root. Include paths are relative to src/.
#   host/          future platform entry points and runtime services (not implemented yet)
#   tools/         transformation/build helpers (rewrite_body.py, reconcile.py, build.sh, ...)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)               # gnu++14 (the recovered code relies on GNU extensions)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)      # compile_commands.json for CLion/clangd code intelligence
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---- engine + game library --------------------------------------------------------------------
# The recovered code still carries Android/ARM-era 32-bit handle casts. GNU's native 64-bit builds
# need -fpermissive so those decomp artifacts remain warnings while restoration work continues.
file(GLOB_RECURSE GOF2_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(gof2 STATIC ${GOF2_SOURCES})
target_include_directories(gof2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(gof2 PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-fpermissive>
    -Wno-int-to-pointer-cast -Wno-int-to-void-pointer-cast)

option(GOF2_BUILD_PARTICLE_SETTINGS_SMOKE
       "Build the native ParticleSettingsRef ABI/runtime smoke test" OFF)
if(GOF2_BUILD_PARTICLE_SETTINGS_SMOKE)
    add_executable(gof2_particle_settings_smoke
        tools/particle_settings_ref_smoke.cpp
        src/engine/render/ParticleSettings.cpp
        src/engine/render/ParticleSettingsRef.cpp)
    target_include_directories(gof2_particle_settings_smoke PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

option(GOF2_BUILD_ABI_RUNTIME_SMOKE
       "Build the focused native ABI/lifetime smoke test" OFF)
if(GOF2_BUILD_ABI_RUNTIME_SMOKE)
    add_executable(gof2_abi_runtime_smoke
        tools/abi_runtime_smoke.cpp
        src/game/core/String.cpp
        src/engine/core/IApplicationModule.cpp
        src/game/weapons/AbstractGun.cpp)
    target_include_directories(gof2_abi_runtime_smoke PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(gof2_abi_runtime_smoke PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(gof2_abi_runtime_smoke PRIVATE -Wl,--gc-sections)
    endif()
endif()

option(GOF2_BUILD_PREMIUM_IAP_SMOKE
       "Build the Android premium purchase callback smoke test" OFF)
if(GOF2_BUILD_PREMIUM_IAP_SMOKE)
    add_executable(gof2_premium_iap_smoke
        tools/premium_iap_smoke.cpp
        src/platform/android/NdkHooks.cpp)
    target_include_directories(gof2_premium_iap_smoke PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/jni"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/gl"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmod/inc")
    target_compile_definitions(gof2_premium_iap_smoke PRIVATE GOF2_PREMIUM_IAP_SMOKE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(gof2_premium_iap_smoke PRIVATE -fpermissive -ffunction-sections -fdata-sections)
        target_link_options(gof2_premium_iap_smoke PRIVATE -Wl,--gc-sections)
    endif()
endif()

# JNI
target_include_directories(gof2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/jni")

# OpenGL ES 2.0 (Khronos registry headers, header-only)
target_include_directories(gof2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/gl")

# ---- FMOD Ex 4.44.07 (audio) ------------------------------------------------------------------
target_include_directories(gof2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmod/inc")

# libzip 0.9.3 (asset/APK archive access)
target_include_directories(gof2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libzip")

# OpenSSL libcrypto (vendored SHA-256 API subset for record integrity hashing)
target_include_directories(gof2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/crypto")
