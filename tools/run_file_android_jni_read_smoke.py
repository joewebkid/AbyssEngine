"""Compile the production JNI Read body against a small host JNI mock."""

import argparse
from pathlib import Path
import subprocess

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
    engine = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    (out / "file_android_jni_read_under_test.inc").write_text(
        "namespace AbyssEngine {\n"
        + function(engine, "String operator+(const String &a, const String &b) {")
        + "\n}\n"
        + function(source, "uint32_t FileInterfaceAndroid::Read(uint32_t n, void *buf) {")
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"file_android_jni_read_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-Isrc", "-Ithird_party/jni", "-I", str(out),
            "tools/file_android_jni_read_smoke.cpp", "src/game/core/String.cpp",
            "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
