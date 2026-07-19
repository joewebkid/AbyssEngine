# build_match_lib.cmake -- CMake-native builder for one vendored static lib (run via `cmake -P`).
#
# Replaces third_party/{openssl,zlib,libzip}/build.sh. Builds the exact subset of
# libcrypto/zlib/libzip the original game statically linked, for armeabi-v7a with the
# downloaded NDK r18b C clang, into third_party/<lib>/lib<...>_gof2.a -- which the
# CMake `relink` target links into the verification libgof2hdaa.so.
#
# Invoked by third_party/match_libs.cmake with -D variables:
#   GOF2_LIB              openssl | zlib | libzip
#   GOF2_NDK_ROOT GOF2_NDK_CLANG GOF2_NDK_AR GOF2_NDK_GCC_BINDIR
#   TP_DIR                absolute path of third_party/
#
# The source trees are downloaded + extracted on first use (pinned versions/URLs below);
# the hand-written helpers (openssl/crypto_memcmp.c, libzip lib/config.h) are committed.

cmake_minimum_required(VERSION 3.20)

set(SYSROOT "${GOF2_NDK_ROOT}/platforms/android-16/arch-arm")

# Common C flags shared by all three libs.
set(CC_BASE
    --target=armv7a-linux-androideabi16 -mfpu=neon -mfloat-abi=softfp
    --sysroot=${SYSROOT}
    -isystem ${GOF2_NDK_ROOT}/sysroot/usr/include
    -isystem ${GOF2_NDK_ROOT}/sysroot/usr/include/arm-linux-androideabi
    -fPIC -Wno-everything)

# ---- helpers -----------------------------------------------------------------------------
function(ensure_source dir url)
    if(NOT EXISTS "${dir}")
        get_filename_component(_parent "${dir}" DIRECTORY)
        get_filename_component(_name "${url}" NAME)
        set(_tar "${_parent}/${_name}")
        message(STATUS "[${GOF2_LIB}] fetching ${url}")
        file(DOWNLOAD "${url}" "${_tar}" STATUS _st)
        list(GET _st 0 _code)
        if(NOT _code EQUAL 0)
            list(GET _st 1 _msg)
            file(REMOVE "${_tar}")
            message(FATAL_ERROR "[${GOF2_LIB}] download failed: ${_msg}")
        endif()
        file(ARCHIVE_EXTRACT INPUT "${_tar}" DESTINATION "${_parent}")
        file(REMOVE "${_tar}")
    endif()
endfunction()

# compile(<out.o> <workdir> <extra-flags-list> <src>) -- run the NDK C clang.
function(compile out workdir extra src)
    execute_process(
        COMMAND ${GOF2_NDK_CLANG} ${CC_BASE} ${extra} -c ${src} -o ${out}
        WORKING_DIRECTORY "${workdir}" RESULT_VARIABLE _rc)
    if(NOT _rc EQUAL 0)
        message(FATAL_ERROR "[${GOF2_LIB}] compile failed: ${src}")
    endif()
endfunction()

function(archive out)                                          # archive(<out.a> <obj>...)
    file(REMOVE "${out}")                                      # rcs replaces but never removes
    execute_process(COMMAND ${GOF2_NDK_AR} rcs ${out} ${ARGN} RESULT_VARIABLE _rc)
    if(NOT _rc EQUAL 0)
        message(FATAL_ERROR "[${GOF2_LIB}] ar failed: ${out}")
    endif()
endfunction()

# ==========================================================================================
if(GOF2_LIB STREQUAL "zlib")
    # zlib 1.2.3 -- the public ABI the original exports (adler32/crc32_combine but no *_combine64,
    # no gz*). Flat .c set, no configure needed; the gz file-I/O layer is omitted (no gz* symbols).
    set(SRC "${TP_DIR}/zlib/zlib-1.2.3")
    ensure_source("${SRC}" "https://zlib.net/fossils/zlib-1.2.3.tar.gz")
    set(OBJDIR "${SRC}/_obj_gof2")
    file(MAKE_DIRECTORY "${OBJDIR}")
    set(units adler32 compress crc32 deflate infback inffast inflate inftrees trees uncompr zutil)
    set(objs "")
    foreach(u ${units})
        compile("${OBJDIR}/${u}.o" "${SRC}" "-O2;-I." "${SRC}/${u}.c")
        list(APPEND objs "${OBJDIR}/${u}.o")
    endforeach()
    archive("${TP_DIR}/zlib/libz_gof2.a" ${objs})

