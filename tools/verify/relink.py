#!/usr/bin/env python3
"""relink.py - assemble the recovered match-build objects into libgof2hdaa.so.

Links every match-build object (cmake-build-match/verify/base/**/*.o, recursively)
into a shared object using the NDK r18b GNU gold linker, run *inside OrbStack* (the
match toolchain is Linux-only, same as tools/verify/orbcc). Produces a .so suitable
for whole-binary symbol comparison via tools/symdiff.py.

Handles the three real link blockers found in this tree:
  * objects that emit R_ARM_REL32 under -fpic are recompiled -fPIC (to a temp dir),
  * recovered_a3cc4.o (hand-asm loadAPK/loadAPKAndZip that references C-name
    gZipMain/gZipPatch) is dropped -- ApkLoader.cpp supersedes it with the C++
    FileInterfaceAndroid::gZipMain statics,
  * crtbegin_so.o / crtend_so.o supply __dso_handle + init/fini.
FMOD and other app-provided imports stay undefined (--unresolved-symbols=ignore-all),
exactly as in the original .so.

After a successful link, run:  python3 tools/symdiff.py
"""
import os, sys, glob, subprocess, tempfile, re

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BASE = os.path.join(ROOT, "cmake-build-match", "verify", "base")
OUT = os.path.join(ROOT, "cmake-build-match", "libgof2hdaa.so")
NDK = os.environ.get("NDK", "/opt/android-ndk-r18b")
MACHINE = os.environ.get("GOF2_ORB_MACHINE", "ubuntu")
GOLD = NDK + "/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/arm-linux-androideabi/bin/ld.gold"
LIBDIR = NDK + "/platforms/android-16/arch-arm/usr/lib"
SONAME = "libgof2hdaa.so"
NEEDED = ["log", "GLESv2", "GLESv1_CM", "EGL", "android", "m", "dl", "c"]
# Static C++ ABI/runtime the original statically linked (provides std::bad_*,
# type_info, __cxa_*, terminate/unexpected/new_handler). Linked as an archive so
# only the objects our code references are pulled in (matching the original subset).
CXX_RUNTIME = [NDK + "/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a/libc++abi.a"]
# Vendored OpenSSL 1.0.2 libcrypto subset (SHA-224/256, CRYPTO_memcmp, OPENSSL_cleanse,
# ARMv7/ARMv8 cpuid probes) -- the game statically linked these from libcrypto.
# Built by third_party/openssl/build.sh. Linked as an archive (only referenced objects pulled).
CRYPTO_LIB = [os.path.join(ROOT, "third_party", "openssl", "libcrypto_gof2.a")]
# Vendored zlib 1.2.7 + libzip 0.9.3 subsets -- the game statically linked these to
# read its APK/.zip asset archives (deflate/inflate/crc32 + zip_open/zip_fread/...).
# Built by third_party/zlib/build.sh and third_party/libzip/build.sh. libzip references
# zlib, so libzip must precede zlib on the link line. Archives -> only referenced objects.
EXTRA_LIBS = [os.path.join(ROOT, "third_party", "libzip", "libzip_gof2.a"),
              os.path.join(ROOT, "third_party", "zlib", "libz_gof2.a")]
# objects that duplicate a C++ TU and pull in undefined C-name globals
DROP = {"recovered_a3cc4.o"}


def orb(*cmd):
    return subprocess.run(["orb", "-m", MACHINE, *cmd], capture_output=True, text=True)


def link(objs):
    cmd = [GOLD, "-shared", "-soname", SONAME, "--build-id",
           "--unresolved-symbols=ignore-all", "--allow-multiple-definition",
           os.path.join(LIBDIR, "crtbegin_so.o"), "-L", LIBDIR]
    for n in NEEDED:
        cmd += ["-l", n]
    cmd += objs
    cmd += [p for p in CRYPTO_LIB if os.path.exists(p)]
    # The original .so exports every zlib/libzip symbol (the whole library was linked,
    # not just the members our decompiled code happens to reference). --whole-archive
    # pulls all objects so the full public+internal symbol set is defined and exported.
    extra = [p for p in EXTRA_LIBS if os.path.exists(p)]
    if extra:
        cmd += ["--whole-archive", *extra, "--no-whole-archive"]
    cmd += CXX_RUNTIME
    cmd += [os.path.join(LIBDIR, "crtend_so.o"), "-o", OUT]
    return orb(*cmd)


def main():
    objs = [p for p in sorted(glob.glob(os.path.join(BASE, "**", "*.o"), recursive=True))
            if os.path.basename(p) not in DROP]
    print(f"[relink] objects: {len(objs)} (recursive, minus {len(DROP)} superseded)")
    if not objs:
        print("[relink] BLOCKED: no objects. Run: python3 tools/verify/verify.py")
        return 2
    # First attempt; if gold reports objects needing -fPIC, rebuild those -fPIC and retry.
    for attempt in range(3):
        r = link(objs)
        if r.returncode == 0:
            print(f"[relink] wrote {os.path.relpath(OUT, ROOT)}\n[relink] now run: python3 tools/symdiff.py")
            return 0
        rel32 = sorted(set(re.findall(r'(\S+\.o): requires unsupported dynamic reloc R_ARM_REL32', r.stderr)))
        if not rel32:
            print("[relink] link failed:\n" + "\n".join(r.stderr.splitlines()[:20]))
            return 1
        print(f"[relink] {len(rel32)} object(s) need -fPIC; rebuilding: {[os.path.basename(x) for x in rel32]}")
        flags = subprocess.run([". tools/verify/match_flags.sh; echo $GOF2_MATCH_CXXFLAGS"],
                               shell=True, cwd=ROOT, capture_output=True, text=True).stdout.split()
        tmp = tempfile.mkdtemp(prefix="relink_pic_")
        fixed = {}
        for o in rel32:
            src = guess_src(o)
            if not src:
                print(f"[relink] cannot map {o} to a source; aborting"); return 1
            po = os.path.join(tmp, os.path.basename(o))
            cc = subprocess.run(["tools/verify/orbcc", *flags, "-fPIC", "-c", src, "-o", po],
                                cwd=ROOT, capture_output=True, text=True)
            if cc.returncode != 0:
                print(f"[relink] -fPIC rebuild of {src} failed:\n{cc.stderr[:400]}"); return 1
            fixed[os.path.abspath(o)] = po
        objs = [fixed.get(os.path.abspath(o), o) for o in objs]
    print("[relink] still failing after PIC retries"); return 1


def guess_src(obj):
    # base/<rel>.o  -> src/<rel>.cpp
    rel = os.path.relpath(os.path.abspath(obj), BASE)
    cand = os.path.join(ROOT, "src", rel[:-2] + ".cpp")
    return cand if os.path.exists(cand) else None


if __name__ == "__main__":
    sys.exit(main())
