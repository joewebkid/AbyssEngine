# Matching ARM (armeabi-v7a, NDK r18b) toolchain for the `verify` build.
#
# Normal development uses the local Apple-clang `debug`/`release` presets and is
# unaffected by this file. The `match` preset selects this toolchain to enable the
# function-level ASM-validation targets (`verify`, `verify-fn`).
#
# The matching compiler only runs on Linux, so it lives in the OrbStack `ubuntu`
# machine and is invoked through tools/verify/orbcc. CMake itself does NOT compile
# anything in this mode (the resilient per-TU build is driven by build_objs.sh, so
# one un-portable TU can't abort the whole run) — hence we mark the compiler as
# working to skip CMake's try_compile probe.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

get_filename_component(_gof2_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(CMAKE_CXX_COMPILER "${_gof2_root}/tools/verify/orbcc")
set(CMAKE_CXX_COMPILER_ID Clang)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(GOF2_MATCH ON CACHE BOOL "Matching ARM verification build (objdump differ)")
set(GOF2_MATCH_OPT "-Oz" CACHE STRING
    "Optimization level for the matching build; the main flag to tune (-Oz/-O2/-Os/...)")
