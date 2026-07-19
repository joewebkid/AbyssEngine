#!/usr/bin/env bash
# Build the zlib subset the game statically linked, for armeabi-v7a with the
# NDK r18b clang. Produces third_party/zlib/libz_gof2.a, which
# tools/verify/relink.py links into libgof2hdaa.so to satisfy the
# deflate/inflate/crc32/adler32/compress/gz* and internal _tr_*/_dist_code/
# _length_code/inflate_fast/inflate_table symbols.
#
# Version: zlib 1.2.3  -- confirmed by the original .so's exported symbol set:
# it has adler32_combine/crc32_combine (added 1.2.2.1) but NOT the *_combine64,
# inflateMark, inflateReset2/ResetKeep, or any gz* symbols (all added in 1.2.4+).
# That is exactly the 1.2.3 public ABI. Its own embedded strings read
# "deflate 1.2.3"/"inflate 1.2.3" (the zlibVersion, not just a format string).
# The original linked zlib WITHOUT the gz file-I/O layer (no gz* symbols at all),
# so we drop gzio.c -- the game only uses the in-memory compress/uncompress +
# deflate/inflate paths (libzip reads archives via its own buffer/file sources).
#
# Runs inside OrbStack (the NDK toolchain is Linux-only), same as the OpenSSL build.
# Requires the source tree at third_party/zlib/zlib-1.2.3 (fetched below if absent).
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
VER=zlib-1.2.3
SRC="$HERE/$VER"

if [ ! -d "$SRC" ]; then
  echo "[zlib] fetching $VER source ..."
  curl -sL -o "$HERE/$VER.tar.gz" "https://zlib.net/fossils/$VER.tar.gz"
  tar xzf "$HERE/$VER.tar.gz" -C "$HERE/"
  rm -f "$HERE/$VER.tar.gz"
fi

orb -m "${GOF2_ORB_MACHINE:-ubuntu}" bash -c '
set -e
cd "'"$SRC"'"
NDK=/opt/android-ndk-r18b; TC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin
SYSROOT=$NDK/platforms/android-16/arch-arm
CC="$TC/clang --target=armv7a-linux-androideabi16 -mfpu=neon -mfloat-abi=softfp \
  --sysroot=$SYSROOT -isystem $NDK/sysroot/usr/include \
  -isystem $NDK/sysroot/usr/include/arm-linux-androideabi \
  -O2 -fPIC -Wno-everything -I."
OBJ=$(mktemp -d)
# zlib 1.2.3 is a flat set of .c files with no platform configure needed for our
# subset (zconf.h ships ready-to-use in the release tarball). gzio.c (the gz
# file-I/O layer) is deliberately omitted -- the original .so exports no gz*
# symbols. example.c/minigzip.c are demo programs, also omitted.
SRCS="adler32 compress crc32 deflate infback inffast inflate inftrees trees \
      uncompr zutil"
for s in $SRCS; do
  $CC -c "$s.c" -o "$OBJ/$s.o"
done
rm -f "'"$HERE"'/libz_gof2.a"   # rcs adds/replaces but never removes; start clean
$TC/llvm-ar rcs "'"$HERE"'/libz_gof2.a" $OBJ/*.o
rm -rf $OBJ
'
echo "[zlib] wrote third_party/zlib/libz_gof2.a"
