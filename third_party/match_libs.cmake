# match_libs.cmake -- build the vendored static libs the original statically linked, with the
# downloaded NDK clang, for the whole-.so relink/symbol-parity gate (the `relink` target).
#
# Each lib is built by a `cmake -P` call to third_party/build_match_lib.cmake (CMake-native; no
# shell). The aggregate target `gof2_libs` is wired as a relink dependency. Only needed for
# `relink` (whole-binary symbol comparison), not for per-function `verify`.

include_guard(GLOBAL)

set(_tp "${CMAKE_SOURCE_DIR}/third_party")
set(_builder "${_tp}/build_match_lib.cmake")

# Shared -D arguments handing the resolved NDK toolchain to the builder script.
set(_match_lib_defs
    -DGOF2_NDK_ROOT=${GOF2_NDK_ROOT}
    -DGOF2_NDK_CLANG=${GOF2_NDK_CLANG}
    -DGOF2_NDK_AR=${GOF2_NDK_AR}
    -DGOF2_NDK_GCC_BINDIR=${GOF2_NDK_GCC_BINDIR}
    -DTP_DIR=${_tp})

# lib name -> output archive the relink link line consumes.
set(_zlib_out   "${_tp}/zlib/libz_gof2.a")
set(_libzip_out "${_tp}/libzip/libzip_gof2.a")
set(_openssl_out "${_tp}/openssl/libcrypto_gof2.a")

add_custom_command(OUTPUT "${_zlib_out}"
    COMMAND ${CMAKE_COMMAND} -DGOF2_LIB=zlib ${_match_lib_defs} -P ${_builder}
    COMMENT "Building vendored zlib 1.2.3 subset (armeabi-v7a)" VERBATIM)
add_custom_command(OUTPUT "${_libzip_out}"
    COMMAND ${CMAKE_COMMAND} -DGOF2_LIB=libzip ${_match_lib_defs} -P ${_builder}
    DEPENDS "${_zlib_out}"
    COMMENT "Building vendored libzip 0.9.3 subset (armeabi-v7a)" VERBATIM)
add_custom_command(OUTPUT "${_openssl_out}"
    COMMAND ${CMAKE_COMMAND} -DGOF2_LIB=openssl ${_match_lib_defs} -P ${_builder}
    COMMENT "Building vendored libcrypto 1.0.2 subset (armeabi-v7a)" VERBATIM)

add_custom_target(gof2_libs DEPENDS "${_zlib_out}" "${_libzip_out}" "${_openssl_out}")
