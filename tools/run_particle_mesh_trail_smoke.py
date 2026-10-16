"""Exercise the production ParticleSystemMesh::emitTrail body without a GPU."""

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

    source = (ROOT / "src/engine/render/ParticleSystemMesh.cpp").read_text()
    fixture = (ROOT / "tools/particle_mesh_trail_smoke.cpp").read_text()
    manager = (ROOT / "src/engine/render/ParticleSystemManager.cpp").read_text()
    bodies = "\n".join((
        function(manager, "void _psm_emitTrail(ParticleSystemMesh *self, int delta)"),
        function(source, "void ParticleSystemMesh::emit(int id)"),
        function(source, "void ParticleSystemMesh::emitTrail(int delta)"),
    ))
    with tempfile.TemporaryDirectory(prefix="particle-trail-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", bodies))
        exe = Path(temp) / "smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
