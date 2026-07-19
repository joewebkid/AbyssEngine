# toolchain.cmake -- the byte-matching ARM (armeabi-v7a, NDK r18b) toolchain.
#
# This is the project's only toolchain: the top CMakeLists applies it automatically, so a plain
# `cmake -B build` configures the matching build. cmake/DownloadNDK.cmake fetches NDK r18b and the
# compiler runs natively (darwin-x86_64 under Rosetta 2 on Apple Silicon).

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

get_filename_component(_gof2_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
include("${CMAKE_CURRENT_LIST_DIR}/DownloadNDK.cmake")

set(CMAKE_CXX_COMPILER "${GOF2_NDK_CLANGXX}")
set(CMAKE_CXX_COMPILER_ID Clang)
set(CMAKE_CXX_COMPILER_WORKS TRUE)     # skip CMake's try_compile probe (cross compiler)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_AR "${GOF2_NDK_AR}" CACHE FILEPATH "NDK llvm-ar" FORCE)

# Optimization level is the main byte-matching knob (the original was built -Oz). Override to A/B.
set(GOF2_MATCH_OPT "-Oz" CACHE STRING "Optimization level for the matching build (-Oz/-O2/-Os/...)")

# The canonical match flags. Toolchain: NDK r18b, clang 7.0.2, armeabi-v7a, libc++ -- the compiler
# the original .so's .comment names. -fPIC (not -fpic): the original .so has no R_ARM_REL32 text
# relocs / DT_TEXTREL and accesses globals GOT-indirect (R_ARM_GLOB_DAT), which is -fPIC codegen;
# -fpic emits R_ARM_REL32 the original lacks. API 21 only gates libc declarations (libc++ <cmath>
# needs bionic's C99 math; 16 fails to compile), not the recovered functions' codegen. All include
# paths are absolute (CMake compiles from the build dir).
set(_match_api 21)
set(CMAKE_CXX_FLAGS
    "-target armv7-none-linux-androideabi${_match_api} \
-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp \
-fPIC -frtti -fstack-protector ${GOF2_MATCH_OPT} \
-stdlib=libc++ \
-isystem ${GOF2_NDK_LIBCXX_INC} \
-isystem ${GOF2_NDK_LIBCXXABI_INC} \
-isystem ${GOF2_NDK_SUPPORT_INC} \
--sysroot=${GOF2_NDK_SYSROOT} \
-isystem ${GOF2_NDK_SYSROOT}/usr/include \
-isystem ${GOF2_NDK_SYSROOT}/usr/include/arm-linux-androideabi \
-D__ANDROID_API__=${_match_api} \
-Wno-int-to-pointer-cast -Wno-int-to-void-pointer-cast \
-I${_gof2_root}/src \
-I${_gof2_root}/third_party/fmod/inc \
-I${_gof2_root}/third_party/gl \
-I${_gof2_root}/third_party/jni \
-I${_gof2_root}/third_party/libzip \
-I${_gof2_root}/third_party/crypto"
    CACHE STRING "matching ARM compile flags" FORCE)
