"""Exercise source-extracted Android archive and filesystem read paths."""

import argparse
import hashlib
from pathlib import Path
import subprocess
import tempfile
import zipfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--apk", type=Path)
    parser.add_argument("--obb", type=Path)
    args = parser.parse_args()
    if bool(args.apk) != bool(args.obb):
        parser.error("--apk and --obb must be supplied together")
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)

    source = (ROOT / "src/engine/file/FileInterfaceAndroid.cpp").read_text()
    loader = (ROOT / "src/platform/android/ApkLoader.cpp").read_text()
    engine = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    state = source[source.index("static void *gApkArchive ="):
                   source.index("uint32_t FileInterfaceAndroid::FileExist(")]
    read_state = source[source.index("static void **gZipMainR ="):
                        source.index("void *FileInterfaceAndroid::OpenRead(")]
    (out / "file_android_readexist_under_test.inc").write_text(
        "namespace AbyssEngine {\n"
        + function(engine, "String operator+(const String &a, const String &b) {")
        + "\n}\n"
        + function(source, "static inline const unsigned short *GetAEWChar(const String &s) {")
        + "static int *gFIAInstCount = &FileInterfaceAndroid::fileCounter;\n"
        + function(source, "FileInterfaceAndroid::FileInterfaceAndroid(FILE *f, bool append) {")
        + function(source, "FileInterfaceAndroid::FileInterfaceAndroid(zip_file *zf, bool append, int start, int p4, int p5) {")
        + function(source, "uint32_t FileInterfaceAndroid::Seek(uint32_t n) {")
        + state
        + function(source, "uint32_t FileInterfaceAndroid::FileExist(String name) {")
        + read_state
        + function(source, "void *FileInterfaceAndroid::OpenRead(String name, int p2, bool p3, int p4, int p5, unsigned int p6) {")
        + next(line + "\n" for line in source.splitlines()
               if line.startswith("static const char *gModeWb = "))
        + function(source, "void *FileInterfaceAndroid::OpenWrite(String name, int, bool, unsigned int) {")
        + function(loader, 'extern "C" int loadAPKAndZip(const char *apkPath, const char *patchPath) {')
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"file_android_readexist_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-Isrc", "-I", str(out),
            "tools/file_android_readexist_smoke.cpp", "src/game/core/String.cpp",
            "-o", str(exe),
        ], cwd=ROOT, check=True)
        with tempfile.TemporaryDirectory(dir=out) as root:
            subprocess.run([str(exe), root], cwd=ROOT, check=True)

    if args.apk:
        apk, obb = args.apk.resolve(), args.obb.resolve()
        if not apk.is_file() or not obb.is_file():
            parser.error("missing APK or OBB")
        try:
            with zipfile.ZipFile(apk) as archive:
                for name, (size, expected) in {
                    "lib/armeabi-v7a/libgof2hdaa.so": (2187328, "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"),
                    "assets/data/bin/items.bin": (27628, "639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c"),
                }.items():
                    if archive.getinfo(name).file_size != size or hashlib.sha256(archive.read(name)).hexdigest() != expected:
                        parser.error(f"APK entry mismatch: {name}")
            with zipfile.ZipFile(obb) as archive:
                for name, (size, expected) in {
                    "47947_GOF2CONTENT_ETC/assets/data/bin/items.bin": (27628, "55b953629d994d553604418988d43d2b1935f93061da7040a464acad57677e95"),
                    "47947_GOF2CONTENT_ETC/assets/data/bin/stations.bin": (3408, "761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3"),
                }.items():
                    if archive.getinfo(name).file_size != size or hashlib.sha256(archive.read(name)).hexdigest() != expected:
                        parser.error(f"OBB entry mismatch: {name}")
        except (OSError, zipfile.BadZipFile, KeyError) as exc:
            parser.error(f"invalid APK/OBB: {exc}")
        for opt in ("-O0", "-O2"):
            exe = out / f"file_android_real_archive_{opt[1:]}.exe"
            subprocess.run([
                args.cxx, "-std=gnu++17", opt, "-Isrc", "-Ithird_party/crypto",
                "-I", str(out), "tools/file_android_real_archive_smoke.cpp",
                "src/game/core/String.cpp", "src/engine/file/AEFile.cpp",
                "src/engine/file/AENormalFile.cpp", "src/engine/file/AEPakFile.cpp",
                "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                "-larchive", "-lcrypto", "-o", str(exe),
            ], cwd=ROOT, check=True)
            with tempfile.TemporaryDirectory(dir=out) as save_root:
                subprocess.run([str(exe), str(apk), str(obb), save_root],
                               cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
