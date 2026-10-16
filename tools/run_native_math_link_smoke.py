"""Build and run the small native math regression without keeping artifacts."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    with tempfile.TemporaryDirectory(prefix="matrix-smoke-", dir=ROOT / "_work") as temp:
        exe = Path(temp) / "native_math_link_smoke.exe"
        sources = [ROOT / "tools/native_math_link_smoke.cpp"] + [
            ROOT / f"src/engine/math/{unit}.cpp" for unit in ("AEMath", "Matrix", "Vector")
        ]
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-ffunction-sections",
                        "-fdata-sections", "-Isrc", *map(str, sources),
                        "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("Native matrix/vector host smoke: OK")


if __name__ == "__main__":
    main()
