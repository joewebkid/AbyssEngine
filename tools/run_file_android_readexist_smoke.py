"""Exercise source-extracted Android archive and filesystem read paths."""

import argparse
from pathlib import Path
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
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


if __name__ == "__main__":
    main()