# ==========================================================================================
elseif(GOF2_LIB STREQUAL "libzip")
    # libzip 0.9.3 -- bracketed from the original's symbol set (pre-0.11 zip_add names,
    # _zip_err_str[] ends at ZIP_ER_DELETED). Needs zlib headers; relink links libzip before zlib.
    set(SRC "${TP_DIR}/libzip/libzip-0.9.3")
    set(ZSRC "${TP_DIR}/zlib/zlib-1.2.3")
    ensure_source("${ZSRC}" "https://zlib.net/fossils/zlib-1.2.3.tar.gz")
    ensure_source("${SRC}" "https://libzip.org/download/libzip-0.9.3.tar.gz")
    # Minimal config.h matching the Android (NDK r18b / API 16) feature set; replaces the
    # autotools-generated header. HAVE_MKSTEMP is intentionally unset so zip_replace.c uses
    # libzip's bundled _zip_mkstemp (which the original .so exports).
    file(WRITE "${SRC}/lib/config.h"
"/* Hand-written config.h for the GOF2 armeabi-v7a build (NDK r18b / API 16). */
#define HAVE_DLFCN_H 1
#define HAVE_FSEEKO 1
#define HAVE_FTELLO 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIBZ 1
#define HAVE_MEMORY_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define STDC_HEADERS 1
#define PACKAGE \"libzip\"
#define VERSION \"0.9.3\"
#define PACKAGE_VERSION \"0.9.3\"
")
    set(OBJDIR "${SRC}/lib/_obj_gof2")
    file(MAKE_DIRECTORY "${OBJDIR}")
    file(GLOB _csrc "${SRC}/lib/zip_*.c")
    list(APPEND _csrc "${SRC}/lib/mkstemp.c")
    set(objs "")
    foreach(c ${_csrc})
        get_filename_component(_n "${c}" NAME_WE)
        compile("${OBJDIR}/${_n}.o" "${SRC}/lib" "-O2;-I.;-I${ZSRC}" "${c}")
        list(APPEND objs "${OBJDIR}/${_n}.o")
    endforeach()
    archive("${TP_DIR}/libzip/libzip_gof2.a" ${objs})

# ==========================================================================================
elseif(GOF2_LIB STREQUAL "openssl")
    # OpenSSL 1.0.2 libcrypto subset: SHA-224/256 (+ ARMv4 asm), CRYPTO_memcmp, OPENSSL_cleanse,
    # ARMv7/ARMv8 cpuid probes -- the symbols the game statically linked from libcrypto.
    set(SRC "${TP_DIR}/openssl/openssl-1.0.2u")
    ensure_source("${SRC}" "https://www.openssl.org/source/openssl-1.0.2u.tar.gz")
    # Configure once to generate opensslconf.h / buildinf.h. Invoke through `perl` explicitly:
    # OpenSSL's Configure is a `:`/`eval exec perl` self-execing script with no #! shebang, which
    # posix_spawn (execute_process) cannot exec directly -- only a shell/perl can launch it.
    if(NOT EXISTS "${SRC}/Makefile")
        find_program(PERL_EXECUTABLE perl REQUIRED)
        execute_process(COMMAND ${PERL_EXECUTABLE} Configure android-armv7 no-shared
                        WORKING_DIRECTORY "${SRC}"
                        OUTPUT_QUIET ERROR_QUIET RESULT_VARIABLE _rc)
        if(NOT _rc EQUAL 0)
            message(FATAL_ERROR "[openssl] perl Configure android-armv7 failed")
        endif()
    endif()
    set(OBJDIR "${SRC}/_obj_gof2")
    file(MAKE_DIRECTORY "${OBJDIR}")
    set(OSSL_INC "-O3;-I.;-Icrypto;-Iinclude;-Icrypto/sha")
    # .S cpuid probe + SHA-256 asm assemble only in ARM mode; sha256-armv4.S's `adrl` needs GNU as.
    compile("${OBJDIR}/armv4cpuid.o"   "${SRC}" "${OSSL_INC};-marm"  "${SRC}/crypto/armv4cpuid.S")
    compile("${OBJDIR}/sha256.o"       "${SRC}" "${OSSL_INC};-mthumb;-DSHA256_ASM" "${SRC}/crypto/sha/sha256.c")
    compile("${OBJDIR}/sha256-armv4.o" "${SRC}" "${OSSL_INC};-marm;-fno-integrated-as;-B${GOF2_NDK_GCC_BINDIR}" "${SRC}/crypto/sha/sha256-armv4.S")
    compile("${OBJDIR}/crypto_memcmp.o" "${SRC}" "${OSSL_INC};-mthumb" "${TP_DIR}/openssl/crypto_memcmp.c")
    compile("${OBJDIR}/armcap.o"       "${SRC}" "${OSSL_INC};-mthumb" "${SRC}/crypto/armcap.c")
    archive("${TP_DIR}/openssl/libcrypto_gof2.a"
        "${OBJDIR}/armv4cpuid.o" "${OBJDIR}/sha256.o" "${OBJDIR}/sha256-armv4.o"
        "${OBJDIR}/crypto_memcmp.o" "${OBJDIR}/armcap.o")

else()
    message(FATAL_ERROR "unknown GOF2_LIB '${GOF2_LIB}'")
endif()

message(STATUS "[${GOF2_LIB}] built lib for the match link")
