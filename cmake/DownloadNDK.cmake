# DownloadNDK.cmake -- make the Android NDK r18b available to the build.
#
# The byte-matching ARM build is locked to NDK r18b (clang 7.0.2): that is the exact compiler the
# original libgof2hdaa.so was built with, so it cannot be swapped for a newer NDK without losing
# instruction-level parity. r18b is also the last NDK that still ships GNU binutils (the ARMv7
# objdump/nm/as the verifier needs) and a standalone ld.gold, alongside clang and libc++.
#
# CMake downloads + extracts the NDK once into a gitignored cache and resolves every tool/include
# path the toolchain and the Python verifier consume. The darwin-x86_64 package runs natively under
# Rosetta 2 on Apple Silicon -- this build targets that host only.
#
# Outputs (CACHE):
#   GOF2_NDK_ROOT
#   GOF2_NDK_CLANGXX  GOF2_NDK_CLANG  GOF2_NDK_AR  GOF2_NDK_GCC_BINDIR
#   GOF2_NDK_OBJDUMP  GOF2_NDK_NM  GOF2_NDK_AS  GOF2_NDK_GOLD
#   GOF2_NDK_SYSROOT  GOF2_NDK_LIBCXX_INC  GOF2_NDK_LIBCXXABI_INC  GOF2_NDK_SUPPORT_INC
#   GOF2_NDK_LIBCXXABI_SRC  GOF2_NDK_LIBCXXABI_A  GOF2_NDK_CRT_LIBDIR

include_guard(GLOBAL)

set(_ndk "android-ndk-r18b")
set(_host "darwin-x86_64")
set(_cache "${CMAKE_SOURCE_DIR}/.cache/ndk")
set(_root "${_cache}/${_ndk}")

# Sentinel: the libc++ headers only exist after a complete extraction.
if(NOT EXISTS "${_root}/sources/cxx-stl/llvm-libc++/include")
    set(_zip "${_cache}/${_ndk}-${_host}.zip")
    set(_url "https://dl.google.com/android/repository/${_ndk}-${_host}.zip")
    message(STATUS "Downloading ${_ndk}-${_host} (~500MB, one time) ...")
    file(DOWNLOAD "${_url}" "${_zip}" SHOW_PROGRESS STATUS _dl)
    list(GET _dl 0 _code)
    if(NOT _code EQUAL 0)
        list(GET _dl 1 _msg)
        file(REMOVE "${_zip}")
        message(FATAL_ERROR "NDK download failed: ${_msg}\n  URL: ${_url}")
    endif()
    message(STATUS "Extracting ${_ndk} ...")
    file(ARCHIVE_EXTRACT INPUT "${_zip}" DESTINATION "${_cache}")
    file(REMOVE "${_zip}")
    if(NOT EXISTS "${_root}/sources/cxx-stl/llvm-libc++/include")
        message(FATAL_ERROR "NDK extraction incomplete: ${_root} missing libc++ headers")
    endif()
endif()

set(GOF2_NDK_ROOT "${_root}" CACHE PATH "Android NDK r18b root" FORCE)

set(_llvm "${_root}/toolchains/llvm/prebuilt/${_host}")
set(_gcc  "${_root}/toolchains/arm-linux-androideabi-4.9/prebuilt/${_host}")

set(GOF2_NDK_CLANGXX    "${_llvm}/bin/clang++"                          CACHE FILEPATH "" FORCE)
set(GOF2_NDK_CLANG      "${_llvm}/bin/clang"                            CACHE FILEPATH "" FORCE)
set(GOF2_NDK_AR         "${_llvm}/bin/llvm-ar"                          CACHE FILEPATH "" FORCE)
set(GOF2_NDK_GCC_BINDIR "${_gcc}/arm-linux-androideabi/bin"            CACHE PATH "" FORCE)
set(GOF2_NDK_OBJDUMP    "${_gcc}/bin/arm-linux-androideabi-objdump"    CACHE FILEPATH "" FORCE)
set(GOF2_NDK_NM         "${_gcc}/bin/arm-linux-androideabi-nm"         CACHE FILEPATH "" FORCE)
set(GOF2_NDK_AS         "${_gcc}/bin/arm-linux-androideabi-as"         CACHE FILEPATH "" FORCE)
set(GOF2_NDK_GOLD       "${_gcc}/arm-linux-androideabi/bin/ld.gold"    CACHE FILEPATH "" FORCE)
set(GOF2_NDK_STRIP      "${_gcc}/bin/arm-linux-androideabi-strip"      CACHE FILEPATH "" FORCE)

set(GOF2_NDK_SYSROOT       "${_root}/sysroot"                                        CACHE PATH "" FORCE)
set(GOF2_NDK_LIBCXX_INC    "${_root}/sources/cxx-stl/llvm-libc++/include"            CACHE PATH "" FORCE)
set(GOF2_NDK_LIBCXXABI_INC "${_root}/sources/cxx-stl/llvm-libc++abi/include"         CACHE PATH "" FORCE)
set(GOF2_NDK_SUPPORT_INC   "${_root}/sources/android/support/include"               CACHE PATH "" FORCE)
set(GOF2_NDK_LIBCXXABI_SRC "${_root}/sources/cxx-stl/llvm-libc++abi/src"             CACHE PATH "" FORCE)
set(GOF2_NDK_LIBCXXABI_A   "${_root}/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a/libc++abi.a" CACHE FILEPATH "" FORCE)
set(GOF2_NDK_CRT_LIBDIR    "${_root}/platforms/android-16/arch-arm/usr/lib"          CACHE PATH "" FORCE)

message(STATUS "NDK r18b: ${GOF2_NDK_ROOT}")
