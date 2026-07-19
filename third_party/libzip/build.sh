#!/usr/bin/env bash
# Build the libzip subset the game statically linked, for armeabi-v7a with the
# NDK r18b clang. Produces third_party/libzip/libzip_gof2.a, which
# tools/verify/relink.py links into libgof2hdaa.so to satisfy the public
# zip_open/zip_fopen/zip_fread/zip_stat/zip_source_* and internal _zip_* symbols.
#
# Version: libzip 0.9.3  -- bracketed from the original's symbol set (see
# third_party/libzip/README.md): the mutation API still uses the pre-0.11 names
# zip_add / zip_add_dir / zip_get_num_files, and the _zip_err_str[] table ends at
# ZIP_ER_DELETED (23) -- ZIP_ER_ENCRNOTSUPP (24, added in 0.10) is absent, pinning
# it to the 0.9 series (latest 0.9.3, 2010). Confirmed: 0.9.3 defines every missing
# _zip_* / zip_* symbol including _zip_cdir_grow, _zip_dirent_torrent_normalize.
#
# Depends on the vendored zlib (third_party/zlib) for <zlib.h>/<zconf.h>; run
# third_party/zlib/build.sh first. libzip 0.9.3 normally configures via autotools;
# we instead supply a minimal config.h with the Android (NDK r18b / API 16) feature
# set and compile the .c files directly -- same approach as the OpenSSL build.
#
# Runs inside OrbStack (the NDK toolchain is Linux-only).
# Requires the source tree at third_party/libzip/libzip-0.9.3 (fetched below if absent).
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
VER=libzip-0.9.3
SRC="$HERE/$VER"
ZLIBSRC="$HERE/../zlib/zlib-1.2.3"

if [ ! -d "$ZLIBSRC" ]; then
  echo "[libzip] zlib source not found at $ZLIBSRC -- run third_party/zlib/build.sh first" >&2
  exit 1
fi

if [ ! -d "$SRC" ]; then
  echo "[libzip] fetching $VER source ..."
  curl -sL -o "$HERE/$VER.tar.gz" "https://libzip.org/download/$VER.tar.gz"
  tar xzf "$HERE/$VER.tar.gz" -C "$HERE/"
  rm -f "$HERE/$VER.tar.gz"
fi

# Minimal config.h matching the Android (NDK r18b, API 16) feature set. The 0.9.3
# autoconf would detect all of these on a modern Linux/Android target.
cat > "$SRC/lib/config.h" <<'EOF'
/* Hand-written config.h for the GOF2 armeabi-v7a build (NDK r18b / API 16).
   Replaces the autotools-generated header; values reflect Android's libc. */
#define HAVE_DLFCN_H 1
#define HAVE_FSEEKO 1
#define HAVE_FTELLO 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIBZ 1
#define HAVE_MEMORY_H 1
/* HAVE_MKSTEMP intentionally unset: the original .so exports _zip_mkstemp, so its
   build used libzip's bundled mkstemp.c fallback (zipint.h #defines mkstemp ->
   _zip_mkstemp when HAVE_MKSTEMP is absent). */
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define STDC_HEADERS 1
#define PACKAGE "libzip"
#define VERSION "0.9.3"
#define PACKAGE_VERSION "0.9.3"
EOF

orb -m "${GOF2_ORB_MACHINE:-ubuntu}" bash -c '
set -e
cd "'"$SRC"'/lib"
NDK=/opt/android-ndk-r18b; TC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin
SYSROOT=$NDK/platforms/android-16/arch-arm
CC="$TC/clang --target=armv7a-linux-androideabi16 -mfpu=neon -mfloat-abi=softfp \
  --sysroot=$SYSROOT -isystem $NDK/sysroot/usr/include \
  -isystem $NDK/sysroot/usr/include/arm-linux-androideabi \
  -O2 -fPIC -Wno-everything -I. -I'"$ZLIBSRC"'"
OBJ=$(mktemp -d)
# Every libzip .c plus mkstemp.c (HAVE_MKSTEMP is unset, so zip_replace.c calls
# the bundled _zip_mkstemp -- which the original .so exports). The archive lets
# relink pull only the referenced objects.
for f in zip_*.c mkstemp.c; do
  $CC -c "$f" -o "$OBJ/${f%.c}.o"
done
rm -f "'"$HERE"'/libzip_gof2.a"   # rcs adds/replaces but never removes; start clean
$TC/llvm-ar rcs "'"$HERE"'/libzip_gof2.a" $OBJ/*.o
rm -rf $OBJ
'
echo "[libzip] wrote third_party/libzip/libzip_gof2.a"
