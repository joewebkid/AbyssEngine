#!/usr/bin/env bash
# One-time provisioning for the matching ASM-validation build. Idempotent: safe to
# re-run. Everything the matching toolchain needs lives in the OrbStack `ubuntu`
# machine (the NDK r18b clang only runs on Linux; OrbStack mirrors the macOS
# filesystem 1:1 so build outputs land back on the Mac).
set -euo pipefail
MACHINE="${GOF2_ORB_MACHINE:-ubuntu}"
NDK="${NDK:-/opt/android-ndk-r18b}"
NDK_URL="https://dl.google.com/android/repository/android-ndk-r18b-linux-x86_64.zip"

say() { printf '\033[1;36m[setup]\033[0m %s\n' "$*"; }

command -v orb >/dev/null || { echo "orb (OrbStack CLI) not found on PATH"; exit 1; }
command -v python3 >/dev/null || { echo "python3 not found on PATH"; exit 1; }

say "checking OrbStack machine '$MACHINE'..."
orb -m "$MACHINE" true 2>/dev/null || {
  echo "OrbStack machine '$MACHINE' is not running. Start it with: orb start $MACHINE"
  exit 1
}

say "checking ARM binutils in '$MACHINE'..."
if ! orb -m "$MACHINE" command -v arm-linux-gnueabihf-objdump >/dev/null 2>&1; then
  say "installing binutils-arm-linux-gnueabihf (needs sudo in the machine)..."
  orb -m "$MACHINE" sudo apt-get update -qq
  orb -m "$MACHINE" sudo apt-get install -y -qq binutils-arm-linux-gnueabihf
fi

say "checking full NDK r18b at $NDK (sysroot + libc++)..."
if ! orb -m "$MACHINE" test -d "$NDK/sysroot" \
   || ! orb -m "$MACHINE" test -d "$NDK/sources/cxx-stl/llvm-libc++/include"; then
  say "downloading + extracting NDK r18b (~512MB, one time)..."
  orb -m "$MACHINE" bash -c "
    set -e
    cd /opt 2>/dev/null || { sudo mkdir -p /opt && cd /opt; }
    curl -fsSL -o /tmp/ndk-r18b.zip '$NDK_URL'
    rm -rf /tmp/ndk-extract && unzip -q /tmp/ndk-r18b.zip -d /tmp/ndk-extract
    sudo rm -rf '$NDK'
    sudo mv /tmp/ndk-extract/android-ndk-r18b '$NDK'
    rm -rf /tmp/ndk-extract /tmp/ndk-r18b.zip
  "
fi

say "verifying the matching compiler runs..."
"$(dirname "$0")/orbcc" --version | head -1

say "OK. Next:  cmake --preset match  &&  cmake --build cmake-build-match --target verify"
