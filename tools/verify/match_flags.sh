# Canonical compiler flags for the matching ARM build (sourced by build_objs.sh
# and read by cmake/orbstack-ndk-arm.toolchain.cmake — keep this the ONE source).
#
# Toolchain: Android NDK r18b, clang 7.0.2, armeabi-v7a, libc++ (from the .so's
# .comment). These are the STARTING POINT for byte-matching; tune per the verify
# report. The big knob is GOF2_MATCH_OPT (opt level): ROADMAP found -Oz; the old
# flags.sh claimed -O2. Override it from the environment to A/B test.
#
# Paths below are resolved in the active backend: OrbStack ubuntu when `orb` is
# available, or the local NDK fallback when GOF2_VERIFY_LOCAL_NDK=1 is set.

HERE="${BASH_SOURCE[0]%/*}"
HERE="$(cd "$HERE" && pwd)"
REPO="$(cd "$HERE/../.." && pwd)"

if [ -z "${NDK:-}" ]; then
  NDK="${GOF2_NDK_ROOT:-}"
fi
if [ -z "${NDK:-}" ]; then
  if [ -d "$REPO/.cache/ndk/android-ndk-r18b" ]; then
    NDK="$REPO/.cache/ndk/android-ndk-r18b"
  else
    NDK="/opt/android-ndk-r18b"
  fi
fi
: "${GOF2_MATCH_OPT:=-Oz}"
# Min API only gates libc declarations, not the recovered functions' codegen; 21
# is needed so libc++ <cmath> finds bionic's C99 math (16 fails to compile).
: "${GOF2_MATCH_API:=21}"

# One argument per line. build_objs.sh turns this back into a Bash array before
# invoking clang, so a local NDK/repository path with spaces stays one argument.
GOF2_MATCH_CXXFLAGS="$(printf '%s\n' \
  -target \
  "armv7-none-linux-androideabi${GOF2_MATCH_API}" \
  -march=armv7-a \
  -mthumb \
  -mfpu=neon \
  -mfloat-abi=softfp \
  -fpic \
  -frtti \
  -fstack-protector \
  "${GOF2_MATCH_OPT}" \
  -stdlib=libc++ \
  -isystem \
  "${NDK}/sources/cxx-stl/llvm-libc++/include" \
  -isystem \
  "${NDK}/sources/cxx-stl/llvm-libc++abi/include" \
  -isystem \
  "${NDK}/sources/android/support/include" \
  "--sysroot=${NDK}/sysroot" \
  -isystem \
  "${NDK}/sysroot/usr/include" \
  -isystem \
  "${NDK}/sysroot/usr/include/arm-linux-androideabi" \
  "-D__ANDROID_API__=${GOF2_MATCH_API}" \
  -Wno-int-to-pointer-cast \
  -Wno-int-to-void-pointer-cast \
  -Isrc \
  -Ithird_party/fmod/inc \
  -Ithird_party/gl \
  -Ithird_party/jni \
  -Ithird_party/libzip \
  -Ithird_party/crypto)"

export GOF2_MATCH_CXXFLAGS
