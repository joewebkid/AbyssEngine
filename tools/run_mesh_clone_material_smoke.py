"""Compile the production MeshCloneMaterial body against fixture owners."""

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
    fixture = (ROOT / "tools/mesh_clone_material_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", function(source, "void PaintCanvas::MeshCloneMaterial"))
    with tempfile.TemporaryDirectory(prefix="mesh-clone-material-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "mesh_clone_material_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "mesh_clone_material_smoke.exe"
        sources = [generated] + [ROOT / f"src/engine/{path}" for path in
                                 ("render/Material.cpp", "math/AEMath.cpp",
                                  "math/Matrix.cpp", "math/Vector.cpp")]
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-ffunction-sections",
                        "-fdata-sections", "-Isrc", *map(str, sources),
                        "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
