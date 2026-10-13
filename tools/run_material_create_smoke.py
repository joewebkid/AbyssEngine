"""Compile both production MaterialCreate bodies with fixture resources."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    fixture = (ROOT / "tools/material_create_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_SIMPLE", function(source, "void PaintCanvas::MaterialCreate(unsigned int &out"))
    fixture = fixture.replace("// PRODUCTION_RESOURCE", function(source, "void PaintCanvas::MaterialCreate(unsigned short resId"))
    with tempfile.TemporaryDirectory(prefix="material-create-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "material_create_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "material_create_smoke.exe"
        sources = [generated] + [ROOT / f"src/engine/{path}" for path in
                                 ("render/Material.cpp", "math/AEMath.cpp",
                                  "math/Matrix.cpp", "math/Vector.cpp")]
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-ffunction-sections",
                        "-fdata-sections", "-Isrc", *map(str, sources),
                        "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
