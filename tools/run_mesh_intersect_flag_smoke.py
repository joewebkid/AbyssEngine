"""Compile and exercise the production MeshIntersect body in a host fixture."""

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

    source = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    fixture = (ROOT / "tools/mesh_intersect_flag_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_MESH_INTERSECT",
                              function(source, "Vec2 MeshIntersect("))
    with tempfile.TemporaryDirectory(prefix="mesh-intersect-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "mesh_intersect_flag_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "mesh_intersect_flag_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
