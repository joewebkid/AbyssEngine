"""Exercise the production ship-parts loader with a synthetic record."""

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
    source = (ROOT / "src/engine/file/FileRead.cpp").read_text()
    (out / "ship_parts_under_test.inc").write_text(
        function(source, "Array<int32_t> *FileRead::loadShipParts(int32_t id) {")
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"ship_parts_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-Isrc", "-I", str(out),
            "tools/ship_parts_smoke.cpp", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
