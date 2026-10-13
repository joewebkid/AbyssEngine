"""Run Material constructors and queue reuse without retaining build output."""

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

    with tempfile.TemporaryDirectory(prefix="material-lifecycle-smoke-", dir=ROOT / "_work") as temp:
        exe = Path(temp) / "material_lifecycle_smoke.exe"
        sources = [ROOT / "tools/material_lifecycle_smoke.cpp"] + [
            ROOT / f"src/engine/{path}" for path in
            ("render/Material.cpp", "math/AEMath.cpp", "math/Matrix.cpp", "math/Vector.cpp")
        ]
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-ffunction-sections",
                        "-fdata-sections", "-Isrc", *map(str, sources),
                        "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
