#!/usr/bin/env bash
# Build the OpenSSL 1.0.2 libcrypto subset the game statically linked, for armeabi-v7a
# with the NDK r18b clang. Produces third_party/openssl/libcrypto_gof2.a, which
# tools/verify/relink.py links into libgof2hdaa.so to satisfy the SHA-224/256,
# CRYPTO_memcmp, OPENSSL_cleanse and ARMv7/ARMv8 cpuid-probe symbols.
#
# Version: OpenSSL 1.0.2u  (the 1.0.2 series is identified by the ARMv8 crypto
# probes _armv8_sha256_probe/_armv8_aes_probe + OPENSSL_instrument_bus2 present in
# the original .so; no version string is embedded since it was statically linked).
#
# Runs inside OrbStack (the NDK toolchain is Linux-only), same as tools/verify/orbcc.
# Requires the source tree at third_party/openssl/openssl-1.0.2u (fetched below if absent).
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRC="$HERE/openssl-1.0.2u"
VER=openssl-1.0.2u

if [ ! -d "$SRC" ]; then
  echo "[openssl] fetching $VER source ..."
  curl -sL -o "$HERE/$VER.tar.gz" "https://www.openssl.org/source/$VER.tar.gz"
  tar xzf "$HERE/$VER.tar.gz" -C "$HERE/"
  rm -f "$HERE/$VER.tar.gz"
fi

orb -m "${GOF2_ORB_MACHINE:-ubuntu}" bash -c '
set -e
cd "'"$SRC"'"
NDK=/opt/android-ndk-r18b; TC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin
SYSROOT=$NDK/platforms/android-16/arch-arm
# Configure once (generates opensslconf.h / buildinf.h); ignore if already done.
[ -f Makefile ] || ./Configure android-armv7 no-shared >/dev/null 2>&1
CC="$TC/clang --target=armv7a-linux-androideabi16 -mfpu=neon -mfloat-abi=softfp \
  --sysroot=$SYSROOT -isystem $NDK/sysroot/usr/include \
  -isystem $NDK/sysroot/usr/include/arm-linux-androideabi \
  -O3 -fPIC -Wno-everything -I. -Icrypto -Iinclude -Icrypto/sha"
OBJ=$(mktemp -d)
# .S cpuid probes assemble only in ARM mode (the OpenSSL asm is ARM, not Thumb).
$CC -marm  -c crypto/armv4cpuid.S -o $OBJ/armv4cpuid.o
# SHA-256: the original used the OpenSSL ARMv4 assembly (it exports both
# sha256_block_data_order and sha256_block_data_order_neon). Build the real .S
# (its `adrl` needs GNU as, so -fno-integrated-as) and compile sha256.c with
# -DSHA256_ASM so it calls the external asm block function instead of its own.
$CC -mthumb -DSHA256_ASM -c crypto/sha/sha256.c -o $OBJ/sha256.o
GAS=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/arm-linux-androideabi/bin
$CC -marm -fno-integrated-as -B$GAS -c crypto/sha/sha256-armv4.S -o $OBJ/sha256-armv4.o
# CRYPTO_memcmp only (isolated TU so the archive does not also pull cryptlib.c
# other ~35 CRYPTO_*/OPENSSL_* symbols the original does not export).
$CC -mthumb -c "'"$HERE"'/crypto_memcmp.c" -o $OBJ/crypto_memcmp.o
$CC -mthumb -c crypto/armcap.c     -o $OBJ/armcap.o     # OPENSSL_cpuid_setup
$TC/llvm-ar rcs "'"$HERE"'/libcrypto_gof2.a" \
  $OBJ/armv4cpuid.o $OBJ/sha256.o $OBJ/sha256-armv4.o $OBJ/crypto_memcmp.o $OBJ/armcap.o
rm -rf $OBJ
'
echo "[openssl] wrote third_party/openssl/libcrypto_gof2.a"
