"""Run the production VBO upload body with recording GL fixtures."""

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
    fixture = (ROOT / "tools/mesh_vbo_intern_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", function(
        source, "int MeshConvertToVBOIntern(Mesh *m)"))
    with tempfile.TemporaryDirectory(prefix="mesh-vbo-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "mesh_vbo_intern_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "mesh_vbo_intern_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
